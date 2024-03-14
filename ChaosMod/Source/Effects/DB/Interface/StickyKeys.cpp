// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class StickyKeys final : public ActiveEffect
{
        float time = 7.0f;
        void Update(const float delta) override
        {
            time -= delta;
            if (time < 0.0f)
            {
                time = 7.0f;
                INPUT inputs[1] = { { INPUT_KEYBOARD } };

                inputs[0].ki.wVk = VK_F1;

                SendInput(1, inputs, sizeof(inputs));
            }
        }

    public:
        explicit StickyKeys(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(StickyKeys, {
    .effectName = "Sticky Keys",
    .description = "Periodically pauses your game while you're in the middle of stuff. Sounds like fun, doesn't it!",
    .category = EffectType::Interface,
});