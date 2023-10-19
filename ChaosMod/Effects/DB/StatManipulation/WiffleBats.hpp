#pragma once
#include "Effects/ActiveEffect.hpp"

class WiffleBats final : public ActiveEffect
{
        using OnCollision = void(__fastcall*)(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2);

        std::unique_ptr<FunctionDetour<OnCollision>> detour = nullptr;

        static void __fastcall Detour(IServerImpl* impl, void* edx, SSPMunitionCollisionInfo* info, uint a2) {}

        void Begin() override { detour->Detour(Detour); }
        void End() override { detour->UnDetour(); }

    public:
        WiffleBats()
        {
            auto collision = reinterpret_cast<OnCollision>(
                GetProcAddress(GetModuleHandleA("server.dll"), "?SPMunitionCollision@IServerImpl@@UAEXABUSSPMunitionCollisionInfo@@I@Z"));
            detour = std::make_unique<FunctionDetour<OnCollision>>(collision);
        }

        DefaultEffectInfo("Wiffle Bats", EffectType::StatManipulation);
};
