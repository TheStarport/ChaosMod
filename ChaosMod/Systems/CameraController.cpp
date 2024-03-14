#include "PCH.hpp"

#include "CameraController.hpp"

std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::chaseCameraDetour;
std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::turretCameraDetour;
std::unique_ptr<FunctionDetour<CameraController::CameraCall>> CameraController::cockpitCameraDetour;

// Produces a very nice QUAKE mode
constexpr float FovRatio = 1.856658f;

std::optional<Camera> fovChase;
std::optional<Camera> fovTurret;
std::optional<Camera> fovCockpit;

int __fastcall CameraController::ChaseCamera(Camera* camera, void* edx, float interval)
{
    if (const auto in = Get<CameraController>(); in->fovOverride != 0.0f)
    {
        if (!fovChase.has_value())
        {
            fovChase = *camera;
        }

        camera->fovX = in->fovOverride;
        camera->fovY = in->fovOverride / FovRatio;
        camera->sinFovX = std::sinf(in->fovOverride);
        camera->sinFovY = std::sinf(in->fovOverride / FovRatio);
        camera->cosFovX = std::cosf(in->fovOverride);
        camera->cosFovY = std::cosf(in->fovOverride / FovRatio);
    }
    else if (fovChase.has_value())
    {
        const auto& replacement = fovChase.value();
        camera->fovX = replacement.fovX;
        camera->fovY = replacement.fovY;
        camera->sinFovX = replacement.sinFovX;
        camera->sinFovY = replacement.sinFovY;
        camera->cosFovX = replacement.cosFovX;
        camera->cosFovY = replacement.cosFovY;
    }

    chaseCameraDetour->UnDetour();
    const auto result = chaseCameraDetour->GetOriginalFunc()(camera, edx, interval);
    chaseCameraDetour->Detour(ChaseCamera);
    return result;
}

int __fastcall CameraController::TurretCamera(Camera* camera, void* edx, float interval)
{
    if (const auto in = Get<CameraController>(); in->fovOverride != 0.0f)
    {
        if (!fovTurret.has_value())
        {
            fovTurret = *camera;
        }

        camera->fovX = in->fovOverride;
        camera->fovY = in->fovOverride / FovRatio;
        camera->sinFovX = std::sinf(in->fovOverride);
        camera->sinFovY = std::sinf(in->fovOverride / FovRatio);
        camera->cosFovX = std::cosf(in->fovOverride);
        camera->cosFovY = std::cosf(in->fovOverride / FovRatio);
    }
    else if (fovTurret.has_value())
    {
        const auto& replacement = fovTurret.value();
        camera->fovX = replacement.fovX;
        camera->fovY = replacement.fovY;
        camera->sinFovX = replacement.sinFovX;
        camera->sinFovY = replacement.sinFovY;
        camera->cosFovX = replacement.cosFovX;
        camera->cosFovY = replacement.cosFovY;
    }

    turretCameraDetour->UnDetour();
    const auto result = turretCameraDetour->GetOriginalFunc()(camera, edx, interval);
    turretCameraDetour->Detour(TurretCamera);
    return result;
}

int __fastcall CameraController::CockpitCamera(Camera* camera, void* edx, float interval)
{
    if (const auto in = Get<CameraController>(); in->fovOverride != 0.0f)
    {
        if (!fovCockpit.has_value())
        {
            fovCockpit = *camera;
        }

        camera->fovX = in->fovOverride;
        camera->fovY = in->fovOverride / FovRatio;
        camera->sinFovX = std::sinf(in->fovOverride);
        camera->sinFovY = std::sinf(in->fovOverride / FovRatio);
        camera->cosFovX = std::cosf(in->fovOverride);
        camera->cosFovY = std::cosf(in->fovOverride / FovRatio);
    }
    else if (fovCockpit.has_value())
    {
        const auto& replacement = fovCockpit.value();
        camera->fovX = replacement.fovX;
        camera->fovY = replacement.fovY;
        camera->sinFovX = replacement.sinFovX;
        camera->sinFovY = replacement.sinFovY;
        camera->cosFovX = replacement.cosFovX;
        camera->cosFovY = replacement.cosFovY;
    }

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
