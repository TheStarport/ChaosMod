#pragma once

class YouSpinMeRightRound final : public ActiveEffect
{
        const Vector v = Vector(500.0f, 500.0f, 500.0f);
        std::map<CSolar*, Matrix> originalOrientations;

        void Begin() override
        {
            originalOrientations.clear();
            Utils::ForEachObject<CSolar>(CObject::Class::CSOLAR_OBJECT, [this](auto solar)
            {
                ShipManipulator::SetAngularVelocity(solar, v);
                originalOrientations[solar] = solar->get_orientation();
            });
        }

        void Update(float delta) override
        {
            if (const auto ship = Utils::GetCShip())
            {
                // TODO: Reverse engineer current directive
                /*auto current = ship->get_behavior_interface()->get_current_directive();
                if (current)
                {
                    auto newOp = pub::AI::DirectiveIdleOp();
                    newOp.fireWeapons = 0;
                    pub::AI::SubmitDirective(ship->id, &newOp);
                }*/
            }
        }

        void End() override
        {
            Utils::ForEachObject<CSolar>(CObject::Class::CSOLAR_OBJECT, [this](auto solar)
            {
                ShipManipulator::SetAngularVelocity(solar, Vector());
                if (auto found = originalOrientations.find(solar); found != originalOrientations.end())
                {
                    pub::SpaceObj::Relocate(solar->get_id(), solar->system, solar->get_position(), found->second);
                }
            });
        }

    public:
        DefEffectInfo("You Spin Me Right Round", 1.0f, EffectType::Movement);
};
