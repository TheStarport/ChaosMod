#pragma once

class ShipManipulator final : public Singleton<ShipManipulator>
{
        using OnPhysicsUpdate = void (*)(unsigned, float);
        static void PhysicsUpdate(uint system, float delta);
        static Vector GetAngularVelocity(CShip* ship);
        static Vector GetVelocity(CShip* ship);

        bool spin = false;
        bool personalSpace = false;

        std::optional<Vector> playerAngularVelocityOverride;
        inline static std::unique_ptr<FunctionDetour<OnPhysicsUpdate>> detour;

    public:
        static void SetAngularVelocity(CObject* object, Vector newVelocity);
        static void SetVelocity(CObject* object, Vector newVelocity);
        static void SetPosition(CObject* object, Vector pos);

        ShipManipulator();

        void SetPersonalSpace(bool should);
        void MakeShipsSpin(bool shouldSpin);
        void OverridePlayerAngularVelocity(std::optional<Vector> override);
};
