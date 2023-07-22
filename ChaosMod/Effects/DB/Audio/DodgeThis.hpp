#pragma once
#include "Effects/ActiveEffect.hpp"

class DodgeThis final : public ActiveEffect
{
        using OnCollision = void(__fastcall*)(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2);

        inline static std::unique_ptr<FunctionDetour<OnCollision>> detour = nullptr;
        inline static bool alt = false;
        inline static uint sound1 = CreateID("chaos_kickball_1");
        inline static uint sound2 = CreateID("chaos_kickball_2");

        static void __fastcall Detour(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2)
        {
            pub::Audio::PlaySoundEffect(0, alt ? sound1 : sound2);
            alt = !alt;

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

        DefEffectInfo("Dodge This", 1.0f, EffectType::Audio);
};
