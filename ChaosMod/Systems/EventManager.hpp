#pragma once

class EventManager : public Singleton<EventManager>
{
        using OnLaunch = void(__fastcall*)(IServerImpl* impl, void* edx, uint client, uint ship);

        static std::unique_ptr<FunctionDetour<OnLaunch>> launchDetour;

        static void __fastcall PlayerLaunchDetour(IServerImpl* impl, void* edx, uint client, uint ship);

    public:
        void SetupDetours() const;
};
