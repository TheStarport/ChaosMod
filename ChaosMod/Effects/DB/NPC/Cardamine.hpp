#pragma once
#include "Effects/ActiveEffect.hpp"

class Cardamine final : public ActiveEffect
{
        using SendCommType = int(__cdecl*)(uint, uint, uint, const Costume*, uint, uint*, int, uint, float, bool);
        inline static const std::unique_ptr<FunctionDetour<SendCommType>> detour = std::make_unique<FunctionDetour<SendCommType>>(pub::SpaceObj::SendComm);

        static int Detour(const uint fromShipId, const uint toShipId, uint voiceId, const Costume* costume, const uint infocardId, uint* voiceLineId,
                          const int a6, const uint infocardId2, const float a8, const bool a9)
        {
            uint voiceLine = Random::i()->Uniform(0u, 1u) ? CreateID("gcs_fidget_traderumor_01-") : CreateID("gcs_fidget_trade_simple_01-");
            detour->UnDetour();
            const int i = pub::SpaceObj::SendComm(fromShipId, toShipId, 2328361551, costume, infocardId, &voiceLine, a6, infocardId2, a8, a9);
            detour->Detour(Detour);
            return i;
        }

        void Begin() override { detour->Detour(Detour); }
        void End() override { detour->UnDetour(); }

    public:
        DefEffectInfo("Cardamine, Cardamine.", 2.0f, EffectType::Npc);
};
