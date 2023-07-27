#pragma once
#include "Structures.hpp"

class TButton : public TControl
{
        byte data[888]; // 0x378 in sub_55D2B0

    public:
        virtual ~TButton() = default;
};

using TButtonFunction = TButton*(__thiscall*)(const TControl* parent, LPCSTR description, LPCSTR mesh, LPCSTR mesh2, LPCSTR clickSound, LPCSTR event,
                                              const Vector& position, LPCSTR hardpoint, const Vector& mouseSize, bool noMeshRender, bool actionButton,
                                              bool noColourChange, bool noZEnable, uint tip, uint tip2, uint keyId);

static TButtonFunction CreateButtonControl = reinterpret_cast<TButtonFunction>(0x5A0C90);
