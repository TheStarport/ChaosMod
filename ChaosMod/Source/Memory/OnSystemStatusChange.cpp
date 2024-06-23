// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming
#include "PCH.hpp"

#include "Memory/OnSystemStatusChange.hpp"

#include "CoreComponents/ChaosTimer.hpp"

bool __fastcall OnSystemStatusChange::OnJumpTunnelEnter(CShip* ship, void* edx, uint* jumpingShipId)
{
    if (Utils::GetCShip() == ship && *jumpingShipId == ship->id)
    {
        ChaosTimer::OnSystemUnload();
    }

    detourJumpTunnelEnter->UnDetour();
    const auto ret = detourJumpTunnelEnter->GetOriginalFunc()(ship, edx, jumpingShipId);
    detourJumpTunnelEnter->Detour(OnJumpTunnelEnter);
    return ret;
}

void __fastcall OnSystemStatusChange::OnJumpInComplete(IServerImpl* impl, void* edx, unsigned int shipId, unsigned int client)
{
    ChaosTimer::OnJumpInComplete();

    detourJumpInComplete->UnDetour();
    impl->JumpInComplete(shipId, client);
    detourJumpInComplete->Detour(OnJumpInComplete);
}

void OnSystemStatusChange::Init()
{
    onJumpTunnelEnter = reinterpret_cast<OnJumpTunnelEnterType>(reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0x54B10);
    detourJumpTunnelEnter = std::make_unique<FunctionDetour<OnJumpTunnelEnterType>>(onJumpTunnelEnter);
    detourJumpTunnelEnter->Detour(OnJumpTunnelEnter);

    onJumpInComplete = reinterpret_cast<OnJumpInCompleteType>(reinterpret_cast<DWORD>(GetModuleHandleA("server.dll")) + 0xFAE0);
    detourJumpInComplete = std::make_unique<FunctionDetour<OnJumpInCompleteType>>(onJumpInComplete);
    detourJumpInComplete->Detour(OnJumpInComplete);
}
