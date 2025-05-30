#include "PCH.hpp"

#include "CoreComponents/TwitchVoting.hpp"

#include "Components/ConfigManager.hpp"
#include "Components/UiManager.hpp"
#include "CoreComponents/ChaosTimer.hpp"

#include "ImGui/ImGuiManager.hpp"

#include <nlohmann/json.hpp>

#include <winsock.h>

std::string TwitchVoting::GetPipeJson(std::string_view identifier, const std::vector<std::string>& params)
{
    nlohmann::json finalJSON;
    finalJSON["identifier"] = identifier;
    finalJSON["options"] = params;
    return finalJSON.dump();
}

bool TwitchVoting::SpawnVotingProxy()
{
    // Check if the voting proxy is already running
    const auto mutex = CreateMutexA(nullptr, FALSE, "ChaosModVotingMutex");
    if (!mutex)
    {
        // It's already open, no need to do anything
        return true;
    }

    // Clear the mutex before we start the process
    ReleaseMutex(mutex);

    votingProxyProcessHandle = nullptr;

    STARTUPINFOA startupInfo = {};
    PROCESS_INFORMATION procInfo = {};

    std::string votingProxyArgs = "utilities/VotingProxy.exe --startProxy";

    const auto config = Get<ConfigManager>();

    if (const bool result =
            CreateProcessA(nullptr, votingProxyArgs.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &procInfo);
        !result)
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

    // Sleep for 0.5 second to give the proxy time to start
    Sleep(500);

    votingProxyProcessHandle = procInfo.hProcess;

    return true;
}

void TwitchVoting::Cleanup()
{
    voteThread.request_stop();
    voteThread.join();
    sock.reset();

    // Backup to ensure no dangling process
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    BOOL res = Process32First(snapShot, &entry);
    while (res)
    {
        if (wcscmp(entry.szExeFile, L"VotingProxy.exe") == 0)
        {
            if (const auto hProcess = OpenProcess(PROCESS_TERMINATE, 0, entry.th32ProcessID); hProcess != nullptr)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        res = Process32Next(snapShot, &entry);
    }
    CloseHandle(snapShot);
}

void TwitchVoting::HandleSocketPayloads(const std::stop_token& t)
{
    while (!t.stop_requested())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::string command;
        while (commandQueue.try_dequeue(command))
        {
            sock->send(zmq::message_t(command), zmq::send_flags::none);

            zmq::message_t reply;
            if (const auto res = sock->recv(reply); res && *res > 0)
            {
                const auto data = reply.to_string();
#ifdef _DEBUG
                static bool loggedPing = false;
                if (strcmp(data.c_str(), "pong") == 0)
                {
                    if (!loggedPing)
                    {
                        loggedPing = true;
                        Log(data);
                    }
                }
                else
                {
                    Log(data);
                }
#endif

                responseQueue.enqueue([this, data]() { HandleMsg(data); });
            }
        }
    }
}

TwitchVoting::~TwitchVoting() { Cleanup(); }
constexpr auto BufferSize = 256u;

bool TwitchVoting::Initialize()
{
    if (!SpawnVotingProxy())
    {
        return false;
    }

    constexpr std::string_view socketUri = "tcp://localhost:5657";

    try
    {
        // Try binding first, otherwise connect
        sock = std::make_unique<zmq::socket_t>(mqContext, zmq::socket_type::req);
        sock->bind(socketUri.data());
    }
    catch (const std::exception&)
    {
        try
        {
            sock->connect(socketUri.data());
        }
        catch (const std::exception&)
        {
            Log("Unable to bind or connect to req/res port (*:5657)");
            return false;
        }
    }

    SendToSocket("hello");
    voteThread = std::jthread(std::bind_front(&TwitchVoting::HandleSocketPayloads, this));
    return true;
}

