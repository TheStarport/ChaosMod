// #include <PCH.hpp>
//
// #include "EffectDispatcher.hpp"
// #include "TwitchVoting.hpp"
//
// #define BUFFER_SIZE           256
// #define VOTINPROXY_START_ARGS L"chaosmod\\TwitchChatVotingProxy.exe --startProxy"
//
// TwitchVoting::TwitchVoting(const std::array<BYTE, 3>& textColor) : Component(), textColor(textColor)
//{
//     enableTwitchVoting = OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTINENABLED);
//
//     if (!enableTwitchVoting)
//     {
//         return;
//     }
//
//     if (std::count_if(EnabledEffects.begin(), EnabledEffects.end(), [](const auto& pair) { return !pair.second.IsExcludedFromVoting(); }) < 3)
//     {
//         ErrorOutWithMsg("You need at least 3 enabled effects (which are not excluded from voting) to enable Twitch "
//                         "voting. Reverting to normal mode.");
//
//         return;
//     }
//
//     // A previous instance of the voting proxy could still be running, wait for it to release the mutex
//     HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"ChaosModVVotingMutex");
//     if (hMutex)
//     {
//         WaitForSingleObject(hMutex, INFINITE);
//         ReleaseMutex(hMutex);
//         CloseHandle(hMutex);
//     }
//
//     twitchSecsBeforeVoting = OptionsManager.GetTwitchValue<int>("TwitchVotingSecsBeforeVoting", OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);
//
//     TwitchOverlayMode =
//         OptionsManager.GetTwitchValue<TwitchOverlayMode>("TwitchVotingOverlayMode", static_cast<TwitchOverlayMode>(OPTION_DEFAULT_TWITCH_OVERLAY_MODE));
//
//     enableTwitchChanceSystem = OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystem", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING);
//     enableVotingChanceSystemRetainChance =
//         OptionsManager.GetTwitchValue<bool>("TwitchVotingChanceSystemRetainChance", OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTINRETAIN_CHANCE);
//
//     enableTwitchRandomEffectVoteable = OptionsManager.GetTwitchValue<bool>("TwitchRandomEffectVoteableEnable", OPTION_DEFAULT_TWITCH_RANDOEFFECT);
//
//     STARTUPINFO startupInfo = {};
//     PROCESS_INFORMATION procInfo = {};
//
//     auto str = _wcsdup(VOTINPROXY_START_ARGS);
// #ifdef _DEBUG
//     DWORD attributes = NULL;
//     if (DoesFileExist("chaosmod\\.forcenovotingconsole"))
//     {
//         attributes = CREATE_NO_WINDOW;
//     }
//
//     bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, attributes, NULL, NULL, &startupInfo, &procInfo);
// #else
//     bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
// #endif
//     free(str);
//
//     if (!result)
//     {
//         ErrorOutWithMsg((std::ostringstream() << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError()
//                                               << "). Please verify the file exists. Reverting to normal mode.")
//                             .str());
//
//         return;
//     }
//
//     pipeHandle = CreateNamedPipe(L"\\\\.\\pipe\\ChaosModVTwitchChatPipe",
//                                  PIPE_ACCESS_DUPLEX,
//                                  PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
//                                  1,
//                                  BUFFER_SIZE,
//                                  BUFFER_SIZE,
//                                  0,
//                                  NULL);
//
//     if (pipeHandle == INVALID_HANDLE_VALUE)
//     {
//         ErrorOutWithMsg("Error while creating a named pipe, previous instance of voting proxy might be running. Try "
//                         "reloading the mod. Reverting to normal mode.");
//
//         return;
//     }
//
//     ConnectNamedPipe(pipeHandle, NULL);
// }
//
// TwitchVoting::~TwitchVoting() { OnModPauseCleanup(); }
//
// void TwitchVoting::OnModPauseCleanup()
//{
//     if (pipeHandle != INVALID_HANDLE_VALUE)
//     {
//         FlushFileBuffers(pipeHandle);
//         DisconnectNamedPipe(pipeHandle);
//         CloseHandle(pipeHandle);
//
//         pipeHandle = INVALID_HANDLE_VALUE;
//     }
// }
//
// void TwitchVoting::OnRun()
//{
//     if (!enableTwitchVoting || !ComponentExists<EffectDispatcher>())
//     {
//         return;
//     }
//
//     GetComponent<EffectDispatcher>()->DispatchEffectsOnTimer = false;
//
//     // Check if there's been no ping for too long and error out
//     // Also if the chance system is enabled, get current vote status every second (if shown on screen)
//     auto curTick = GetTickCount64();
//     if (lastPing < curTick - 1000)
//     {
//         if (NoPingRuns == 5)
//         {
//             ErrorOutWithMsg("Connection to TwitchChatVotingProxy aborted. Returning to normal mode.");
//
//             return;
//         }
//
//         NoPingRuns++;
//         lastPing = curTick;
//     }
//
//     if (lastVotesFetchTime < curTick - 500)
//     {
//         lastVotesFetchTime = curTick;
//
//         if (isVotingRunning && enableTwitchChanceSystem && TwitchOverlayMode == TwitchOverlayMode::OverlayIngame)
//         {
//             // Get current vote status to display procentages on screen
//             SendToPipe("getcurrentvotes");
//         }
//     }
//
//     char buffer[BUFFER_SIZE];
//     DWORD bytesRead;
//     if (!ReadFile(pipeHandle, buffer, BUFFER_SIZE, &bytesRead, NULL))
//     {
//         while (GetLastError() == ERROR_IO_PENDING)
//         {
//             // WAIT(0);
//         }
//     }
//
//     if (bytesRead > 0)
//     {
//         if (!HandleMsg(std::string(buffer)))
//         {
//             return;
//         }
//     }
//
//     if (!receivedHello)
//     {
//         return;
//     }
//
//     if (GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= 1 && !hasReceivedResult)
//     {
//         // Get vote result 1 second before effect is supposed to dispatch
//
//         if (isVotingRunning)
//         {
//             isVotingRunning = false;
//
//             SendToPipe("getvoteresult");
//         }
//     }
//     else if (GetComponent<EffectDispatcher>()->ShouldDispatchEffectNow())
//     {
//         // End of voting round; dispatch resulted effect
//
//         // Should be random effect voteable, so just dispatch random effect
//         if (ChosenEffectIdentifier->GetEffectId().empty())
//         {
//             GetComponent<EffectDispatcher>()->DispatchRandomEffect();
//         }
//         else
//         {
//             GetComponent<EffectDispatcher>()->DispatchEffect(*ChosenEffectIdentifier);
//         }
//         GetComponent<EffectDispatcher>()->ResetTimer();
//
//         if (ComponentExists<MetaModifiers>())
//         {
//             for (int i = 0; i < GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch; i++)
//             {
//                 GetComponent<EffectDispatcher>()->DispatchRandomEffect();
//             }
//         }
//
//         isVotingRoundDone = true;
//     }
//     else if (!isVotingRunning && receivedFirstPing &&
//              (twitchSecsBeforeVoting == 0 || GetComponent<EffectDispatcher>()->GetRemainingTimerTime() <= twitchSecsBeforeVoting) && isVotingRoundDone)
//     {
//         // New voting round
//
//         isVotingRunning = true;
//         hasReceivedResult = false;
//         isVotingRoundDone = false;
//
//         ChosenEffectIdentifier = std::make_unique<EffectIdentifier>();
//
//         effectChoices.clear();
//         std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> ChoosableEffects;
//         for (auto& pair : EnabledEffects)
//         {
//             auto& [effectIdentifier, effectData] = pair;
//
//             if (!effectData.IsMeta() && !effectData.IsExcludedFromVoting() && !effectData.IsUtility() && !effectData.IsHidden())
//             {
//                 ChoosableEffects.emplace(effectIdentifier, effectData);
//             }
//         }
//
//         for (int idx = 0; idx < 4; idx++)
//         {
//             // 4th voteable is for random effect (if enabled)
//             if (idx == 3)
//             {
//                 if (enableTwitchRandomEffectVoteable)
//                 {
//                     effectChoices.push_back(std::make_unique<ChoosableEffect>(EffectIdentifier(), "Random Effect", !alternatedVotingRound ? 4 : 8));
//                 }
//
//                 break;
//             }
//
//             float totalWeight = 0.f;
//             for (const auto& pair : ChoosableEffects)
//             {
//                 const EffectData& effectData = pair.second;
//
//                 totalWeight += GetEffectWeight(effectData);
//             }
//
//             float chosen = Random.GetRandomFloat(0.f, totalWeight);
//
//             totalWeight = 0.f;
//
//             std::unique_ptr<ChoosableEffect> pTargetChoice;
//
//             for (auto& pair : ChoosableEffects)
//             {
//                 auto& [effectIdentifier, effectData] = pair;
//
//                 totalWeight += GetEffectWeight(effectData);
//
//                 if (chosen <= totalWeight)
//                 {
//                     // Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by
//                     // EffectWeightMult
//                     effectData.Weight = 0;
//
//                     pTargetChoice = std::make_unique<ChoosableEffect>(effectIdentifier,
//                                                                       effectData.HasCustomName() ? effectData.CustomName : effectData.Name,
//                                                                       !alternatedVotingRound             ? idx + 1
//                                                                       : enableTwitchRandomEffectVoteable ? idx + 5
//                                                                                                          : idx + 4);
//                     break;
//                 }
//             }
//
//             EffectIdentifier effectIdentifier = pTargetChoice->EffectIdentifier;
//
//             effectChoices.push_back(std::move(pTargetChoice));
//             ChoosableEffects.erase(effectIdentifier);
//         }
//
//         std::vector<std::string> effectNames;
//         for (const auto& pChoosableEffect : effectChoices)
//         {
//             effectNames.push_back(pChoosableEffect->EffectName);
//         }
//
//         SendToPipe("vote", effectNames);
//
//         alternatedVotingRound = !alternatedVotingRound;
//     }
//
//     if (isVotingRunning && TwitchOverlayMode == TwitchOverlayMode::OverlayIngame)
//     {
//         // Print voteables on screen
//
//         // Count total votes if chance system is enabled
//         int totalVotes = 0;
//         if (enableTwitchChanceSystem)
//         {
//             for (const auto& pChoosableEffect : effectChoices)
//             {
//                 int chanceVotes = pChoosableEffect->ChanceVotes + (enableVotingChanceSystemRetainChance ? 1 : 0);
//
//                 totalVotes += chanceVotes;
//             }
//         }
//
//         float y = .1f;
//         for (const auto& pChoosableEffect : effectChoices)
//         {
//             std::ostringstream oss;
//             oss << pChoosableEffect->Match << ": " << pChoosableEffect->EffectName;
//
//             // Also show chance percentages if chance system is enabled
//             if (enableTwitchChanceSystem)
//             {
//                 float percentage;
//                 if (totalVotes == 0)
//                 {
//                     percentage = 100 / effectChoices.size() * .01f;
//                 }
//                 else
//                 {
//                     int chanceVotes = pChoosableEffect->ChanceVotes + (enableVotingChanceSystemRetainChance ? 1 : 0);
//
//                     percentage = !chanceVotes ? .0f : std::roundf(static_cast<float>(chanceVotes) / static_cast<float>(totalVotes) * 100.f) / 100.f;
//                 }
//
//                 oss << " (" << percentage * 100.f << "%)";
//             }
//
//             oss << std::endl;
//
//             DrawScreenText(oss.str(), { .95f, y }, .41f, { textColor[0], textColor[1], textColor[2] }, true, ScreenTextAdjust::Right, { .0f, .95f }, true);
//
//             y += .05f;
//         }
//     }
// }
//
// bool TwitchVoting::IsEnabled() const { return enableTwitchVoting; }
//
// bool TwitchVoting::HandleMsg(const std::string& msg)
//{
//     if (msg == "hello")
//     {
//         receivedHello = true;
//
//         LOG("Received hello from voting proxy");
//     }
//     else if (msg == "ping")
//     {
//         lastPing = GetTickCount64();
//         NoPingRuns = 0;
//         receivedFirstPing = true;
//     }
//     else if (msg == "invalid_login")
//     {
//         ErrorOutWithMsg("Invalid Twitch Credentials. Please verify your config. Reverting to normal mode.");
//
//         return false;
//     }
//     else if (msg == "invalid_channel")
//     {
//         ErrorOutWithMsg("Invalid Twitch Channel. Please verify your config. Reverting to normal mode.");
//
//         return false;
//     }
//     else
//     {
//         nlohmann::json receivedJSON = nlohmann::json::parse(msg);
//         if (!receivedJSON.empty())
//         {
//             std::string identifier = receivedJSON["Identifier"];
//             if (identifier == "voteresult")
//             {
//                 int result = receivedJSON["SelectedOption"];
//
//                 hasReceivedResult = true;
//
//                 // If random effect voteable (result == 3) won, dispatch random effect later
//                 ChosenEffectIdentifier = std::make_unique<EffectIdentifier>(result == 3 ? EffectIdentifier() : effectChoices[result]->EffectIdentifier);
//             }
//             else if (identifier == "currentvotes")
//             {
//                 std::vector<int> options = receivedJSON["Votes"];
//                 if (options.size() == effectChoices.size())
//                 {
//                     for (int idx = 0; idx < options.size(); idx++)
//                     {
//                         int votes = options[idx];
//                         effectChoices[idx]->ChanceVotes = votes;
//                     }
//                 }
//             }
//         }
//     }
//
//     return true;
// }
//
// std::string TwitchVoting::GetPipeJson(std::string identifier, std::vector<std::string> params)
//{
//     nlohmann::json finalJSON;
//     finalJSON["Identifier"] = identifier;
//     finalJSON["Options"] = params;
//     return finalJSON.dump();
// }
//
// void TwitchVoting::SendToPipe(std::string identifier, std::vector<std::string> params)
//{
//     auto msg = GetPipeJson(identifier, params);
//     msg += "\n";
//     WriteFile(pipeHandle, msg.c_str(), msg.length(), NULL, NULL);
// }
//
// void TwitchVoting::ErrorOutWithMsg(const std::string&& msg)
//{
//     std::wstring wStr = { msg.begin(), msg.end() };
//     MessageBox(NULL, wStr.c_str(), L"ChaosModV Error", MB_OK | MB_ICONERROR);
//
//     DisconnectNamedPipe(pipeHandle);
//     CloseHandle(pipeHandle);
//     pipeHandle = INVALID_HANDLE_VALUE;
//
//     if (ComponentExists<EffectDispatcher>())
//     {
//         GetComponent<EffectDispatcher>()->DispatchEffectsOnTimer = true;
//     }
//     enableTwitchVoting = false;
// }
