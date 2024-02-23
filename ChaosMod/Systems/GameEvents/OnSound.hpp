#pragma once

struct FlSound
{
        uint unk;
        uint hash;
};

class OnSound
{
        inline static std::pair<uint, uint> originalThrusterSounds;
        inline static std::unordered_map<uint, std::pair<uint, uint>> customThrusterSoundMap;
        static CEquip* __stdcall DetourThrusterFind(CEquipManager* equipManager, EquipmentClass thrusterClass);
        static CShip* __fastcall ShipConstructorDetour(CShip* ship, void* edx, int unk);
        static void DetourThrusterFindNaked();
        using ShipConstructorType = CShip*(__fastcall*)(CShip* ship, void* edx, int unk);
        inline static FunctionDetour<ShipConstructorType> shipConstructorDetour{ reinterpret_cast<ShipConstructorType>(
            reinterpret_cast<DWORD>(GetModuleHandleA(nullptr)) + 0x12EFB0) };

        using CreateSoundType = FlSound* (*)(uint& hash);
        using PlaySoundType = bool (*)(FlSound* createdSound, int, int);
        using InterceptSoundType = bool(__fastcall*)(FlSound*);
        inline static auto createSound = reinterpret_cast<CreateSoundType>(0x42ae40);
        inline static auto playSound = reinterpret_cast<PlaySoundType>(0x4285f0);
        inline static auto interceptSound = reinterpret_cast<InterceptSoundType>(0x42B840);
        // inline static std::unique_ptr<FunctionDetour<PlaySoundType>> detourPlaySound = nullptr;
        inline static std::unique_ptr<FunctionDetour<InterceptSoundType>> detourInterceptSound = nullptr;

        static bool __fastcall SoundIntercept(FlSound* sound);

    public:
        OnSound() = delete;
        static void Init();
};
