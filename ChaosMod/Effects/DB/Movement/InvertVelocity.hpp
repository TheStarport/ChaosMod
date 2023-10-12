#pragma once

class InvertVelocity final : public ActiveEffect
{
        static void InvertEngines(CShip* ship)
        {
            EngineEquipConsts::CRUISING_SPEED = -EngineEquipConsts::CRUISING_SPEED;

            CEquipTraverser tr;
            CEquip* equip = GetEquipManager(ship)->Traverse(tr);
            while (equip)
            {
                EquipDesc e;
                equip->GetEquipDesc(e);
                if (CEEngine* engine = CEEngine::cast(equip))
                {
                    const float thrustValue = *reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(engine) + 3) + 112);
                    *reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(engine) + 3) + 112) = -thrustValue;
                }
                else if (CEThruster* thruster = CEThruster::cast(equip))
                {
                    const float thrustValue = *(float*)(*(reinterpret_cast<DWORD*>(thruster) + 3) + 140);
                    *reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(thruster) + 3) + 140) = -thrustValue;
                }

                equip = GetEquipManager(ship)->Traverse(tr);
            }
        }

        void Begin() override
        {
            const auto ship = Utils::GetCShip();
            InvertEngines(ship);
        }

        void End() override
        {
            if (const auto ship = Utils::GetCShip())
            {
                InvertEngines(ship);
            }
        }

    public:
        DefEffectInfo("Invert Velocity", .75f, EffectType::Movement);
};
