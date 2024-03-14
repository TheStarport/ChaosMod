#pragma once
#include "Structures.hpp"

class TToggle : public TControl
{
        byte data[972]; // 0x3CC in sub_55D3C0
    public:
        bool GetToggleState()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x3A0;
            const bool value = *reinterpret_cast<bool*>(state);
            return value;
        }

        void SetToggleState(const bool value)
        {
            const uint state = reinterpret_cast<uint>(this) + 0x3A0;
            *reinterpret_cast<bool*>(state) = value;
        }

        virtual ~TToggle() = default;
};

using TToggleFunction = TToggle*(__thiscall*)(const TControl* parent, LPCSTR description, LPCSTR enabled, LPCSTR disabled, LPCSTR sound, uint u1,
                                              const Vector& position, LPCSTR hardpoint, const Vector& mouseSize, uint u2, uint u3, uint u4, uint u5,
                                              uint enabledResStrId, uint disabledResStrId);

static TToggleFunction CreateToggleControl = reinterpret_cast<TToggleFunction>(0x5A0FB0);
