#include "PCH.hpp"

#include "Components/EventManager.hpp"

std::unique_ptr<FunctionDetour<EventManager::OnLaunch>> EventManager::launchDetour;

void EventManager::PlayerLaunchDetour(IServerImpl* impl, void* edx, uint client, uint ship)
{
    launchDetour->UnDetour();
    launchDetour->GetOriginalFunc()(impl, edx, client, ship);
    launchDetour->Detour(PlayerLaunchDetour);
}

void EventManager::SetupDetours() const
{
    const HMODULE server = GetModuleHandleA("server.dll");

    launchDetour = std::make_unique<FunctionDetour<OnLaunch>>(OnLaunch(GetProcAddress(server, "?PlayerLaunch@IServerImpl@@UAEXII@Z")));
    launchDetour->Detour(PlayerLaunchDetour);
}
