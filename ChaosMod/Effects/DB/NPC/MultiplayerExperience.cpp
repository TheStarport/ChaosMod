#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Utilities/ChatConsole.hpp"

class MultiplayerExperience final : public ActiveEffect
{
        std::array<std::wstring, 30> lines = {
            L"Cardamine, Cardamine, everyone wants Cardamine.",
            L"I hope this cargo hold full of water fetches a nice price.",
            L"The name is Cosmo Van Nostrom. Remember it! I’m about to be your worst nightmare, flatfoot. This is wrongful"
            L"arrest, harassment and wanton destruction of my property.",
            L"Don't get caught off guard, I don't want to end up a corpse before my time because YOU were day dreaming.",
            L"Mind if I take a peak in your hold? Didn't think so?",
            L"Is that a Rheinland Cruiser?",
            L"I have your artefact. The one Sinclair lost on Pygar, I’ve found it. That’s why I came.",
            L"I’m hit! My cruise engines are down!",
            L"Target the nearest fighter. We can win this one. We have to.",
            L"We’ll meet again. You’ll never make it out of Bretonia alive!",
            L"Open fire, chaps. We have to take out that cruiser!",
            L"I'm going to use whats left of you to grease my ship.",
            L"That's the Donau, Admiral Schultzky’s flagship.",
            L"Give me the tome!",
            L"I'm going to tell Juni that you're not even worth the junk you're flying.",
            L"The Blood Dragons are the best fighter pilots in the colonies.",
            L"Mr Trent! We need to talk. I was with you on Freeport 7 – we met on the rescue ship. Something’s going on."
            L"I’m being followed.",
            L"Make sure he lives, he owes me some credits.",
            L"Richard Winston Tobias, Esquire, dear Lady.",
            L"I can’t remember when I ever saw such a vision of loveliness.",
            L"Trent we have to go faster!",
            L"My radar is showing new enemies!",
            L"I could kill you right here before you even know it.",
            L"I have one in my sights, taking the shot.",
            L"Got one in my sights, dropping the hammer.",
            L"Hand over the Artefact and we will spare your lives!",
            L"WHAT? Rheinlanders!?",
            L"I hear you can buy anything on a Junker base.",
            L"I'm a trader out of the Colorodo system. I'm carrying Oxygen to the Dresden system.",
            L"I'm on patrol out of the New York System. We're heading to the California system. We have 104 waypoints left to go.",
        };

        float interval = 3.0f;

        void Update(const float delta) override
        {
            interval -= delta;
            if (interval < 0.0f)
            {
                interval = 3.0f;

                std::vector<CShip*> ships = {};
                Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT, [&ships](CShip* ship) { ships.emplace_back(ship); });

                if (ships.size() == 1)
                {
                    return;
                }

                CShip* ship;
                while (true)
                {
                    ship = ships[Get<Random>()->Uniform(0u, ships.size() - 1)];
                    if (!ship->is_player())
                    {
                        break;
                    }
                }

                const auto line = lines[Get<Random>()->Uniform(0u, lines.size() - 1)];

                int rep = 0;
                pub::SpaceObj::GetRep(ship->id, rep);
                FmtStr name1(0, nullptr);
                FmtStr name2(0, nullptr);
                const ushort* str = nullptr;
                Reputation::Vibe::GetName(rep, name1, name2, str);

                if (!rep)
                {
                    return;
                }

                auto color = Get<Random>()->Uniform(0u, 0xFFFFFFu);
                color += (0x40 << 24);
                Get<ChatConsole>()->Tra(color, -1);
                Get<ChatConsole>()->Print(std::format(L"{}:", std::wstring(reinterpret_cast<const wchar_t*>(str))), false);
                Get<ChatConsole>()->Style(static_cast<USHORT>(ChatConsole::Style::Dialog));
                Get<ChatConsole>()->Print(std::format(L" {}", line), true);
            }
        }

    public:
        explicit MultiplayerExperience(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(MultiplayerExperience, {
    .effectName = "The Multiplayer Experience",
    .description = "Wouldn't it be more immersive if the NPCs talked like players did in multiplayer?",
    .category = EffectType::Npc
});