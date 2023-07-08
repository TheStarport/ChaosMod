// #pragma once
//
// enum class TwitchOverlayMode : int
//{
//	ChatMessages,
//	OverlayIngame,
//	OverlayOBS
// };
//
// class TwitchVoting : public Component
//{
// private:
//	struct ChoosableEffect
//	{
//		ChoosableEffect(const EffectIdentifier& effectIdentifier, const std::string& name, int match)
//			: EffectIdentifier(effectIdentifier), EffectName(name), Match(match)
//		{
//		}
//
//		EffectIdentifier EffectIdentifier;
//		std::string EffectName;
//		int Match;
//		int ChanceVotes = 0;
//	};
//
//	bool enableTwitchVoting;
//
//	bool receivedHello = false;
//	bool receivedFirstPing = false;
//	bool hasReceivedResult = false;
//
//	int twitchSecsBeforeVoting;
//
//	HANDLE pipeHandle = INVALID_HANDLE_VALUE;
//
//	unsigned long long lastPing = GetTickCount64();
//	unsigned long long lastVotesFetchTime = GetTickCount64();
//
//	int NoPingRuns = 0;
//
//	bool isVotingRoundDone = true;
//	bool alternatedVotingRound = false;
//
//	TwitchOverlayMode TwitchOverlayMode;
//
//	bool enableTwitchChanceSystem;
//	bool enableVotingChanceSystemRetainChance;
//	bool enableTwitchRandomEffectVoteable;
//
//	std::array<BYTE, 3> textColor;
//
//	bool isVotingRunning = false;
//
//	std::vector<std::unique_ptr<ChoosableEffect>> effectChoices;
//
//	std::unique_ptr<EffectIdentifier> ChosenEffectIdentifier;
//	std::string GetPipeJson(std::string identifier, std::vector<std::string> params);
//
// protected:
//	TwitchVoting(const std::array<BYTE, 3>& TextColor);
//	virtual ~TwitchVoting() override;
//
// public:
//	virtual void OnModPauseCleanup() override;
//	virtual void OnRun() override;
//
//	bool IsEnabled() const;
//
//	bool HandleMsg(const std::string& msg);
//
//	void SendToPipe(std::string identifier, std::vector<std::string> params = {});
//
//	void ErrorOutWithMsg(const std::string&& msg);
//
//	template <class T>
//		requires std::is_base_of_v<Component, T>
//	friend struct ComponentHolder;
// };
