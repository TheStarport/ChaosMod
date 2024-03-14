#pragma once

class CameraController final : public Component
{
        using CameraCall = int(__fastcall*)(Camera*, void* edx, float interval);
        float fovOverride = 0.0f;
        std::array<std::pair<float, float>, 3> xyFovPairs;

        static int __fastcall ChaseCamera(Camera* camera, void* edx, float interval);
        static int __fastcall TurretCamera(Camera* camera, void* edx, float interval);
        static int __fastcall CockpitCamera(Camera* camera, void* edx, float interval);

        static std::unique_ptr<FunctionDetour<CameraCall>> chaseCameraDetour;
        static std::unique_ptr<FunctionDetour<CameraCall>> turretCameraDetour;
        static std::unique_ptr<FunctionDetour<CameraCall>> cockpitCameraDetour;

    public:
        void OverrideFov(float newFov);
        CameraController();
};
