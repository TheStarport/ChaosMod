#pragma once

#define WIN32_LEAN_AND_MEAN
#include <refl.hpp>

using byte = unsigned char;
using uint = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

#include <Windows.h>

#include <mmsystem.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <winuser.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <numbers>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <FLCore/Common.hpp>
#include <FLCore/FLCoreDALib.h>
#include <FLCore/FLCoreServer.h>
#include <Singleton.hpp>

#include "Components/Component.hpp"
#include "Components/Random.hpp"

// Our Utils
#include "Utils.hpp"

// FLSDK Utils
#include <Utils/Detour.hpp>
#include <Utils/Utils.hpp>

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

class CrashCatcher;
class ChaosMod : public Singleton<ChaosMod>
{
        static void DelayedInit();
        static void* ScriptLoadHook(const char* script);
        static void __stdcall TerminateAllThreads();
        static BOOL __stdcall FreeLibraryDetour(const HMODULE handle);
        CrashCatcher* cc;

    public:
        ChaosMod();
        ~ChaosMod();

        static std::optional<std::string> HashLookup(uint hash);
        static void SetLanguage(Language lang);
        static std::string GetInfocardName(uint ids);
        static bool RunningOnWine();
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
