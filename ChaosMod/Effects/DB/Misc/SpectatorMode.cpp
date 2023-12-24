#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class SpectatorMode final : public ActiveEffect
{
    public:
        static void SetCloak(const bool state)
        {
            CShip* ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            CEquipTraverser tr;
            CEquip* equip = GetEquipManager(ship)->Traverse(tr);
            while (equip)
            {
                EquipDesc e;
                equip->GetEquipDesc(e);
                uint q = e.archId;
                if (CECloakingDevice::cast(equip))
                {
                    equip->Activate(state);
                    return;
                }

                equip = GetEquipManager(ship)->Traverse(tr);
            }
        }

    private:
        void Begin() override { SetCloak(true); }

        void End() override { SetCloak(false); }

    public:
        explicit SpectatorMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// TODO: Rewrite Spectator mode
