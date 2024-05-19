#include "PCH.hpp"

#include "Components/ShipManipulator.hpp"

void ShipManipulator::SetAngularVelocity(CObject* object, const Vector& newVelocity)
{
    const auto v = Vector(newVelocity.x, newVelocity.y, newVelocity.z);
    const uint physicsObject = *reinterpret_cast<uint*>(reinterpret_cast<PCHAR>(*reinterpret_cast<uint*>(reinterpret_cast<uint>(object) + 84)) + 152);
    if (!physicsObject)
    {
        return;
    }
    *reinterpret_cast<Vector*>(physicsObject + 148) = v;
}

Vector ShipManipulator::GetAngularVelocity(CObject* ship)
{
    const uint physicsObject = *reinterpret_cast<uint*>(reinterpret_cast<PCHAR>(*reinterpret_cast<uint*>(reinterpret_cast<uint>(ship) + 84)) + 152);
    if (!physicsObject)
    {
        return {};
    }

    return *reinterpret_cast<Vector*>(physicsObject + 148);
}

Vector ShipManipulator::GetVelocity(CObject* ship)
{
    const uint physicsObject = *reinterpret_cast<uint*>(reinterpret_cast<PCHAR>(*reinterpret_cast<uint*>(reinterpret_cast<uint>(ship) + 84)) + 152);
    if (!physicsObject)
    {
        return {};
    }

    return *reinterpret_cast<Vector*>(physicsObject + 164);
}

void ShipManipulator::SetVelocity(CObject* object, const Vector& newVelocity)
{
    const auto v = Vector(newVelocity.x, newVelocity.y, newVelocity.z);
    const uint physicsObject = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(object) + 84)) + 152);
    if (!physicsObject)
    {
        return;
    }

    *reinterpret_cast<Vector*>(physicsObject + 164) = v;
}

void ShipManipulator::SetPosition(CObject* object, const Vector& pos)
{
    const auto v = Vector(pos.x, pos.y, pos.z);
    *reinterpret_cast<Vector*>(reinterpret_cast<PCHAR>(object) + 44) = v;
    object->update_tree();
}

void ShipManipulator::PhysicsUpdate(const uint system, float delta)
{
    detour->UnDetour();

    if (Get<ShipManipulator>()->cocaineMode)
    {
        PhySys::Update(system, delta);
        PhySys::Update(system, delta);
    }

    const auto ship = Utils::GetCShip();
    if (!ship)
    {
        PhySys::Update(system, delta);
        detour->Detour(PhysicsUpdate);
        return;
    }

    if (Get<ShipManipulator>()->playerAngularVelocityOverride.has_value())
    {
        SetAngularVelocity(ship, Get<ShipManipulator>()->playerAngularVelocityOverride.value());
    }

    if (Get<ShipManipulator>()->spin)
    {
        Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                    [](auto ship)
                                    {
                                        Vector vel = GetAngularVelocity(ship);
                                        vel.z = 4.0f;
                                        SetAngularVelocity(ship, vel);
                                    });
    }

    if (Get<ShipManipulator>()->gravity)
    {
        Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                    [delta](auto ship)
                                    {
                                        Vector newVelocity = ShipManipulator::GetVelocity(ship);
                                        newVelocity.y -= 500.f * delta;
                                        ShipManipulator::SetVelocity(ship, newVelocity);
                                    });
    }

    if (Get<ShipManipulator>()->personalSpace)
    {

        Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                    [ship](CShip* otherShip)
                                    {
                                        if (ship == otherShip)
                                        {
                                            return;
                                        }

                                        const auto playerPos = ship->position;
                                        const auto otherPos = otherShip->position;

                                        const auto vectorDiff = playerPos - otherPos;
                                        const auto length = glm::length(vectorDiff);

                                        constexpr float maxFalloffDistance = 400.0f;
                                        constexpr float minFalloffDistance = 200.0f;

                                        if (std::abs(length) > maxFalloffDistance)
                                        {
                                            return;
                                        }

                                        static constexpr float maxForce = 6000;

                                        const float forceFallOffPercentage =
                                            1 - std::clamp((length - minFalloffDistance) / (maxFalloffDistance - minFalloffDistance), 0.0f, 1.0f);

                                        const auto unitVector = vectorDiff / length;
                                        // Apply the force then invert (to push away)
                                        const auto forceVector = -(unitVector * (maxForce * forceFallOffPercentage));

                                        const auto newVelocity = (forceVector / otherShip->shiparch()->mass) + GetVelocity(otherShip);

                                        SetVelocity(ship, newVelocity);
                                    });
    }

    PhySys::Update(system, delta);
    detour->Detour(PhysicsUpdate);
}

ShipManipulator::ShipManipulator()
{
    detour = std::make_unique<FunctionDetour<OnPhysicsUpdate>>(PhySys::Update);
    detour->Detour(PhysicsUpdate);
}

void ShipManipulator::SetPersonalSpace(const bool should) { personalSpace = should; }

void ShipManipulator::OverridePlayerAngularVelocity(const std::optional<Vector>& override) { playerAngularVelocityOverride = override; }
void ShipManipulator::SetCocaineMode(const bool mode) { cocaineMode = mode; }
void ShipManipulator::MakeShipsSpin(const bool shouldSpin) { spin = shouldSpin; }
void ShipManipulator::EnableGravity(const bool enable) { gravity = enable; }