void TwitchVoting::Poll()
{
    if (!sock || sock->handle() == nullptr)
    {
        return;
    }

    if (!Get<ConfigManager>()->chaosSettings.enableTwitchVoting)
    {
        Cleanup();
        return;
    }

    std::function<void()> func;
    while (responseQueue.try_dequeue(func))
    {
        func();
    }

    // Check if there's been no ping for too long and error out
    // Also if the chance system is enabled, get current vote status every second (if shown on screen)
    const auto curTick = GetTickCount64();
    if (lastPing < curTick - 1000)
    {
        SendToSocket("ping");

        if (noPingRuns++ == 6)
        {
            Log("Connection to voting proxy process lost");
            // Cleanup();
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
            SendToSocket("getcurrentvotes");
        }
    }

    if (!handshakeCompleted)
    {
        return;
    }

    if (const auto timeUntilChaos = Get<ChaosTimer>()->GetTimeUntilChaos(); timeUntilChaos <= 1.0f)
    {
        if (!hasReceivedResult && isVotingRunning)
        {
            isVotingRunning = false;
            SendToSocket("getvoteresult");
        }
        else if (hasReceivedResult)
        {
            isVotingRoundDone = true;

            if (Get<ChaosTimer>()->DoubleTimeActive())
            {
                Get<ChaosTimer>()->TriggerChaos();
            }

            if (selectedResult == 3) // Twitch selected Random
            {
                Get<ChaosTimer>()->TriggerChaos();
                return;
            }

            if (const float value = Get<Random>()->UniformFloat(0.0f, 1.0f); value <= Get<ConfigManager>()->chaosSettings.baseTwitchVoteWeight)
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

                Get<ChaosTimer>()->TriggerChaos(effect);
                return;
            }

            Get<ChaosTimer>()->TriggerChaos();
        }
    }
    else if (!isVotingRunning && isVotingRoundDone)
    {
        isVotingRunning = true;
        hasReceivedResult = false;
        isVotingRoundDone = false;

        effectSelection = Get<ChaosTimer>()->GetNextEffects();

        int index = alternatedVotingRound ? 1 : 5;

        std::vector<std::string> effectNames;

        for (const ActiveEffect* effect : effectSelection)
        {
            auto info = effect->GetEffectInfo();
            effectNames.push_back(std::format("{}.) {}", index++, info.effectName));
        }

        effectNames.emplace_back(std::format("{}.) Random Effect", alternatedVotingRound ? 4 : 8));

        SendToSocket("vote", effectNames);

        ImGuiManager::SetVotingChoices(effectNames);

        alternatedVotingRound = !alternatedVotingRound;
    }
}

const TwitchVoting::VoteInfo& TwitchVoting::GetCurrentVoteInfo() const { return voteInfo; }

bool TwitchVoting::IsInitialized() const { return handshakeCompleted; }

void TwitchVoting::SendToSocket(const std::string_view identifier, const std::vector<std::string>& params)
{
    commandQueue.enqueue(GetPipeJson(identifier, params));
}

void TwitchVoting::HandleMsg(std::string_view message)
{
    if (message == "hello back" && !handshakeCompleted)
    {
        handshakeCompleted = true;
        noPingRuns = 0;

        Log("Voting socket has completed the handshake!");
    }
    else if (message == "pong")
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

        if (const std::string identifier = receivedJson["identifier"]; identifier == "voteresult")
        {
            int result = receivedJson["selectedOption"];

            hasReceivedResult = true;

            Log(std::format("Pipe (voteresult): {}", result));
            selectedResult = result;
        }
        else if (identifier == "currentvotes")
        {
            const std::vector<int> votes = receivedJson["votes"];
            const std::vector<float> votePercentages = receivedJson["votePercentages"];
            voteInfo.totalVotes = receivedJson["totalVotes"];
            if (votes.size() == voteInfo.votes.size())
            {
                for (uint idx = 0; idx < voteInfo.votes.size(); idx++)
                {
                    voteInfo.votes[idx] = votes[idx];
                    voteInfo.votePercentages[idx] = votePercentages[idx];
                }
            }
        }
        else if (identifier == "error")
        {
            Log(std::format("Error from pipe! {}", receivedJson["message"].get<std::string>()));
        }
    }
}
