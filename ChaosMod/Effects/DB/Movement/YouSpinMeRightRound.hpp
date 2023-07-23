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
            const auto ship = Utils::GetCShip();
            if (ship)
            {
                const auto directive = ship->get_behavior_interface()->get_current_directive();
                if (const auto op = directive->op; op == pub::AI::OP_TYPE::M_DOCK || op == pub::AI::OP_TYPE::M_FACE)
                {
                    const auto newOp = pub::AI::DirectiveIdleOp();
                    ship->get_behavior_interface()->set_current_directive(*directive, &newOp);
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
