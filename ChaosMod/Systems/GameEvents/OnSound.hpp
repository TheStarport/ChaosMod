#pragma once

struct FlSound
{
        uint unk;
        uint hash;
};

class OnSound
{
    private:
        using CreateSoundType = FlSound* (*)(uint& hash);
        using PlaySoundType = bool (*)(FlSound* createdSound, int, int);
        using InterceptSoundType = bool(__fastcall*)(FlSound*);
        inline static auto createSound = reinterpret_cast<CreateSoundType>(0x42ae40);
        inline static auto playSound = reinterpret_cast<PlaySoundType>(0x4285f0);
        inline static auto interceptSound = reinterpret_cast<InterceptSoundType>(0x42B840);
        //inline static std::unique_ptr<FunctionDetour<PlaySoundType>> detourPlaySound = nullptr;
        inline static std::unique_ptr<FunctionDetour<InterceptSoundType>> detourInterceptSound = nullptr;

        static bool __fastcall SoundIntercept(FlSound* sound);

    public:
        OnSound() = delete;
        static void Detour();
};
