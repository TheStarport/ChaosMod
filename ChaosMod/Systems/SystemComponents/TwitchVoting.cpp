#include "PCH.hpp"

#include "TwitchVoting.hpp"

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/ImguiComponents/ImGuiManager.hpp"
#include "Systems/UiManager.hpp"

#include <nlohmann/json.hpp>

std::string TwitchVoting::GetPipeJson(std::string_view identifier, std::vector<std::string> params)
{
    nlohmann::json finalJSON;
    finalJSON["Identifier"] = identifier;
    finalJSON["Options"] = params;
    return finalJSON.dump();
}

bool TwitchVoting::SpawnVotingProxy()
{
    // A previous instance of the voting proxy could still be running, wait for it to release the mutex
    if (const auto mutex = OpenMutex(SYNCHRONIZE, FALSE, L"ChaosModVVotingMutex"))
    {
        WaitForSingleObject(mutex, INFINITE);
        ReleaseMutex(mutex);
        CloseHandle(mutex);
    }

    STARTUPINFOA startupInfo = {};
    PROCESS_INFORMATION procInfo = {};

    static char votingProxyArgs[] = "TwitchChatVotingProxy.exe --startProxy";

#ifdef _DEBUG
    DWORD attributes = NULL;
    if (std::filesystem::exists(".forcenovotingconsole"))
    {
        attributes = CREATE_NO_WINDOW;
    }

    bool result = CreateProcessA(nullptr, votingProxyArgs, nullptr, nullptr, TRUE, attributes, nullptr, nullptr, &startupInfo, &procInfo);
#else
    bool result = CreateProcessA(nullptr, votingProxyArgs, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &procInfo);
#endif

    if (!result)
    {
        LPSTR messageBuffer = nullptr;
        const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                           nullptr,
                                           GetLastError(),
                                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                           reinterpret_cast<LPSTR>(&messageBuffer),
                                           0,
                                           nullptr);

        std::string message(messageBuffer, size);

        LocalFree(messageBuffer);
        Log(std::format("Error while starting voting proxy. Error ({}): {}", GetLastError(), message));
        return false;
    }

    // Sleep for 1 second to give the proxy time to start
    Sleep(1000);

    return true;
}

void TwitchVoting::Cleanup()
{
    if (pipeHandle != INVALID_HANDLE_VALUE)
    {
        FlushFileBuffers(pipeHandle);
        DisconnectNamedPipe(pipeHandle);
        CloseHandle(pipeHandle);

        pipeHandle = INVALID_HANDLE_VALUE;
    }
}

TwitchVoting::~TwitchVoting() { Cleanup(); }
constexpr auto BufferSize = 256u;

bool TwitchVoting::Initialize()
{
    SpawnVotingProxy();

    pipeHandle = CreateNamedPipe(
        LR"(\\.\pipe\ChaosModVVotingPipe)", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1, BufferSize, BufferSize, 0, nullptr);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        Log("Error while creating a named pipe, previous instance of voting proxy might be running.");
        return false;
    }

    ConnectNamedPipe(pipeHandle, nullptr);

    return true;
}

