#pragma once

#include <reshade.hpp>

class ReshadeManager : public Component
{
        struct ResourceHash
        {
                size_t operator()(reshade::api::resource value) const
                {
                    // Simply use the handle (which is usually a pointer) as hash value (with some bits shaved off due to pointer alignment)
                    // ReSharper disable once CppRedundantCastExpression
                    return static_cast<size_t>(value.handle >> 4);
                }
        };

        struct __declspec(uuid("5D25B47E-2456-4EED-8AFA-E7F5B1CF9097")) DataContainer
        {
                reshade::api::effect_runtime* runtime = nullptr;
        };

        HMODULE thisDll = nullptr;
        inline static std::unordered_map<reshade::api::pipeline, reshade::api::pipeline, ResourceHash> replacePipelineLookup;

        static void OnInitEffect(reshade::api::effect_runtime* runtime);
        static void OnDestroyEffect(reshade::api::effect_runtime* runtime);

        reshade::api::effect_runtime* runtime = nullptr;

    public:
        [[nodiscard]]
        std::optional<std::pair<Vector, reshade::api::effect_uniform_variable>> GetUniformFloat(const std::string& name, size_t valueCount = 3) const;
        void SetUniformFloat(const std::string& name, const Vector& value, size_t count) const;
        void ToggleTechnique(const std::string& techniqueName, bool state) const;
        void SetHModule(HMODULE module);
        void InitReshade() const;
};