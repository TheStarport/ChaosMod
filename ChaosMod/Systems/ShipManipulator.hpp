#pragma once

class ShipManipulator final : public Singleton<ShipManipulator>
{
        using OnPhysicsUpdate = void (*)(unsigned, float);
        static void PhysicsUpdate(uint system, float delta);
        static void SetAngularVelocity(CShip* ship, Vector newVelocity);
        static Vector GetAngularVelocity(CShip* ship);
        static Vector GetVelocity(CShip* ship);
        static void SetVelocity(CShip* ship, Vector newVelocity);
        static void SetPosition(CShip* ship, Vector pos);

        bool spin = false;

        std::optional<Vector> playerAngularVelocityOverride;
        inline static std::unique_ptr<FunctionDetour<OnPhysicsUpdate>> detour;

    public:
        ShipManipulator();

        void MakeShipsSpin(bool shouldSpin);
        void OverridePlayerAngularVelocity(std::optional<Vector> override);
};
