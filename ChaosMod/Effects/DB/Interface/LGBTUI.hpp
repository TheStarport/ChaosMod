// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/MemoryEffect.hpp"

class Lgbtui final : public MemoryEffect
{
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

    public:
        void Update(float delta) override
        {
            const auto [module, address, length] = GetMemoryAddress();

            // TODO: Rewrite this to not be awful
            for (uint i = 0u; i < 20; i++)
            {
                CalculateRgb();
            }

            std::array bytes = { b, g, r };
            Utils::Memory::WriteProcMem(module + address, bytes.data(), bytes.size());
        }

        DefEffectInfo("LGBTUI+", 1.0f, EffectType::Interface);

    protected:
        DefMemoryAddress("freelancer.exe", 0x0D57AC, 6);
};
