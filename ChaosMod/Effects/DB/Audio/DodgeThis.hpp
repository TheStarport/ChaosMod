#pragma once
#include "Effects/ActiveEffect.hpp"

class DodgeThis final : public ActiveEffect
{
        using OnCollision = void(__fastcall*)(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2);

        inline static std::unique_ptr<FunctionDetour<OnCollision>> detour = nullptr;
        inline static std::array<uint, 6> sounds = {
            CreateID("chaos_bat_1"), CreateID("chaos_bat_2"), CreateID("chaos_bat_3"),
            CreateID("chaos_bat_4"), CreateID("chaos_bat_5"), CreateID("chaos_bat_6"),
        };

        static void __fastcall Detour(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2)
        {
            pub::Audio::PlaySoundEffect(0, sounds[Random::i()->Uniform(0u, sounds.size() - 1)]);

            detour->UnDetour();
            detour->GetOriginalFunc()(impl, edx, info, a2);
            detour->Detour(Detour);
        }

        void Begin() override { detour->Detour(Detour); }
        void End() override { detour->UnDetour(); }

    public:
        DodgeThis()
        {
            auto collision = reinterpret_cast<OnCollision>(
                GetProcAddress(GetModuleHandleA("server.dll"), "?SPMunitionCollision@IServerImpl@@UAEXABUSSPMunitionCollisionInfo@@I@Z"));
            detour = std::make_unique<FunctionDetour<OnCollision>>(collision);
        }

        DefaultEffectInfo("Dodge This", EffectType::Audio);
};
