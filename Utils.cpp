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

    void ForEachShip(const std::function<void(CShip*)>& func)
    {
        if (auto* obj = dynamic_cast<CShip*>(CObject::FindFirst(CObject::CSHIP_OBJECT)))
        {
            func(obj);

            CShip* next;
            do
            {
                next = dynamic_cast<CShip*>(CObject::FindNext());
                if (next)
                {
                    func(next);
                }
            }
            while (next);
        }
    }

    void ForEachSolar(const std::function<void(CSolar*)>& func)
    {
        if (auto* obj = dynamic_cast<CSolar*>(CObject::FindFirst(CObject::CSOLAR_OBJECT)))
        {
            func(obj);

            CSolar* next;
            do
            {
                next = dynamic_cast<CSolar*>(CObject::FindNext());
                if (next)
                {
                    func(next);
                }
            }
            while (next);
        }
    }

    void ForEachAsteroid(const std::function<void(CAsteroid*)>& func)
    {
        if (auto* obj = dynamic_cast<CAsteroid*>(CObject::FindFirst(CObject::CASTEROID_OBJECT)))
        {
            func(obj);

            CAsteroid* next;
            do
            {
                next = dynamic_cast<CAsteroid*>(CObject::FindNext());
                if (next)
                {
                    func(next);
                }
            }
            while (next);
        }
    }
} // namespace Utils
