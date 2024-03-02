#include "PCH.hpp"

#include "ShipInfocardOverride.hpp"

#define RadToDeg(x, xx) ((x / xx) * (180.f / 3.14159265f))

int ShipInfocardOverride::GetShipIds(Archetype::Ship* ship)
{
    curShip = ship;
    return shipOverrideIds; // Some arbitrary value I copied. Defaults to the patriot information.
}

__declspec(naked) void ShipInfocardOverride::PatchShipInfoSellerIdsNaked()
{
    __asm {
			mov ecx, [edi + 0x0F0]
			mov edx, [edi + 0x0F4]
			mov ebp, [edi + 0x0EC]
			push esi
			push eax
			push edi
			push ecx
			push edx
			push ebp
			mov  ecx, edi
			mov  isInventory, 0
			call GetShipIds
			pop ebp
			pop edx
			pop ecx
			pop edi
			mov edx, eax
			pop eax
			pop esi
			jmp[idsShipJump2]
    }
}

__declspec(naked) void ShipInfocardOverride::PatchShipInfoInventoryIdsNaked()
{
    __asm {
			mov eax, [ebp + 0x0EC]
			mov edx, [ebp + 0x014]
			mov ecx, [ebp + 0x018]
			push esi
			push eax
			push edi
			push ecx
			push edx
			push ebp
			mov  ecx, ebp
			mov  isInventory, 1
			call GetShipIds
			pop ebp
			pop edx
			pop ecx
			pop edi
			mov ecx, eax
			pop eax
			pop esi
			jmp[idsShipJump]
    }
}

