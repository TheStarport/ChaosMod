#include "PCH.hpp"

#include "OnHit.hpp"

#include "Effects/AddressTable.hpp"
#include "Systems/ChaosTimer.hpp"

__declspec(naked) void OnHit::NakedOnHit1()
{
    __asm
    {
        push ecx
        push ecx
        call OnDamageHit
        pop ecx
        jmp [oldOnHit1]
    }
}

__declspec(naked) void OnHit::NakedOnHit2()
{
    __asm
    {
        push ecx
        push ecx
        call OnDamageHit
        pop ecx
        jmp [oldOnHit2]
    }
}

__declspec(naked) void OnHit::NakedOnDamage()
{
    __asm
    {
        push [esp+0Ch]
        push [esp+0Ch]
        push [esp+0Ch]
        push ecx
        call OnDamage
        mov eax, [esp]
        add esp, 10h
        jmp eax
    }
}

void __stdcall OnHit::OnDamageHit(const char* ecx)
{
    char* p;
    memcpy(&p, ecx + 0x10, 4);
    uint client;
    memcpy(&client, p + 0xB4, 4);
    uint spaceId;
    memcpy(&spaceId, p + 0xB0, 4);

    dmgToClient = client;
    dmgToSpaceId = spaceId;
}

void OnHit::OnDamage(DamageList* dmgList, DamageEntry dmgEntry)
{
    ChaosTimer::OnApplyDamage(dmgToSpaceId, dmgList, dmgEntry, false);

    dmgList->add_damage_entry(dmgEntry.subObj, dmgEntry.health, dmgEntry.fate);

    ChaosTimer::OnApplyDamage(dmgToSpaceId, dmgList, dmgEntry, true);

    dmgToSpaceId = 0;
    dmgToClient = 0;
}

void OnHit::Detour()
{
    auto naked1 = reinterpret_cast<PDWORD>(NakedOnHit1);
    auto naked2 = reinterpret_cast<PDWORD>(NakedOnHit2);
    auto nakedApply = reinterpret_cast<PDWORD>(NakedOnDamage);

    auto offset = RelOfs("server.dll", AddressTable::DamageHit1);
    MemUtils::ReadProcMem(offset, &oldOnHit1, 4);
    MemUtils::WriteProcMem(offset, &naked1, 4);
    offset = RelOfs("server.dll", AddressTable::DamageHit2);
    MemUtils::WriteProcMem(offset, &naked1, 4);
    offset = RelOfs("server.dll", AddressTable::DamageHit3);
    MemUtils::WriteProcMem(offset, &naked1, 4);

    offset = RelOfs("server.dll", AddressTable::DamageHit4);
    MemUtils::ReadProcMem(offset, &oldOnHit2, 4);
    MemUtils::WriteProcMem(offset, &naked2, 4);
    offset = RelOfs("server.dll", AddressTable::DamageHit5);
    MemUtils::WriteProcMem(offset, &naked2, 4);
    offset = RelOfs("server.dll", AddressTable::DamageHit6);
    MemUtils::WriteProcMem(offset, &naked2, 4);

    offset = RelOfs("server.dll", AddressTable::DamageApply);
    MemUtils::WriteProcMem(offset, &nakedApply, 4);
}
