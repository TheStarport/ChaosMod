// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

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
        DefEffectInfo("Sticky Keys", 1.0f, EffectType::Interface);
};
