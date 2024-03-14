#pragma once

enum class AddressTable : DWORD
{
    // Audio
    CurrentSystemSpace = 0x2798F4,
    CurrentSystemDanger = 0x2797F0,
    CurrentSystemBattle = 0x2798EC,
    CurrentPlayingMusic = 0x27895C,

    // Interface
    HudPowerColor = 0x0D57AC,
    HudShieldColor = 0x0D5843,
    HudHullColor = 0x0D588D,
    EnergyBarHeight = 0x1D7E68,
    TotalBarWidth = 0x1D7E58,

    // Visual
    MaxDrawDistanceBases = 0x213EC8,
    MaxDrawDistancePlanets = 0x014053, // Rendcomp.dll
    MaxDrawDistanceShips = 0x1C8910,

    // Other
    TradelaneSpeed = 0x13F3CC,
    CollisionDamage = 0x18D184,

    ShipDestroyedFunction = 0x87274,

    DamageHit1 = 0x853F4,
    DamageHit2 = 0x872CC,
    DamageHit3 = 0x8761C,
    DamageHit4 = 0x85458,
    DamageHit5 = 0x87330,
    DamageHit6 = 0x87680,
    DamageApply = 0x841EC,
};

inline DWORD RelOfs(const std::string& str, AddressTable offset)
{
    const auto base = reinterpret_cast<DWORD>(GetModuleHandleA(str.c_str()));
    return base == 0 ? 0 : base + static_cast<DWORD>(offset);
}
