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
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <FLCore/FLCoreCommon.h>
#include <FLCore/FLCoreDALib.h>
#include <FLCore/FLCoreServer.h>
#include <Singleton.hpp>

#include "../Utils.hpp"
#include "Utilities/Random.hpp"

#include <Utils/Detour.hpp>
#include <Utils/Utils.hpp>

void Log(const std::string& log);

void SetWireFrames();
void Assert(bool cond, const std::string& text, const std::string& file, int line);

std::string GetInfocardName(uint ids);

#define ASSERT(cond, text) Assert(cond, text, __FILE__, __LINE__)

// Register all fields we want to dynamically edit
REFL_AUTO(type(Archetype::Launcher), field(powerUsage), field(muzzleVelocity), field(damagePerFire), field(hitPoints), field(refireDelay));

REFL_AUTO(type(Archetype::Gun, bases<Archetype::Launcher>), field(turnRate), field(dispersionAngle));
