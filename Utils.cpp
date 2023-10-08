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

    std::string GetResourceString(ResourceIds id)
    {
        HMODULE hModule = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)GetResourceString, &hModule);

        // One megabyte static buffer to hold any resources we need to load. This is excessive, but we do not know the size until load time.
        static std::array<char, 1'049'000> buffer;

        const auto len = LoadStringA(hModule, static_cast<uint>(id), buffer.data(), buffer.size());
        if (len == -1)
        {
            return {};
        }

        return { buffer.data(), static_cast<uint>(len) };
    }
} // namespace Utils
