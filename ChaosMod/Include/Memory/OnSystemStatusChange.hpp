#pragma once

class OnSystemStatusChange
{
        using OnJumpTunnelEnterType = bool(__fastcall*)(CShip* ship, void* edx, uint* jumpingShipId);
        using OnJumpInCompleteType = void (__fastcall*)(IServerImpl *impl, void* edx, unsigned int shipId, unsigned int client);
        inline static OnJumpTunnelEnterType onJumpTunnelEnter;
        inline static OnJumpInCompleteType onJumpInComplete;
        inline static std::unique_ptr<FunctionDetour<OnJumpTunnelEnterType>> detourJumpTunnelEnter = nullptr;
        inline static std::unique_ptr<FunctionDetour<OnJumpInCompleteType>> detourJumpInComplete = nullptr;

        static bool __fastcall OnJumpTunnelEnter(CShip* ship, void* edx, uint* jumpingShipId);
        static void __fastcall OnJumpInComplete(IServerImpl *impl, void* edx, unsigned int shipId, unsigned int client);

    public:
        OnSystemStatusChange() = delete;
        static void Init();
};
