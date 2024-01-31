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

struct Weight : refl::attr::usage::field
{
        uint w = 3u;
        explicit constexpr Weight(const uint weight) : w(weight) {}
};

// Register all fields we want to dynamically edit

REFL_AUTO(type(Archetype::Ship), field(holdSize), field(linearDrag), field(strafeForce), field(maxBankAngle), field(nudgeForce), field(maxNanobots),
          field(maxShieldBats), field(strafePowerUsage), field(isNomad));

REFL_AUTO(type(Archetype::Explosion), field(radius), field(impulse), field(hullDamage), field(energyDamage));
REFL_AUTO(type(Archetype::MotorData), field(delay), field(acceleration), field(lifetime));

REFL_AUTO(type(Archetype::Light), field(alwaysOn), field(dockingLight));
REFL_AUTO(type(Archetype::Power), field(capacity), field(chargeRate), field(thrustCapacity), field(thrustChargeRate));
REFL_AUTO(type(Archetype::Engine), field(maxForce), field(linearDrag), field(powerUsage), field(cruisePowerUsage), field(cruiseChargeTime),
          field(reverseFraction));

REFL_AUTO(type(Archetype::Launcher), field(powerUsage), field(muzzleVelocity), field(damagePerFire, Weight(1u)), field(hitPoints, Weight(1u)),
          field(refireDelay));
REFL_AUTO(type(Archetype::MineDropper, bases<Archetype::Launcher>));
REFL_AUTO(type(Archetype::CounterMeasureDropper, bases<Archetype::Launcher>), field(range));
REFL_AUTO(type(Archetype::Gun, bases<Archetype::Launcher>), field(turnRate), field(dispersionAngle));

REFL_AUTO(type(Archetype::Projectile), field(lifeTime), field(ownerSafeTime), field(requiresAmmo), field(forceGunOri));
REFL_AUTO(type(Archetype::CounterMeasure, bases<Archetype::Projectile>), field(linearDrag), field(range), field(diversionPercentage));
REFL_AUTO(type(Archetype::Munition, bases<Archetype::Projectile>), field(hullDamage), field(energyDamage), field(timeToLock), field(seekerRange),
          field(seekerFovDeg), field(maxAngularVelocity), field(detonationDist), field(cruiseDisruptor));
REFL_AUTO(type(Archetype::Mine, bases<Archetype::Projectile>), field(linearDrag), field(detonationDist), field(seekerDist), field(acceleration),
          field(topSpeed));
REFL_AUTO(type(Archetype::ShieldGenerator), field(regenerationRate), field(maxCapacity), field(constantPowerDraw), field(rebuildPowerDraw),
          field(offlineRebuildTime), field(hitPoints));

REFL_AUTO(type(Archetype::Thruster), field(hitPoints), field(powerUsage), field(maxForce));
