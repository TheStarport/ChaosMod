#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class CockpitMode final : public ActiveEffect
{
        using ChangeCamera = void(__thiscall*)(void* unk, const char* camera, int alwaysZero);
        inline static auto changeCamera = reinterpret_cast<ChangeCamera>(0x549E70);
        inline static auto cameraPtr = reinterpret_cast<PVOID>(0x678B98);
        inline static auto isThirdPerson = reinterpret_cast<PBOOL>(0x00678BA4);

        void FrameUpdate(float delta) override
        {
            if (Utils::GetCShip() && *isThirdPerson)
            {
                changeCamera(cameraPtr, "first_person", 0);
            }
        }

        void End() override
        {
            if (Utils::GetCShip() && !*isThirdPerson)
            {
                changeCamera(cameraPtr, "third_person", 0);
            }
        }

    public:
        explicit CockpitMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CockpitMode, {
    .effectName = "Cockpit Mode",
    .description = "Did anyone willingly play in this mode?",
    .category = EffectType::Visual,
    .timingModifier = 1.0f
});