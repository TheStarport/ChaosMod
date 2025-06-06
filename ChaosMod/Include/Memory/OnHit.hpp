#pragma once

struct MunitionImpactData
{
        uint attackerId;
        Archetype::Munition* munitionId;
        uint subObjId;
};

class OnHit
{

        using ShipMunitionHitFunc = void(__thiscall*)(EqObj*, MunitionImpactData*, DamageList*);
        inline static auto ShipMunitionHitCall = reinterpret_cast<ShipMunitionHitFunc>(0x6CE9350);
        static bool __stdcall ExplosionHit(EqObj* obj, ExplosionDamageEvent* explosion, DamageList* dmg);
        static void __stdcall SolarExplosionHit(EqObj* obj, ExplosionDamageEvent* explosion, DamageList* dmg);
        static void __fastcall ShipMunitionHit(EqObj* ship, void* edx, MunitionImpactData* data, DamageList* dmg);
        static void __stdcall ShipColGrpDmg(EqObj* obj, CArchGroup* colGrp, float& incDmg, DamageList* dmg);
        static void __stdcall ShipFuseLight(EqObj* ship, uint fuseCause, uint* fuseId, ushort sId, float radius, float fuseLifetime);
        static void __fastcall ShipEquipDamage(EqObj* obj, void* edx, CAttachedEquip* equip, float incDmg, DamageList* dmg);
        static void __stdcall ShipEquipmentDestroyed(EqObj* ship, const CEquip* eq, DamageEntry::SubObjFate fate, DamageList* dmgList);

        static void ExplosionHitNaked();
        static void SolarExplosionHitNaked();
        static void ShipColGrpDmgNaked();
        static void ShipFuseLightNaked();
        static void ShipEquipmentDestroyedNaked();

    public:
        OnHit() = delete;
        static void Detour();
};
