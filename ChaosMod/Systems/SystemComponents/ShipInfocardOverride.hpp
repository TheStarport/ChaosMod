#pragma once

class ShipInfocardOverride
{
        static int __fastcall GetShipIds(Archetype::Ship* ship);
        static void PatchShipInfoSellerIdsNaked(void);
        static void PatchShipInfoInventoryIdsNaked(void);

        inline static constexpr int shipOverrideIds = 66572;
        inline static constexpr int shipWeaponInfoIds = 66608;
        inline static Archetype::Ship* curShip = nullptr;

        inline static DWORD idsShipJump = 0x47BD44;
        inline static bool isInventory = false;

        inline static DWORD idsShipJump2 = 0x4B8C12;

    public:
        static std::optional<std::wstring> OverrideIds(uint ids);
        static void Init();
};
