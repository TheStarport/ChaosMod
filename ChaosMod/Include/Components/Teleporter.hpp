#pragma once

class Teleporter : public Component
{
        inline static uint entryObject = 0;

        struct SavedPosition
        {
                uint system{};
                Vector position{};
                Matrix orientation{};
        };

        SavedPosition savedPosition = {};

        struct EntryStack
        {
                uint obj;
                Vector pos;
                byte unk[0x28];
                Matrix orientation;
        };

        class VectorHelpers
        {
            public:
                static void AddX(Vector& v, float dist, const Matrix& ornt)
                {
                    v.x += dist * ornt[0][0];
                    v.y += dist * ornt[1][0];
                    v.z += dist * ornt[2][0];
                }
                // Up/down.
                static void AddY(Vector& v, float dist, const Matrix& ornt)
                {
                    v.x += dist * ornt[0][1];
                    v.y += dist * ornt[1][1];
                    v.z += dist * ornt[2][1];
                }
                // Forwards/backwards.
                static void AddZ(Vector& v, float dist, const Matrix& ornt)
                {
                    v.x -= dist * ornt[0][2];
                    v.y -= dist * ornt[1][2];
                    v.z -= dist * ornt[2][2];
                }
        };

        static void* __stdcall EnterSystemHook(EntryStack* stack);
        static void SwitchOutDoneHook();

        static void ChangeSystem(uint newSystem, const Vector& pos, Matrix orient, bool checkTradeLane = true);

    public:
        static void QueueTeleportEffect(float timer = .3f);
        static void WarpToSolar(CSolar* solar, bool checkForTradelane = true);
        void WaypointWarp();
        void WarpToRandomStar(bool inSystem);
        void WarpToRandomSolar(bool inSystem);
        void WarpToRandomSystem();
        void WarpToPoint(uint system, const Vector& pos, const Vector& rot = {0.f, 0.f, 0.f}) const;
        void BeamToRandomBase(bool inSystem);
        void SaveCurrentPosition();
        void RestorePreviousPosition();
};
