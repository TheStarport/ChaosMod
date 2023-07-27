#pragma once
#include "Structures.hpp"

class TWire : public TControl
{
        byte data[912]; // 0x390 in sub_5A4E10
    public:
        virtual ~TWire() = default;
};

using TWireControl = TWire*(__thiscall*)(const TControl* parent, LPCSTR description, LPCSTR sound, const Vector& position, const Vector& dimensions,
                                         LPCSTR hardpoint, uint u2, uint resHintId, uint u4, uint u5);

static TWireControl CreateWireControl = reinterpret_cast<TWireControl>(0x5A0D00);
