#include "PCH.hpp"

#include "Utils.hpp"

DWORD dummy;

namespace Utils
{
    Archetype::Ship* GetShipArch()
    {
        auto shipId = (uint*)0x67337C;
        return Archetype::GetShip(*shipId);
    }

    __declspec(naked) CShip* GetCShip()
    {
        __asm
        {
			mov	eax, 0x54baf0
			call	eax
			test	eax, eax
			jz	noship
			add	eax, 12
			mov	eax, [eax + 4]
			noship:
			ret
        }
    }

    EquipDescList* GetEquipDescList() { return reinterpret_cast<EquipDescList*>(0x672960); }

    IObjInspectImpl* GetInspect(uint shipId)
    {
        static auto getShipInspect = reinterpret_cast<_GetShipInspect>(reinterpret_cast<DWORD>(GetModuleHandleA("server.dll")) + 0x206C0);

        uint iDunno;
        IObjInspectImpl* inspect;
        return !getShipInspect(shipId, inspect, iDunno) ? nullptr : inspect;
    }

    __declspec(naked) void __stdcall LightFuse(IObjInspectImpl* ship, uint fuseId, float delay, float lifetime, float skip)
    {
        __asm
        {
            lea eax, [esp + 8] // fuseId
            push[esp + 20] // skip
            push[esp + 16] // delay
            push 0 // SUBOBJ_ID_NONE
            push eax
            push[esp + 32] // lifetime
            mov ecx, [esp + 24]
            mov eax, [ecx]
            call[eax + 0x1E4]
            ret 20
        }
    }

    __declspec(naked) void __stdcall UnLightFuse(IObjInspectImpl* ship, uint fuseId, float delay)
    {
        __asm
        {
            mov ecx, [esp + 4]
            lea eax, [esp + 8] // fuseId
            push[esp + 12] // delay
            push 0 // SUBOBJ_ID_NONE
            push eax // fuseId
            mov eax, [ecx]
            call[eax + 0x1E8]
            ret 12
        }
    }
} // namespace Utils
