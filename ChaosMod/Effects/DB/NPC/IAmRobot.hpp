#pragma once
#include "Effects/ActiveEffect.hpp"

class IAmRobot final : public ActiveEffect
{
        using SendCommType = int (*)(unsigned int, unsigned int, unsigned int, Costume *, unsigned int, unsigned int *, int, unsigned int, float, bool);
        inline static std::unique_ptr<FunctionDetour<SendCommType>> sendCommDetour;

        static int Detour(const unsigned int a1, const unsigned int a2, const unsigned int a3, [[maybe_unused]] Costume *a4, const unsigned int a5,
                          unsigned int *a6, const int a7, const unsigned int a8, const float a9, const bool a0)
        {
            Costume replacement = { 0, CreateID("robot_body_a"), CreateID("robot_hand_left"), CreateID("robot_hand_right") };
            sendCommDetour->UnDetour();
            const auto i = sendCommDetour->GetOriginalFunc()(a1, a2, a3, &replacement, a5, a6, a7, a8, a9, a0);
            sendCommDetour->Detour(Detour);
            return i;
        }

        void Begin() override { sendCommDetour->Detour(Detour); }
        void End() override { sendCommDetour->UnDetour(); }

    public:
        IAmRobot()
        {
            auto orig = reinterpret_cast<SendCommType>(GetProcAddress(GetModuleHandleA("server.dll"), "?SendComm@SpaceObj@pub@@YAHIIIPBUCostume@@IPAIHIM_N@Z"));
            sendCommDetour = std::make_unique<FunctionDetour<SendCommType>>(orig);
        }

        DefaultEffectInfo("I Am Robot", EffectType::Npc);
};
