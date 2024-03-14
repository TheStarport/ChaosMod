// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"
#include "Systems/HudInterface.hpp"

class Lgbtui final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("freelancer.exe", HudPowerColor, 3)
        MemoryListItem("freelancer.exe", HudShieldColor, 3)
        MemoryListItem("freelancer.exe", HudHullColor, 3)
        MemoryListEnd(offsets);
        // clang-format on

        byte r = 255;
        byte g = 0;
        byte b = 0;
        int dr = 0;
        int dg = 0;
        int db = 0;

        void CalculateRgb()
        {
            r += dr;
            g += dg;
            b += db;

            if (r == 255 && g == 0 && b == 0)
            {
                dr = 0;
                dg = 1;
                db = 0;
            }

            if (r == 255 && g == 255 && b == 0)
            {
                dr = -1;
                dg = 0;
                db = 0;
            }

            if (r == 0 && g == 255 && b == 0)
            {
                dr = 0;
                dg = 0;
                db = 1;
            }

            if (r == 0 && g == 255 && b == 255)
            {
                dr = 0;
                dg = -1;
                db = 0;
            }

            if (r == 0 && g == 0 && b == 255)
            {
                dr = 1;
                dg = 0;
                db = 0;
            }

            if (r == 255 && g == 0 && b == 255)
            {
                dr = 0;
                dg = 0;
                db = -1;
            }
        }

        void Update(float delta) override
        {
            for (uint i = 0u; i < 20; i++)
            {
                CalculateRgb();
            }

            std::array bytes = { b, g, r };

            for (auto& [module, address, length, originalData] : GetMemoryAddresses())
            {
                MemUtils::WriteProcMem(module + address, bytes.data(), bytes.size());
            }

            Get<HudInterface>()->SetUiColor(((r & 0xff) << 16) + ((g & 0xFF) << 8) + (b & 0xFF));
        }

        void End() override
        {
            MemoryEffect::End();
            Get<HudInterface>()->SetUiColor(0x0);
        }

    public:
        explicit Lgbtui(const EffectInfo& effectInfo) : MemoryEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Lgbtui, {
    .effectName = "LGBTUI+",
    .description = "Makes your interface covered in rainbows to show your support for our beautiful brothers and sisters",
    .category = EffectType::Interface,
});