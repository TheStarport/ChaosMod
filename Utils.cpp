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

    EquipDescList* GetEquipDescList() { return (EquipDescList*)(0x672960); }
} // namespace Utils