std::optional<std::wstring> ShipInfocardOverride::OverrideIds(const uint ids)
{
    if (curShip == nullptr)
    {
        return std::nullopt;
    }

    if (ids == shipOverrideIds)
    {
        // 1 = Gun/Turret/Special Mounts
        // 2 = Armour
        // 3 = Cargo Space
        // 4 = Max Bats/Bots
        // 5 = Turn Rate
        // 6 = Base Drag
        // 7 = Max Resp. Rate
        // 8 = Additional Equipment

        int guns = 0, turrets = 0, torps = 0;
        bool hasMine = false, hasCm = false;
        std::vector<char*> hpsGun;
        std::vector<char*> hpsTurret;
        std::vector<char*> hpsTorps;

        for (const auto& val : curShip->hardpoints)
        {
            switch (val.type) // NOLINT
            {
                case hp_gun_special_1:
                case hp_gun_special_2:
                case hp_gun_special_3:
                case hp_gun_special_4:
                case hp_gun_special_5:
                case hp_gun_special_6:
                case hp_gun_special_7:
                case hp_gun_special_8:
                case hp_gun_special_9:
                case hp_gun_special_10:
                    for (auto hp : *curShip->get_legal_hps(val.type))
                    {
                        if (std::ranges::find(hpsGun, hp.value) == hpsGun.end() && std::ranges::find(hpsTurret, hp.value) == hpsTurret.end())
                        {
                            hpsGun.emplace_back(hp.value);
                            guns++;
                        }
                    }
                    break;
                default:;
            }
        }

        for (const auto& val : curShip->hardpoints)
        {
            switch (val.type) // NOLINT
            {
                case hp_turret_special_1:
                case hp_turret_special_2:
                case hp_turret_special_3:
                case hp_turret_special_4:
                case hp_turret_special_5:
                case hp_turret_special_6:
                case hp_turret_special_7:
                case hp_turret_special_8:
                case hp_turret_special_9:
                    for (auto hp : *curShip->get_legal_hps(val.type))
                    {
                        if (std::ranges::find(hpsTurret, hp.value) == hpsTurret.end() && std::ranges::find(hpsGun, hp.value) == hpsGun.end())
                        {
                            hpsTurret.emplace_back(hp.value);
                            turrets++;
                        }
                    }
                    break;

                case hp_mine_dropper: hasMine = true; break;
                case hp_countermeasure_dropper: hasCm = true; break;

                case hp_torpedo_special_1:
                case hp_torpedo_special_2:
                    for (auto& hp : *curShip->get_legal_hps(val.type))
                    {
                        if (std::ranges::find(hpsTorps, hp.value) == hpsTorps.end())
                        {
                            hpsTorps.emplace_back(hp.value);
                            torps++;
                        }
                    }
                    break;
                default:;
            }
        }

        // clang-format off
        std::wostringstream ss;
        ss << std::fixed; // Don't use e notation
        if (!isInventory)
        {
            ss << LR"(<?xml version="1.0" encoding="UTF-16"?><RDL><PUSH/><PARA/><JUST loc="center"/><TEXT>Stats</TEXT><PARA/><JUST loc="left"/><TEXT>)" << guns << L"/" << turrets
               << L"/" << torps << L"</TEXT><PARA/>" << 
                L"<TEXT>" << static_cast<int>(curShip->hitPoints) << L"</TEXT><PARA/>" << 
                L"<TEXT>" << static_cast<int>(curShip->holdSize) << L"</TEXT><PARA/>" << 
                L"<TEXT>" << curShip->maxNanobots << L"/" << curShip->maxShieldBats << L"</TEXT><PARA/>" << 
                L"<TEXT>" << std::setprecision(2) << roundf(RadToDeg(curShip->steeringTorque.x, curShip->angularDrag.x) * 100.f) / 100.f << L" (deg/s)</TEXT><PARA/>" << 
                L"<TEXT>" << std::setprecision(3) << roundf(curShip->rotationInertia.x / (curShip->angularDrag.x * 0.4342944f) * 1000.f) / 1000.f << L" (s)</TEXT><PARA/>" << 
                L"<TEXT>" << (hasMine ? L"Mine" : L"") << (hasCm ? (hasMine ? L", CM" : L"CM ") : L"") << L"</TEXT><PARA/>" << 
                L"<PARA/><POP/></RDL>";
        }
        else
        {
            ss << LR"(<?xml version="1.0" encoding="UTF-16"?><RDL><PUSH/><PARA/><JUST loc="center"/><TEXT>Stats</TEXT><PARA/><JUST loc="left"/><PARA/>)"
               << L"<TEXT>Gun/Turret/Special: " << guns << L"/" << turrets << L"/" << torps << L"</TEXT><PARA/>"
               << L"<TEXT>Armor: " << static_cast<int>(curShip->hitPoints) << L"</TEXT><PARA/>"
               << L"<TEXT>Cargo Space: " << static_cast<int>(curShip->holdSize) << L"</TEXT><PARA/>"
               << L"<TEXT>Max Batteries/Nanobots: " << curShip->maxNanobots << L"/" << curShip->maxShieldBats << L"</TEXT><PARA/>"
               << L"<TEXT>Turn Rate: " << std::setprecision(2) << roundf(RadToDeg(curShip->steeringTorque.x, curShip->angularDrag.x) * 100.f) / 100.f
               << L" (deg/s)</TEXT><PARA/>"
               << L"<TEXT>Response Rate: " << std::setprecision(3)
               << roundf(curShip->rotationInertia.x / (curShip->angularDrag.x * 0.43429448f) * 1000.f) / 1000.f << L" (s)</TEXT><PARA/>"
               << L"<TEXT>Additional Equipment: " << (hasMine ? L"Mine" : L"") << (hasCm ? (hasMine ? L", CM" : L"CM ") : L"")
               << L"</TEXT><PARA/>"
               << L"<PARA/><POP/></RDL>";
        }

        return { ss.str() };
    }

    if (ids == shipWeaponInfoIds)
    {
            std::wostringstream ss;
        ss << LR"(<?xml version="1.0" encoding="UTF-16"?><RDL><PUSH/><PARA/><JUST loc="center"/><TEXT>Stats</TEXT><PARA/><JUST loc="left"/>)"
           << L"<TEXT>Gun/Turret/Special Mounts:</TEXT><PARA/>"
           << L"<TEXT>Armor:</TEXT><PARA/>"
           << L"<TEXT>Cargo Space:</TEXT><PARA/>"
           << L"<TEXT>Max Nanobots/Batteries:</TEXT><PARA/>"
           << L"<TEXT>Turn Rate:</TEXT><PARA/>"
           << L"<TEXT>Time To 100% TR (Response):</TEXT><PARA/>"
           << L"<TEXT>Additional Equipment: </TEXT><PARA/><POP/></RDL>";
        return { ss.str() };
    }

    return std::nullopt;
}

void ShipInfocardOverride::Init()
{
#define AddrIdsShipSeller    ((PBYTE)0x4B8C00)
#define AddrIdsShipInventory ((PBYTE)0x47BD38)

    ProtectExecuteReadWrite(AddrIdsShipSeller, 5);
    AddrIdsShipSeller[0] = 0xe9;
    *(DWORD*)(AddrIdsShipSeller + 1) = (PBYTE)PatchShipInfoSellerIdsNaked - AddrIdsShipSeller - 5;

    ProtectExecuteReadWrite(AddrIdsShipInventory, 5);
    AddrIdsShipInventory[0] = 0xe9;
    *(DWORD*)(AddrIdsShipInventory + 1) = (PBYTE)PatchShipInfoInventoryIdsNaked - AddrIdsShipInventory - 5;
}
