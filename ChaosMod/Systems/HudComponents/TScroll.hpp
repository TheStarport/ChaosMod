#pragma once
#include "Structures.hpp"

class TScroll : public TControl
{
        byte data[1072]; // 0x430 in sub_597AB0

    public:
        float GetCurrentPosition()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x3C8;
            return *reinterpret_cast<float*>(state);
        }

        void SetCurrentPosition(const uint value) { Perform(0x38, value, 0); }

        void SetCurrentCapacity(const uint cap) { Perform(0x37, 1, cap); }
};

using TScrollFunction = TScroll*(__thiscall*)(const TControl* parent, LPCSTR desc, const Vector& pos, float ux, float uy, float uz, uint capacity, uint count,
                                              bool horizontal);

static TScrollFunction CreateScrollControl = reinterpret_cast<TScrollFunction>(0x5A17E0);
