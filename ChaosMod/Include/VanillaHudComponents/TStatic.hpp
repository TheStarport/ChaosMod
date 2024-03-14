#pragma once
#include "Structures.hpp"

class TStatic : public TControl
{
        byte data[1056]; // 0x420 in sub_589A20
    public:
        virtual ~TStatic() = default;
};

using TStaticFunction = TStatic*(__thiscall*)(const TControl* parent, LPCSTR description, LPCSTR mesh, const Vector& position, LPCSTR hardpoint,
                                              const Vector& mouseSize, uint u1, uint u2, uint u3, uint u4, uint u5, uint u6, uint u7, uint u8, uint u9,
                                              uint u10, const Vector& unknownVector);

static TStaticFunction CreateStaticControl = reinterpret_cast<TStaticFunction>(0x5A0DB0);
inline TStatic* CreateDefaultStaticControl(TControl* parent, LPCSTR name, LPCSTR mesh, const Vector& pos, LPCSTR hardpoint, const Vector& mouseSize)
{
    return CreateStaticControl(parent, name, mesh, pos, hardpoint, mouseSize, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, CreateVector(0, 0, 0));
}
