#include "PCH.hpp"

#include "CameraController.hpp"

std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::chaseCameraDetour;
std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::turretCameraDetour;
std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::cockpitCameraDetour;

constexpr float defaultXFov = 51.4414444f;
constexpr float defaultYFov = 27.7064610f;
constexpr float fovRatio = 1.856658f;

int __fastcall CameraController::ChaseCamera(Camera* camera, void* edx, float interval)
{
    auto in = i();
    if (in->fovOverride != 0.0f)
    {
        camera->fFovX = in->fovOverride;
        camera->fFovY = in->fovOverride / fovRatio;
        camera->fSinFovX = std::sinf(in->fovOverride);
        camera->fSinFovY = std::sinf(in->fovOverride / fovRatio);
        camera->fCosFovX = std::cosf(in->fovOverride);
        camera->fCosFovY = std::cosf(in->fovOverride / fovRatio);
    }
    else
    {
        camera->fFovX = defaultXFov;
        camera->fFovY = defaultYFov;
        camera->fSinFovX = std::sinf(defaultXFov);
        camera->fSinFovY = std::sinf(defaultYFov);
        camera->fCosFovX = std::cosf(defaultXFov);
        camera->fCosFovY = std::cosf(defaultYFov);
    }

    chaseCameraDetour->UnDetour();
    const auto result = chaseCameraDetour->GetOriginalFunc()(camera, edx, interval);
    chaseCameraDetour->Detour(ChaseCamera);
    return result;
}

int __fastcall CameraController::TurretCamera(Camera* camera, void* edx, float interval)
{
    turretCameraDetour->UnDetour();
    const auto result = turretCameraDetour->GetOriginalFunc()(camera, edx, interval);
    turretCameraDetour->Detour(TurretCamera);
    return result;
}

int __fastcall CameraController::CockpitCamera(Camera* camera, void* edx, float interval)
{
    cockpitCameraDetour->UnDetour();
    const auto result = cockpitCameraDetour->GetOriginalFunc()(camera, edx, interval);
    cockpitCameraDetour->Detour(CockpitCamera);
    return result;
}

void CameraController::OverrideFov(const float newFov) { fovOverride = newFov; }

CameraController::CameraController()
{
    chaseCameraDetour = std::make_unique<FunctionDetour<CameraCall>>(reinterpret_cast<CameraCall>(0x518E40));
    turretCameraDetour = std::make_unique<FunctionDetour<CameraCall>>(reinterpret_cast<CameraCall>(0x51A730));
    cockpitCameraDetour = std::make_unique<FunctionDetour<CameraCall>>(reinterpret_cast<CameraCall>(0x517BA0));

    chaseCameraDetour->Detour(ChaseCamera);
    cockpitCameraDetour->Detour(CockpitCamera);
    turretCameraDetour->Detour(TurretCamera);
}
