#pragma once
#include "../Effects/ActiveEffect.hpp"

#include "ChaosMod.hpp"
#include "Components/Component.hpp"

#include <concurrentqueue.h>
#include <zmq_addon.hpp>

struct PipeMessage
{
    std::string identifier;
    std::vector<std::string> params;
};

class TwitchVoting final : public Component
{
    public:
        struct VoteInfo
        {
                int totalVotes = 0;
                std::array<int, 4> votes = {};
                std::array<float, 4> votePercentages = {};
        };

    private:
        HANDLE votingProxyProcessHandle = nullptr;
        zmq::context_t mqContext;
        std::unique_ptr<zmq::socket_t> sock;

        // Send-Receive payloads from another thread
        std::jthread voteThread;
        moodycamel::ConcurrentQueue<PipeMessage> commandQueue;
        moodycamel::ConcurrentQueue<std::function<void()>> responseQueue;

        u64 lastPing = 0;
        u64 lastVoteFetch = 0;
        int noPingRuns = 0;
        int selectedResult = -1;

        bool handshakeCompleted = false;
        bool hasReceivedResult = false;

        bool isVotingRoundDone = true;
        bool alternatedVotingRound = true;

        bool isVotingRunning = false;

        VoteInfo voteInfo{};

        std::vector<ActiveEffect*> effectSelection;

        bool SpawnVotingProxy();
        void HandleMsg(std::string_view message);
        void SendToSocket(std::string_view identifier, const std::vector<std::string>& params = {});
        void Cleanup();
        void HandleSocketPayloads(const std::stop_token& t);

    public:
        ~TwitchVoting() override;
        bool Initialize();
        void Poll();
        const VoteInfo& GetCurrentVoteInfo() const;
        bool IsInitialized() const;
};
