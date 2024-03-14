#pragma once

class ShipManipulator final : public Component
{
        using OnPhysicsUpdate = void (*)(unsigned, float);
        static void PhysicsUpdate(uint system, float delta);
        static Vector GetAngularVelocity(CShip* ship);
        static Vector GetVelocity(CShip* ship);

        bool spin = false;
        bool personalSpace = false;
        bool cocaineMode = false;

        std::optional<Vector> playerAngularVelocityOverride;
        inline static std::unique_ptr<FunctionDetour<OnPhysicsUpdate>> detour;

    public:
        static void SetAngularVelocity(CObject* object, const Vector& newVelocity);
        static void SetVelocity(CObject* object, const Vector& newVelocity);
        static void SetPosition(CObject* object, const Vector& pos);

        ShipManipulator();

        void SetPersonalSpace(bool should);
        void MakeShipsSpin(bool shouldSpin);
        void OverridePlayerAngularVelocity(const std::optional<Vector>& override);
        void SetCocaineMode(const bool mode);
};
