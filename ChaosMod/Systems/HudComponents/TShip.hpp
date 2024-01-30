#pragma once
#include "Structures.hpp"

class TShip : public TControl
{
        byte data[1074];

    public:
        Vector GetShipViewMatrix()
        {
            uint state = reinterpret_cast<uint>(this) + 0x360;
            const Vector value = *reinterpret_cast<Vector *>(state);
            return value;
        }

        void SetShipViewMatrix(const Vector &V)
        {
            uint state = reinterpret_cast<uint>(this) + 0x360;
            *reinterpret_cast<Vector *>(state) = V;
        }

        virtual ~TShip() = default;
};

using TShipFunction = TShip *(__thiscall *)(const TControl *parent, LPSTR Description, const Vector &automatedRotation, const Vector &startPosition,
                                            const Vector &startingRotation, const Vector &clipping, uint u1, uint u2, uint u3, uint &u4, uint &u5);

using LoadShipFunction = bool(__thiscall *)(TShip *ship, uint &arch, bool b1, bool b2);

#define DefaultShipControls 0, 0, 0
#define DefaultShipColor1   0xFFFFDC0C
#define DefaultShipColor2   0xFFFFFFFF

static TShipFunction CreateShipControl = reinterpret_cast<TShipFunction>(0x5A1020);
static LoadShipFunction LoadShip = reinterpret_cast<LoadShipFunction>(0x585A90);
