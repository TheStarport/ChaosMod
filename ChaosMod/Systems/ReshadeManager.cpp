#include "PCH.hpp"

#include "ReshadeManager.hpp"

#include "SystemComponents/GlobalTimers.hpp"

using namespace reshade;
using namespace reshade::api;

// ReSharper disable twice CppInconsistentNaming
extern "C" __declspec(dllexport) const char* NAME = "Chaos Mod Shader Replacement";
extern "C" __declspec(dllexport) const char* DESCRIPTION = "Chaos Mod will replace the active shader with various visual effects!";

void ReshadeManager::OnInitEffect(effect_runtime* runtime)
{
    if (!i()->runtime)
    {
        // Set a timer to disable any active effects one second after start, so they can all init
        GlobalTimers::i()->AddTimer(
            [](auto delta)
            {
                i()->ToggleTechnique("", false);
                return true;
            },
            1.0f);
    }

    // Assume last created effect runtime is the main one
    i()->runtime = runtime;
}

void ReshadeManager::OnDestroyEffect(effect_runtime* runtime) { i()->runtime = nullptr; }

std::optional<std::pair<Vector, effect_uniform_variable>> ReshadeManager::GetUniformFloat(const std::string& name, const size_t valueCount) const
{
    const auto var = runtime->find_uniform_variable("ChaosMod.fx", name.c_str());
    if (!var.handle)
    {
        return std::nullopt;
    }

    Vector vec;
    runtime->get_uniform_value_float(var, reinterpret_cast<float*>(&vec), valueCount);

    return { std::make_pair(vec, var) };
}

void ReshadeManager::SetUniformFloat(const std::string& name, const Vector& value, const size_t count) const
{
    if (!runtime)
    {
        return;
    }

    const auto vector = GetUniformFloat(name, count);
    if (!vector.has_value())
    {
        throw std::runtime_error("Shader variable was not found and could not be set.");
    }

    runtime->set_uniform_value_float(vector->second, reinterpret_cast<const float*>(&value), count);
}

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

                                      if (const std::string name = techniqueArr; techniqueName.empty() || name == techniqueName)
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
