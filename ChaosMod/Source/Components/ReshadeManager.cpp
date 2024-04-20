#include "PCH.hpp"

#include "Components/ReshadeManager.hpp"
#include "Components/GlobalTimers.hpp"

using namespace reshade;
using namespace reshade::api;

// ReSharper disable twice CppInconsistentNaming
extern "C" __declspec(dllexport) const char* NAME = "Chaos Mod Shader Replacement";
extern "C" __declspec(dllexport) const char* DESCRIPTION = "Chaos Mod will replace the active shader with various visual effects!";

void ReshadeManager::OnInitEffect(effect_runtime* runtime)
{
    auto i = Get<ReshadeManager>();
    if (!i->runtime)
    {
        // Set a timer to disable any active effects one second after start, so they can all init
        Get<GlobalTimers>()->AddTimer(
            [i](auto delta)
            {
                for (const auto& entry : std::filesystem::recursive_directory_iterator("reshade-shaders"))
                {
                    if (!entry.is_regular_file() || entry.file_size() > UINT_MAX || !entry.path().generic_string().ends_with(".fx"))
                    {
                        continue;
                    }

                    i->ToggleTechnique(entry.path().filename().generic_string(), "", false);
                }
                return true;
            },
            1.0f);
    }

    // Assume last created effect runtime is the main one
    i->runtime = runtime;
}

void ReshadeManager::OnDestroyEffect(effect_runtime* runtime) { Get<ReshadeManager>()->runtime = nullptr; }

std::optional<std::pair<Vector, effect_uniform_variable>> ReshadeManager::GetUniformFloat(const std::string_view fx, const std::string& name, const size_t valueCount) const
{
    const auto var = runtime->find_uniform_variable(fx.data(), name.c_str());
    if (!var.handle)
    {
        return std::nullopt;
    }

    Vector vec;
    runtime->get_uniform_value_float(var, reinterpret_cast<float*>(&vec), valueCount);

    return { std::make_pair(vec, var) };
}

void ReshadeManager::SetUniformFloat(const std::string_view fx, const std::string& name, const Vector& value, const size_t count) const
{
    if (!runtime)
    {
        return;
    }

    const auto vector = GetUniformFloat(fx.data(), name, count);
    if (!vector.has_value())
    {
        throw std::runtime_error("Shader variable was not found and could not be set.");
    }

    runtime->set_uniform_value_float(vector->second, reinterpret_cast<const float*>(&value), count);
}

void ReshadeManager::SetUniformInt(const std::string_view fx, const std::string_view name, const int value, const int value2) const
{
    if (!runtime)
    {
        return;
    }

    const auto var = runtime->find_uniform_variable(fx.data(), name.data());
    if (!var.handle)
    {
        return;
    }

    runtime->set_uniform_value_int(var, value, value2);
}

void ReshadeManager::UpdateTexture(const std::string_view fx, const std::string_view name, const uint32_t width, const uint32_t height, const uint8_t* pixels) const
{
    if (!runtime)
    {
        return;
    }

    const auto var = runtime->find_texture_variable(fx.data(), name.data());
    if (!var.handle)
    {
        return;
    }

    runtime->update_texture(var, width, height, pixels);
}

void ReshadeManager::ToggleTechnique(std::string_view fx, const std::string& techniqueName, bool state) const
{
    if (!runtime)
    {
        return;
    }

    runtime->enumerate_techniques(fx.data(),
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
