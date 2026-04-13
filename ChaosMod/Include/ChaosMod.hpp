#pragma once
#include "ChaosConfig.hpp"

using byte = unsigned char;
using uint = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif

#define __             std::ignore
#define F(...)         std::format(__VA_ARGS__)
#define RC(type, cast) reinterpret_cast<type>(cast)
#define SC(type, cast) static_cast<type>(cast)

#include <Windows.h>

#include <mmsystem.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <winuser.h>
#undef SendMessage
#undef PlaySound

#include <array>
#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <queue>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std::chrono_literals;
using namespace std::string_view_literals;

#include <FLCore/FLCoreDefs.hpp>

#include "Types/BaseRef.hpp"
#include "Types/ClientId.hpp"
#include "Types/EquipmentRef.hpp"
#include "Types/GoodRef.hpp"
#include "Types/GroupId.hpp"
#include "Types/ObjectRef.hpp"
#include "Types/RepGroupId.hpp"
#include "Types/RepId.hpp"
#include "Types/ShipRef.hpp"
#include "Types/SystemRef.hpp"
#include "Utils/Action.hpp"
#include <concurrencpp/concurrencpp.h>

#include "TaskScheduler.hpp"

#include "ChaosConfig.hpp"

#include "FLCore/Common/CommonMethods.hpp"

#include <FLUF/Utils/Reflection.hpp>
#include <rfl.hpp>
#include <rfl/yaml.hpp>

#include <magic_enum/magic_enum_containers.hpp>
#include <magic_enum/magic_enum_flags.hpp>
using namespace magic_enum::bitwise_operators;

#include "Components/Component.hpp"
#include "Components/Random.hpp"

// FLSDK Utils
#include <Utils/Detour.hpp>
#include <Utils/Utils.hpp>

#include "Common/FlufCombinedModule.hpp"
#include "Fluf.hpp"

enum class Language
{
    English,
    German,
    French,
    // Russian
    Chinese,
    UwU,
    Chef,
    Cockney,
    Leet,
    LolCat,
    Pirate,
    Scottish
};

class ChaosMod : public FlufCombinedModule
{
        void OnClientLoad() override;
        void OnClientUpdate(float delta) override;
        void OnClientFixedUpdate(float delta, bool gamePaused) override;

        inline static ChaosMod* instance;
        std::shared_ptr<ChaosConfig> chaosConfig;

    public:
        ChaosMod();
        ~ChaosMod() override;

        static std::optional<std::string> HashLookup(uint hash);
        static void SetLanguage(Language lang);
        static std::string GetInfocardName(uint ids);
        static bool RunningOnWine();
        static ChaosConfig* GetConfig();
        ModuleMajorVersion MajorVersion() override;
        ModuleMinorVersion MinorVersion() override;
        std::string_view GetModuleName() override;
};

void Log(const std::string& log);
#ifdef _DEBUG
    #define DLog(x) Log(x)
#else
    #define DLog(x)
#endif

void SetWireFrames();
void Assert(bool cond, const std::string& text, const std::string& file, int line);

// ReSharper disable twice CppInconsistentNaming
#ifdef _DEBUG
    #define ASSERT(cond, text) Assert(cond, text, __FILE__, __LINE__)
#else
    #define ASSERT(cond, text) \
        (void)(cond);          \
        (void)(text);
#endif

#include "Reflection.hpp"

template <typename T>
    requires std::is_default_constructible_v<T>
struct ConfigHelper
{
        ConfigHelper() = delete;
        static std::optional<T> Load(std::string_view path, const bool fromUserData = false, const bool saveIfNotFound = true)
        {
            std::ifstream inFile(GetSaveLocation(path, fromUserData).data());
            if (!inFile.is_open())
            {
                if (saveIfNotFound)
                {
                    Save(path, T(), fromUserData);
                    return { T() };
                }

                return std::nullopt;
            }

            auto newConfig = rfl::yaml::read<T, rfl::DefaultIfMissing>(inFile);
            if (!newConfig.has_value())
            {
                const std::string err = std::format(
                    "Failed to load config file '{}'.\n\n{}\n\nClick 'OK' to reset the config.", path, newConfig.error().what());
                if (MessageBoxA(nullptr, err.c_str(), "Config Load Error", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
                {
                    std::exit(1);
                }

                Save(path, T(), fromUserData);
                return { T() };
            }

            return newConfig.value();
        }

        static bool Save(std::string_view path, const T& data, const bool fromUserData = false)
        {
            std::ofstream outFile(GetSaveLocation(path, fromUserData).data(), std::ios::trunc);
            if (!outFile.is_open())
            {
                return false;
            }

            outFile << rfl::yaml::write(static_cast<const T&>(data));
            outFile.close();

            return true;
        }

    private:
        static std::string GetSaveLocation(std::string_view path, const bool fromUserData)
        {
            if (fromUserData)
            {
                std::array<char, MAX_PATH> totalPath{};
                GetUserDataPath(totalPath.data());
                return std::format("{}/{}", std::string(totalPath.data()), path);
            }

            return std::format("{}", path);
        }
};

template <typename T>
    requires std::is_base_of_v<CObject, T>
void ForEachObject(const CObject::Class cls, const std::function<void(T*)>& func)
{
    if (auto* obj = dynamic_cast<T*>(CObject::FindFirst(cls)))
    {
        func(obj);

        T* next;
        do
        {
            next = dynamic_cast<T*>(CObject::FindNext());
            if (next)
            {
                func(next);
            }
        }
        while (next);
    }
}