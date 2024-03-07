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
std::optional<std::string> HashLookup(uint hash);

enum class Language
{
    English,
    German,
    French,
    // Russian
    Chinese,
    UwU
};

std::string GetInfocardName(uint ids);
void SetLanguage(Language lang);

#define ASSERT(cond, text) Assert(cond, text, __FILE__, __LINE__)

struct Weight : refl::attr::usage::field
{
        uint w = 3u;
        explicit constexpr Weight(const uint weight) : w(weight) {}
};

struct Clamp : refl::attr::usage::field
{
        float min = 0;
        float max = INT_MAX;
        explicit constexpr Clamp(float min, float max) : min(min), max(max) {}
};

// If higher values are considered a nerf
struct Inverted : refl::attr::usage::field
{
        explicit constexpr Inverted() {}
};

// No-Boon-Or-Bust when a value is neither good nor bad
struct Nbb : refl::attr::usage::field
{
        explicit constexpr Nbb() {}
};

// Register all fields we want to dynamically edit

REFL_AUTO(type(Archetype::Ship), field(holdSize), field(linearDrag, Clamp(1.f, 1200.f), Inverted()), field(strafeForce), field(maxBankAngle, Clamp(0.f, 6.2f)),
          field(nudgeForce), field(maxNanobots, Clamp(0.f, 100.f)), field(maxShieldBats, Clamp(0.f, 100.f)), field(strafePowerUsage), field(isNomad, Nbb()));

REFL_AUTO(type(Archetype::Explosion), field(radius), field(impulse, Clamp(0.f, 10000.f)), field(hullDamage, Clamp(0.f, 5000.f)),
          field(energyDamage, Clamp(0.f, 5000.f)));
REFL_AUTO(type(Archetype::MotorData), field(delay, Clamp(0.f, 20.f)), field(acceleration, Clamp(0.f, 1000.f)), field(lifetime));

REFL_AUTO(type(Archetype::Light), field(alwaysOn), field(dockingLight));
REFL_AUTO(type(Archetype::Power), field(capacity, Clamp(200.f, UINT_MAX)), field(chargeRate, Clamp(20.f, UINT_MAX)), field(thrustCapacity),
          field(thrustChargeRate));
REFL_AUTO(type(Archetype::Engine), field(maxForce, Clamp(25'000.f, 200'000.f)), field(linearDrag, Clamp(1.f, 1200.f), Inverted()),
          field(powerUsage, Clamp(0.f, 1000.f), Inverted()), field(cruisePowerUsage), field(cruiseChargeTime, Clamp(0.f, 100.f), Inverted()),
          field(reverseFraction, Clamp(0.f, 1.f)));

REFL_AUTO(type(Archetype::Launcher), field(powerUsage, Inverted()), field(muzzleVelocity, Clamp(10.f, 100'000.f)),
          field(damagePerFire, Weight(20u), Clamp(0.f, 5.f), Inverted()), field(hitPoints, Weight(20u), Clamp(100.f, 100'000.f)),
          field(refireDelay, Inverted()));
REFL_AUTO(type(Archetype::MineDropper, bases<Archetype::Launcher>));
REFL_AUTO(type(Archetype::CounterMeasureDropper, bases<Archetype::Launcher>), field(range));
REFL_AUTO(type(Archetype::Gun, bases<Archetype::Launcher>), field(turnRate, Clamp(0.f, 100'000.f)), field(dispersionAngle));

REFL_AUTO(type(Archetype::Projectile), field(lifeTime), field(ownerSafeTime, Inverted()), field(requiresAmmo), field(forceGunOri, Nbb()));
REFL_AUTO(type(Archetype::CounterMeasure, bases<Archetype::Projectile>), field(linearDrag, Inverted()), field(range),
          field(diversionPercentage, Clamp(0.f, 100.f), Inverted()));
REFL_AUTO(type(Archetype::Munition, bases<Archetype::Projectile>), field(hullDamage, Clamp(0.f, 5000.f)), field(energyDamage, Clamp(0.f, 5000.f)),
          field(timeToLock, Inverted()), field(seekerRange), field(seekerFovDeg), field(maxAngularVelocity), field(detonationDist),
          field(cruiseDisruptor, Nbb()));
REFL_AUTO(type(Archetype::Mine, bases<Archetype::Projectile>), field(linearDrag), field(detonationDist), field(seekerDist), field(acceleration),
          field(topSpeed));
REFL_AUTO(type(Archetype::ShieldGenerator), field(regenerationRate, Clamp(0.f, 1000.f)), field(maxCapacity, Clamp(10.f, 100'000.f)),
          field(constantPowerDraw, Clamp(0.f, 500.f), Inverted()), field(rebuildPowerDraw, Clamp(10.f, 500.f)), field(offlineRebuildTime, Clamp(2.f, 30.f)),
          field(hitPoints));

REFL_AUTO(type(Archetype::Thruster), field(hitPoints), field(powerUsage), field(maxForce, Clamp(24'000.f, 480'000.f)));
