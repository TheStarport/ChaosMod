#pragma once

class YouSpinMeRightRound final : public ActiveEffect
{
        const Vector v = Vector(100000.0f, 100000.0f, 100000.0f);
        const Vector vv = Vector(0.0f, 0.0f, 0.0f);

        void Begin() override
        {
            Utils::ForEachSolar([this](auto solar) { solar->init_physics(vv, v); });
        }

        void Update(float delta) override
        {
            if (const auto ship = Utils::GetCShip())
            {
                {
                    auto newOp = pub::AI::DirectiveIdleOp();
                    newOp.fireWeapons = 0;
                    pub::AI::SubmitDirective(ship->id, &newOp);
                }
            }
        }

        void End() override
        {
            Utils::ForEachSolar([this](auto solar) { PhySys::DeactivatePhysics(solar); });
        }

    public:
        DefEffectInfo("You Spin Me Right Round", 1.0f, EffectType::Movement);
};
