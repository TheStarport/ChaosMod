#pragma once

class AssetTracker
{
    public:
        struct Npc
        {
                uint loadout;
                uint arch;
                uint level;
        };

    private:
        using ShipCreateType = int (*)(unsigned& id, const pub::SpaceObj::ShipInfo& info);
        inline static FunctionDetour<ShipCreateType> shipCreateType = FunctionDetour(pub::SpaceObj::Create);

        inline static std::unordered_map<uint, Npc> npcMap;

        static int OnShipCreate(unsigned& id, const pub::SpaceObj::ShipInfo& info)
        {
            shipCreateType.UnDetour();
            const auto ret = shipCreateType.GetOriginalFunc()(id, info);
            shipCreateType.Detour(OnShipCreate);

            if (id)
            {
                npcMap[id] = { info.loadout, info.shipArchetype, info.level };
            }

            return ret;
        }

    public:
        static Npc* GetNpcCreationParams(const unsigned id)
        {
            const auto npc = npcMap.find(id);
            if (npc == npcMap.end())
            {
                return nullptr;
            }

            return &npc->second;
        }

        static void StartDetours() { shipCreateType.Detour(OnShipCreate); }

        AssetTracker() = delete;
};