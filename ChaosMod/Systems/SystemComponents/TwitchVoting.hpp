#pragma once
#include "Effects/ActiveEffect.hpp"

class TwitchVoting final : public Component
{
        void* pipeHandle = INVALID_HANDLE_VALUE;
        u64 lastPing = 0;
        u64 lastVoteFetch = 0;
        int noPingRuns = 0;
        int selectedResult = -1;

        bool receivedHello = false;
        bool hasReceivedResult = false;

        bool isVotingRoundDone = true;
        bool alternatedVotingRound = true;

        bool isVotingRunning = false;

        std::array<int, 4> votes = {};
        std::vector<ActiveEffect*> effectSelection;

        static std::string GetPipeJson(std::string_view identifier, std::vector<std::string> params);
        static bool SpawnVotingProxy();
        void HandleMsg(std::string_view message);
        void SendToPipe(std::string_view identifier, const std::vector<std::string>& params = {}) const;
        void Cleanup();

    public:
        ~TwitchVoting() override;
        bool Initialize();
        void Poll();
};
