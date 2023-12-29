// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming
#include "PCH.hpp"

#include "OnSound.hpp"

#include "Systems/ChaosTimer.hpp"

bool __fastcall OnSound::SoundIntercept(FlSound* sound)
{
    if (const uint newHash = ChaosTimer::OnSoundEffect(sound->hash); newHash != sound->hash)
    {
        if (!newHash)
        {
            return false;
        }

        pub::Audio::PlaySoundEffect(1, newHash);
        return false;
    }

    detourInterceptSound->UnDetour();
    const auto val = interceptSound(sound);
    detourInterceptSound->Detour(SoundIntercept);
    return val;
}

void OnSound::Detour()
{
    detourInterceptSound = std::make_unique<FunctionDetour<InterceptSoundType>>(interceptSound);
    detourInterceptSound->Detour(SoundIntercept);
}
