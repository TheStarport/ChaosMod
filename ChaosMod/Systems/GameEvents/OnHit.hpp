#pragma once

class OnHit
{
        inline static uint dmgToSpaceId;
        inline static uint dmgToClient;

        inline static FARPROC oldOnHit1;
        inline static FARPROC oldOnHit2;
        static void NakedOnHit1();
        static void NakedOnHit2();
        static void NakedOnDamage();

        static void __stdcall OnDamageHit(const char* ecx);
        static void __stdcall OnDamage(DamageList* dmgList, DamageEntry dmgEntry);

    public:
        OnHit() = delete;
        static void Detour();
};
