// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"
#include "Systems/HudInterface.hpp"

class BuggyInterface final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("freelancer.exe", EnergyBarHeight, sizeof(double))
        MemoryListItem("freelancer.exe", TotalBarWidth, sizeof(double))
        MemoryListEnd(offsets);
        // clang-format on

        double width = 1.0;
        double height = 0.1;
        float time = 1.0f;
        bool goingDown = true;
        bool ended = false;

        using LockManeuverT = bool(__fastcall*)(IBehaviorManager* bm, void* edx, bool lock);
        inline static FunctionDetour<LockManeuverT> lockManeuverDetour = FunctionDetour(reinterpret_cast<LockManeuverT>(GetProcAddress(
            GetModuleHandleA("common.dll"), "?lock_maneuvers@IBehaviorManager@@QAE_N_N@Z"))); // NOLINT(clang-diagnostic-cast-function-type-strict)

        static bool __fastcall LockManeuverDetour(IBehaviorManager* bm, void* edx, bool lock)
        {
            if (lock)
            {
                Get<HudInterface>()->SetBuggyInterface(false);
            }

            lockManeuverDetour.UnDetour();
            bool res = lockManeuverDetour.GetOriginalFunc()(bm, edx, lock);
            lockManeuverDetour.Detour(LockManeuverDetour);
            return res;
        }

    public:
        void Begin() override
        {
            width = 1.0;
            height = 0.1;
            goingDown = true;
            ended = false;

            MemoryEffect::Begin();
            auto info = offsets[0];
            MemUtils::WriteProcMem(info.module + info.offset, &height, info.length);

            info = offsets[1];
            MemUtils::WriteProcMem(info.module + info.offset, &width, info.length);

            Get<HudInterface>()->SetBuggyInterface(true);
        }

        void Update(const float delta) override
        {
            if (goingDown)
            {
                width = std::lerp(width, 0.052, delta * 2);
                height = std::lerp(height, 0.0052, delta * 2);
                if (height <= 0.0053)
                {
                    goingDown = false;
                }
            }
            else
            {
                width = std::lerp(width, 1, delta * 2);
                height = std::lerp(height, 0.1, delta * 2);

                if (height >= 0.099)
                {
                    goingDown = true;
                }
            }

            auto info = offsets[0];
            MemUtils::WriteProcMem(info.module + info.offset, &height, info.length);

            info = offsets[1];
            MemUtils::WriteProcMem(info.module + info.offset, &width, info.length);
        }

        void End() override
        {
            if (!ended)
            {
                ended = true;
                MemoryEffect::End();
                Get<HudInterface>()->SetBuggyInterface(false);
            }
        }

        explicit BuggyInterface(const EffectInfo& effectInfo) : MemoryEffect(effectInfo) { lockManeuverDetour.Detour(LockManeuverDetour); }
};

// clang-format off
SetupEffect(BuggyInterface, {
    .effectName = "Buggy Interface",
    .description = "Causes all the buttons and UI elements to start flying all over the screen.",
    .category = EffectType::Interface,
});