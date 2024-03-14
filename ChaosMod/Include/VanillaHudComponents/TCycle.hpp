#pragma once
#include "Structures.hpp"

class TCycle : public TControl
{
        byte data[888]; // 0x378 in sub_55D2B0

    public:
        virtual ~TCycle() = default;

        byte UpOrLeft()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x34C;
            return *reinterpret_cast<byte*>(state);
        }

        // If flipped horizontally, this will be right
        byte DownOrRight()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x360;
            return *reinterpret_cast<byte*>(state);
        }
};

using TCycleFunction = TCycle*(__thiscall*)(const TControl* parent, LPCSTR description, const Vector& position, float distanceAwayFromEachOther,
                                            float dunnoIncreasingMakesThemBigger, float u1, float u2, float u3, bool rotateHorizontal);

static TCycleFunction CreateCycleControl = reinterpret_cast<TCycleFunction>(0x5A1870);
