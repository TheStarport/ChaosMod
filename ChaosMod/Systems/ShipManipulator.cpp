#include "PCH.hpp"

#include "ShipManipulator.hpp"

void ShipManipulator::SetAngularVelocity(CShip* ship, Vector newVelocity)
{
    const Vector v = Vector(newVelocity.x, newVelocity.y, newVelocity.z);
    const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
    *reinterpret_cast<Vector*>(ptr + 148) = v;
}

Vector ShipManipulator::GetAngularVelocity(CShip* ship)
{
    const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
    return *reinterpret_cast<Vector*>(ptr + 148);
}

Vector ShipManipulator::GetVelocity(CShip* ship)
{
    const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
    return *reinterpret_cast<Vector*>(ptr + 164);
}

void ShipManipulator::SetVelocity(CShip* ship, Vector newVelocity)
{
    const Vector v = Vector(newVelocity.x, newVelocity.y, newVelocity.z);
    const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
    *reinterpret_cast<Vector*>(ptr + 164) = v;
}

void ShipManipulator::SetPosition(CShip* ship, Vector pos)
{
    const Vector v = Vector(pos.x, pos.y, pos.z);
    *reinterpret_cast<Vector*>(PCHAR(ship) + 44) = v;
    ship->update_tree();
}

void ShipManipulator::PhysicsUpdate(const uint system, const float delta)
{
    const auto ship = Utils::GetCShip();
    if (!ship)
    {
        detour->UnDetour();
        PhySys::Update(system, delta);
        detour->Detour(PhysicsUpdate);
        return;
    }

    if (i()->playerAngularVelocityOverride.has_value())
    {
        SetAngularVelocity(ship, i()->playerAngularVelocityOverride.value());
    }

    if (i()->spin)
    {
        Utils::ForEachShip(
            [](auto ship)
            {
                Vector vel = GetAngularVelocity(ship);
                vel.z = 4.0f;
                SetAngularVelocity(ship, vel);
            });
    }

    detour->UnDetour();
    PhySys::Update(system, delta);
    detour->Detour(PhysicsUpdate);
}

ShipManipulator::ShipManipulator()
{
    detour = std::make_unique<FunctionDetour<OnPhysicsUpdate>>(PhySys::Update);
    detour->Detour(PhysicsUpdate);
}

void ShipManipulator::OverridePlayerAngularVelocity(const std::optional<Vector> override) { playerAngularVelocityOverride = override; }
void ShipManipulator::MakeShipsSpin(const bool shouldSpin) { spin = shouldSpin; }