// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/ActiveEffect.hpp"

class FlappyArchitecture final : public ActiveEffect
{
        struct AnimationData
        {
                int id;
                BayState state;
                bool close = false;
        };
        std::unordered_map<CObject*, std::vector<AnimationData>> objectAnimations;

        static void FlipBay(CShip* ship, bool forceClose = false)
        {
            static auto shieldDeactivate = reinterpret_cast<PBYTE>(0x62b2ba1);
            const BayState state = ship->get_bay_state();
            if (forceClose)
            {
                ship->close_bay();
                return;
            }

            if (state == BayState::Open)
            {
                ship->close_bay();
            }
            else if (state == BayState::Closed)
            {
                *shieldDeactivate = 0xEB;
                ship->open_bay();
                *shieldDeactivate = 0x74;
            }
        }

        static void ProcessAnimation(AnimationData& anim)
        {
            if (anim.state == BayState::Opening)
            {
                auto test = DALib::Anim->CurrentTime(anim.id);
                if (DALib::Anim->CurrentTime(anim.id) >= DALib::Anim->TotalTime(anim.id))
                {
                    if (anim.close)
                    {
                        DALib::Anim->Stop(anim.id);
                        DALib::Anim->Close(anim.id);
                        AnimDB::Rem(anim.id);
                        return;
                    }

                    anim.state = BayState::Closing;
                    DALib::Anim->Play(anim.id, AnimationDirection::Backwards, -2, 10, 0, 1, 1, 0);
                    AnimDB::Add(anim.id);
                }
            }
            else if (anim.state == BayState::Closing)
            {
                auto test = DALib::Anim->CurrentTime(anim.id);
                if (DALib::Anim->CurrentTime(anim.id) <= 0.0f)
                {
                    DALib::Anim->Stop(anim.id);

                    if (anim.close)
                    {

                        return;
                    }

                    anim.state = BayState::Opening;
                    DALib::Anim->Play(anim.id, AnimationDirection::Forwards, -2, 10, 0, 1, 1, 0);
                    AnimDB::Add(anim.id);
                }
            }
        }

        const std::array<const char*, 16> validDockingTargets = {
            "sc_open dock",  "sc_open dock1", "sc_open dock2", "sc_open dock3", "sc_open dock4", "sc_open dock5",  "sc_open dock6",  "sc_open dock7",
            "sc_open dock8", "sc_open dock9", "sc_open dockA", "sc_open dockB", "sc_open dockC", "sc_open dock a", "sc_open dock b", "sc_extend wing",
        };

        void Update(float delta) override
        {
            Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT, [](CShip* sh) { FlipBay(sh); });
            Utils::ForEachObject<CSolar>(
                CObject::Class::CSOLAR_OBJECT,
                [this](CSolar* solar)
                {
                    auto anim = objectAnimations.find(solar);
                    if (anim == objectAnimations.end())
                    {
                        std::vector<AnimationData> data;

                        for (auto& dock : validDockingTargets)
                        {
                            const int id = DALib::Anim->Open(
                                solar->get_archetype()->anim, solar->dunnoEngineObject1, dock, 0, 0); // NOLINT(bugprone-narrowing-conversions)

                            if (id == -1)
                            {
                                continue;
                            }

                            data.emplace_back(id, BayState::Opening);
                            DALib::Anim->Play(id, AnimationDirection::Forwards, -2, 10, 0, 1, 1, 0);
                            AnimDB::Add(id);
                        }

                        if (!data.empty())
                        {
                            objectAnimations[solar] = data;
                            anim = objectAnimations.find(solar);
                        }
                    }

                    if (anim != objectAnimations.end())
                    {
                        for (auto& dock : anim->second)
                        {
                            ProcessAnimation(dock);
                        }
                    }
                });
        }

        void End() override
        {
            Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT, [](CShip* sh) { FlipBay(sh, true); });
            for (auto& val : objectAnimations | std::views::values)
            {
                for (auto& [id, state, close] : val)
                {
                    close = true;
                }
            }

            Update(0);
            objectAnimations.clear();
        }

    public:
        DefEffectInfo("Flappy Architecture", 1.0f, EffectType::Misc);
};
