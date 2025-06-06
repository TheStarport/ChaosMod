#include "PCH.hpp"

#include "Memory/OnHit.hpp"

#include "CoreComponents/ChaosTimer.hpp"
#include "Effects/AddressTable.hpp"

FARPROC ExplosionHitOrigFunc, SolarExplosionHitOrigFunc, ShipMunitionHitOrigFunc, ShipColGrpDmgFunc, ShipFuseLightFunc, ShipEquipDestroyedFunc;

bool __stdcall OnHit::ExplosionHit(EqObj* obj, ExplosionDamageEvent* explosion, DamageList* dmg) { return ChaosTimer::OnExplosion(obj, explosion, dmg); }

__declspec(naked) void OnHit::ExplosionHitNaked()
{
    __asm {
        push ecx
        push[esp + 0xC]
        push[esp + 0xC]
        push ecx
        call ExplosionHit
        pop ecx
        test al, al
        jz callOriginal
        ret 0x8

        callOriginal:
        jmp [ExplosionHitOrigFunc]
    }
}

void __stdcall OnHit::SolarExplosionHit(EqObj* obj, ExplosionDamageEvent* explosion, DamageList* dmg) {}

__declspec(naked) void OnHit::SolarExplosionHitNaked()
{
    __asm {
		push ecx
		push[esp + 0xC]
		push[esp + 0xC]
		push ecx
		call SolarExplosionHit
		pop ecx
		ret 0x8
    }
}

void __fastcall OnHit::ShipMunitionHit(EqObj* ship, void* edx, MunitionImpactData* data, DamageList* dmg)
{
    ChaosTimer::OnMunitionHit(ship, data, dmg, false);

    ShipMunitionHitCall(ship, data, dmg);

    ChaosTimer::OnMunitionHit(ship, data, dmg, true);
}

void __stdcall OnHit::ShipColGrpDmg(EqObj* obj, CArchGroup* colGrp, float& incDmg, DamageList* dmg) {}

__declspec(naked) void OnHit::ShipColGrpDmgNaked()
{
    __asm {
		push ecx
		push [esp+0x10]
		lea eax, [esp + 0x10]
		push eax
		push[esp + 0x10]
		push ecx
		call ShipColGrpDmg
		pop ecx
		jmp [ShipColGrpDmgFunc]
    }
}

void __stdcall OnHit::ShipFuseLight(EqObj* ship, uint fuseCause, uint* fuseId, ushort sId, float radius, float fuseLifetime) {}

__declspec(naked) void OnHit::ShipFuseLightNaked()
{
    __asm {
		push ecx
		push [esp+0x18]
		push [esp+0x18]
		push [esp+0x18]
		push [esp+0x18]
		push [esp+0x18]
		push ecx
		call ShipFuseLight
		pop ecx
		jmp [ShipFuseLightFunc]
    }
}

using ShipEquipDamageType = void(__thiscall*)(EqObj*, CEquip*, float incDmg, DamageList* dmg);
auto ShipEquipDamageFunc = reinterpret_cast<ShipEquipDamageType>(0x6CEA4A0);
void __fastcall OnHit::ShipEquipDamage(EqObj* obj, void* edx, CAttachedEquip* equip, float incDmg, DamageList* dmg)
{
    ShipEquipDamageFunc(obj, equip, incDmg, dmg);
}

void __stdcall OnHit::ShipEquipmentDestroyed(EqObj* ship, const CEquip* eq, DamageEntry::SubObjFate fate, DamageList* dmgList) {}

__declspec(naked) void OnHit::ShipEquipmentDestroyedNaked()
{
    __asm {
        push ecx
        push[esp + 0x10]
        push[esp + 0x10]
        push[esp + 0x10]
        push ecx
        call ShipEquipmentDestroyed
        pop ecx
        jmp[ShipEquipDestroyedFunc]
    }
}

void OnHit::Detour()
{
    static bool detoured = false;
    if (detoured)
    {
        return;
    }

    detoured = true;
    const auto shipEquipDamage = reinterpret_cast<PDWORD>(ShipEquipDamage);
    const auto explosionHitNaked = reinterpret_cast<PDWORD>(ExplosionHitNaked);
    const auto solarExplosionHitNaked = reinterpret_cast<PDWORD>(SolarExplosionHitNaked);
    const auto shipMunitionHit = reinterpret_cast<PDWORD>(ShipMunitionHit);
    const auto shipColGrpDmgNaked = reinterpret_cast<PDWORD>(ShipColGrpDmgNaked);
    const auto shipFuseLightNaked = reinterpret_cast<PDWORD>(ShipFuseLightNaked);
    const auto shipEquipmentDestroyedNaked = reinterpret_cast<PDWORD>(ShipEquipmentDestroyedNaked);

    auto offset = RelOfs("server.dll", AddressTable::ShipEquipDamage);
    MemUtils::WriteProcMem(offset, &shipEquipDamage, 4);

    offset = RelOfs("server.dll", AddressTable::ExplosionHit);
    MemUtils::ReadProcMem(offset, &ExplosionHitOrigFunc, 4);
    MemUtils::WriteProcMem(offset, &explosionHitNaked, 4);

    offset = RelOfs("server.dll", AddressTable::SolarExplosionHit);
    MemUtils::ReadProcMem(offset, &SolarExplosionHitOrigFunc, 4);
    MemUtils::WriteProcMem(offset, &solarExplosionHitNaked, 4);

    offset = RelOfs("server.dll", AddressTable::SolarMunitionHit);
    MemUtils::ReadProcMem(offset, &ShipMunitionHitOrigFunc, 4);
    MemUtils::WriteProcMem(offset, &shipMunitionHit, 4);

    offset = RelOfs("server.dll", AddressTable::ShipCollisionGroupDamaged);
    MemUtils::ReadProcMem(offset, &ShipColGrpDmgFunc, 4);
    MemUtils::WriteProcMem(offset, &shipColGrpDmgNaked, 4);

    offset = RelOfs("server.dll", AddressTable::ShipFuseLight);
    MemUtils::ReadProcMem(offset, &ShipFuseLightFunc, 4);
    MemUtils::WriteProcMem(offset, &shipFuseLightNaked, 4);

    offset = RelOfs("server.dll", AddressTable::ShipEquipmentDestroyed);
    MemUtils::ReadProcMem(offset, &ShipEquipDestroyedFunc, 4);
    MemUtils::WriteProcMem(offset, &shipEquipmentDestroyedNaked, 4);
}
