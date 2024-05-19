#pragma once

class ShipManipulator final : public Component
{
        using OnPhysicsUpdate = void (*)(unsigned, float);
        static void PhysicsUpdate(uint system, float delta);

        bool spin = false;
        bool personalSpace = false;
        bool cocaineMode = false;
        bool gravity = false;

        std::optional<Vector> playerAngularVelocityOverride;
        inline static std::unique_ptr<FunctionDetour<OnPhysicsUpdate>> detour;

    public:
        static void SetAngularVelocity(CObject* object, const Vector& newVelocity);
        static void SetVelocity(CObject* object, const Vector& newVelocity);
        static void SetPosition(CObject* object, const Vector& pos);
        static Vector GetAngularVelocity(CObject* ship);
        static Vector GetVelocity(CObject* ship);

        ShipManipulator();

        void SetPersonalSpace(bool should);
        void MakeShipsSpin(bool shouldSpin);
        void EnableGravity(bool enable);
        void OverridePlayerAngularVelocity(const std::optional<Vector>& override);
        void SetCocaineMode(bool mode);
};
