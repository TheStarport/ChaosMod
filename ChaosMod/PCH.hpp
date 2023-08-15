#pragma once

#define WIN32_LEAN_AND_MEAN

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
#include <ranges>
#include <set>
#include <sstream>
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
