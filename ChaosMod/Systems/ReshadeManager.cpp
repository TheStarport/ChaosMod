#include "PCH.hpp"

#include "ReshadeManager.hpp"

using namespace reshade;
using namespace reshade::api;

// ReSharper disable twice CppInconsistentNaming
extern "C" __declspec(dllexport) const char* NAME = "Chaos Mod Shader Replacement";
extern "C" __declspec(dllexport) const char* DESCRIPTION = "Chaos Mod will replace the active shader with various visual effects!";

void ReshadeManager::OnInitEffect(effect_runtime* runtime)
{
    // Assume last created effect runtime is the main one
    i()->runtime = runtime;
}

void ReshadeManager::OnDestroyEffect(effect_runtime* runtime) { i()->runtime = nullptr; }

void ReshadeManager::ToggleTechnique(const std::string& techniqueName, bool state) const
{
    if (!runtime)
    {
        return;
    }

    runtime->enumerate_techniques("ChaosMod.fx",
                                  [techniqueName, state](reshade::api::effect_runtime* effectRuntime, const reshade::api::effect_technique technique)
                                  {
                                      char techniqueArr[MAX_PATH];
                                      effectRuntime->get_technique_name(technique, techniqueArr);

                                      if (const std::string name = techniqueArr; name == techniqueName)
                                      {
                                          effectRuntime->set_technique_state(technique, state);
                                      }
                                  });
}
void ReshadeManager::SetHModule(const HMODULE module) { thisDll = module; }

void ReshadeManager::InitReshade() const
{
    register_addon(thisDll, GetModuleHandleA("d3d9.dll"));
    register_event<addon_event::init_effect_runtime>(OnInitEffect);
    register_event<addon_event::destroy_effect_runtime>(OnDestroyEffect);
}