void TwitchVoting::Poll()
{
    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        return;
    }

    if (!ConfigManager::i()->enableTwitchVoting)
    {
        if (pipeHandle != INVALID_HANDLE_VALUE)
        {
            Cleanup();
        }

        return;
    }

    // Check if there's been no ping for too long and error out
    // Also if the chance system is enabled, get current vote status every second (if shown on screen)
    const auto curTick = GetTickCount64();
    if (lastPing < curTick - 1000)
    {
        if (noPingRuns++ == 5)
        {
            Log("Connection to voting proxy process lost. Check chaosmod/chaosproxy.log for more information.");
            return;
        }

        lastPing = curTick;
    }

    if (lastVoteFetch < curTick - 500)
    {
        lastVoteFetch = curTick;

        if (isVotingRunning)
        {
            // Get current vote status to display percentages on screen
            SendToPipe("getcurrentvotes");
        }
    }

    char buffer[BufferSize];
    DWORD bytesRead;
    if (!ReadFile(pipeHandle, buffer, BufferSize, &bytesRead, nullptr))
    {
        return;
    }

    if (bytesRead > 0)
    {
        HandleMsg(buffer);
    }

    if (!receivedHello)
    {
        return;
    }

    if (const auto timeUntilChaos = ChaosTimer::i()->GetTimeUntilChaos(); timeUntilChaos <= 1.0f)
    {
        if (!hasReceivedResult && isVotingRunning)
        {
            isVotingRunning = false;
            SendToPipe("getvoteresult");
        }
        else if (hasReceivedResult)
        {
            isVotingRoundDone = true;
            if (selectedResult == 3) // Twitch selected Random
            {
                ChaosTimer::i()->TriggerChaos();
                return;
            }

            if (const float value = Random::i()->UniformFloat(0.0f, 1.0f); value <= ConfigManager::i()->baseTwitchVoteWeight)
            {
                auto* effect = effectSelection[selectedResult];
                if (!effect->CanSelect())
                {
                    pub::Audio::PlaySoundEffect(1, CreateID("ui_execute_transaction"));
                    Log("Twitch picked an effect that couldn't be selected. Stupid Twitch...");

                    const FmtStr fmt(458854, nullptr);
                    pub::Player::DisplayMissionMessage(1, fmt, MissionMessageType::Type2, false);
                    return;
                }

                ChaosTimer::i()->TriggerChaos(effect);
                return;
            }

            ChaosTimer::i()->TriggerChaos();
        }
    }
    else if (!isVotingRunning && isVotingRoundDone)
    {
        isVotingRunning = true;
        hasReceivedResult = false;
        isVotingRoundDone = false;

        effectSelection = ChaosTimer::i()->GetNextEffects();

        std::vector<std::string> effectNames;

        alternatedVotingRound = !alternatedVotingRound;

        int index = !alternatedVotingRound ? 1 : 5;
        for (ActiveEffect* effect : effectSelection)
        {
            auto info = effect->GetEffectInfo();
            effectNames.push_back(std::format("{}.) {}", index++, info.effectName));
        }

        effectNames.emplace_back(std::format("{}.) Random Effect", !alternatedVotingRound ? 4 : 8));

        SendToPipe("vote", effectNames);

        ImGuiManager::SetVotingChoices(effectNames);
    }
}

void TwitchVoting::SendToPipe(const std::string_view identifier, const std::vector<std::string>& params) const
{
    auto msg = GetPipeJson(identifier, params);
    msg += "\n";
    WriteFile(pipeHandle, msg.c_str(), msg.length(), nullptr, nullptr);
}

void TwitchVoting::HandleMsg(std::string_view message)
{
    if (message == "hello")
    {
        if (!receivedHello)
        {
            receivedHello = true;
            noPingRuns = 0;

            Log("Voting pipe has sent hello message!");
            SendToPipe("hello_back");
        }
    }
    else if (message == "ping")
    {
        lastPing = GetTickCount64();
        noPingRuns = 0;
    }
    else
    {
        auto receivedJson = nlohmann::json::parse(message);
        if (receivedJson.empty())
        {
            return;
        }

        const std::string identifier = receivedJson["Identifier"];
        if (identifier == "voteresult")
        {
            int result = receivedJson["SelectedOption"];

            hasReceivedResult = true;

            Log(std::format("Pipe (voteresult): {}", result));
            selectedResult = result;
        }
        else if (identifier == "currentvotes")
        {
            const std::vector<int> options = receivedJson["Votes"];
            if (options.size() == votes.size())
            {
                for (uint idx = 0; idx < options.size(); idx++)
                {
                    votes[idx] = options[idx];
                }
            }
        }
        else if (identifier == "error")
        {
            Log(std::format("Error from pipe! {}", receivedJson["Message"].get<std::string>()));
        }
    }
}
