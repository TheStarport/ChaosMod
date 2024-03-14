#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class SelfDestruct final : public ActiveEffect
{
        void Begin() override { pub::Audio::PlaySoundEffect(1, CreateID("chaos_self_destruct")); }

        void End() override
        {
            if (const CShip* ship = Utils::GetCShip(); ship && Get<Random>()->Uniform(0u, 1u))
            {
                pub::SpaceObj::Destroy(ship->id, DestroyType::Fuse);
            }
            else
            {
                pub::Audio::PlaySoundEffect(1, CreateID("chaos_windows_error"));
                MessageBoxA(nullptr, "SelfDestruct.exe has stopped working.", "SelfDestruct.exe", MB_OK);
            }
        }

    public:
        explicit SelfDestruct(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SelfDestruct, {
    .effectName = "Self Destruct",
    .description = "You probably shouldn't have pressed that button. Hopefully it's a dud...",
    .category = EffectType::Misc,
    .fixedTimeOverride = 12.0f,
});