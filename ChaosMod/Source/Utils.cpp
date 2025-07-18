#include "PCH.hpp"

#include "Utils.hpp"

DWORD dummy;

namespace Utils
{
    struct SphereObject
    {
            void* vtable;
            uint dunno[3];
    };

    struct StarSystemSphereManagerThing
    {
            SphereObject nebulaObjectHandle;
            CacheString nebulaFilepath;
            SphereObject basicStarsObjectHandle;
            CacheString basicStarsFilepath;
            SphereObject complexStarsObjectHandle;
            CacheString complexStarsFilepath;

            uint spheresLoaded;
            uint targetSphereCount;
    };

    const char* GetCurrentStarSphere()
    {
        static auto mgr = reinterpret_cast<StarSystemSphereManagerThing*>(0x679900);
        return mgr->nebulaFilepath.value;
    }

    void SetNewStarSphere(const char* filepath)
    {
        using unloadStarSphere = int(__fastcall*)(DWORD);
        const auto unloadStarSphereFunc = reinterpret_cast<unloadStarSphere>(0x420F70);
        static auto mgr = reinterpret_cast<StarSystemSphereManagerThing*>(0x679900);

        mgr->spheresLoaded = 0;
        mgr->nebulaFilepath.clear();

        mgr->nebulaFilepath.value = StringAlloc(filepath, false);
        unloadStarSphereFunc(reinterpret_cast<DWORD>(mgr));
    }

    void CatchupNpc(ResourcePtr<SpawnedObject> obj, bool& catchingUp, const float catchUpDistance)
    {
        const auto player = Utils::GetCShip();
        const auto npc = obj.Acquire();
        if (!npc || !player)
        {
            return;
        }

        const auto distance = glm::length(player->position - npc->obj->position);
        if (catchingUp && std::abs(distance) < catchUpDistance * 0.33333f)
        {
            catchingUp = false;

            pub::AI::DirectiveCancelOp op;
            op.fireWeapons = true;
            pub::AI::SubmitDirective(npc->spaceObj, &op);
        }
        else if (!catchingUp && std::abs(distance) > catchUpDistance)
        {
            catchingUp = true;

            pub::AI::DirectiveFollowOp op;
            op.maxDistance = 400.f;
            op.followSpaceObj = player->id;
            op.dunno2 = 400.f;
            pub::AI::SubmitDirective(npc->spaceObj, &op);
        }
    }

    Archetype::Ship* GetShipArch()
    {
        const auto shipId = (uint*)0x67337C;
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
        static auto getShipInspect = reinterpret_cast<GetShipInspectT>(reinterpret_cast<DWORD>(GetModuleHandleA("server.dll")) + 0x206C0);

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
