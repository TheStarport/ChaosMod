#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class LowBudgetSciFiMovie final : public ActiveEffect
{
        inline static std::vector<uint> pewHashes{};
        inline static std::vector<uint> replacementHashes = {
            2369019649, 2841267407, 3135368001, 2614337743, 2673861575, 3209977415, 3016016333, 3070855878, 2688758343, 2989019722, 2184583879, 2539423809,
            3076262977, 2560397249, 3097293761, 2266818753, 2375845313, 2912682433, 3193464207, 2656573839, 3193441679, 2656625039, 3193501071, 2965927109,
            2761999752, 2299940875, 2643243779, 3032838915, 2483682304, 2394261122, 2776681800, 2334361865, 2456277057, 2993138753, 2456270913, 2993116225,
            2456240193, 2844475208, 2812521284, 3011180109, 2895753859, 2757177165, 2220315469, 2757183309, 2220337997, 2757214029, 2894533830, 2357684422,
            2894560454, 2357694662, 2894562502, 2225550917, 2507558272, 3044422016, 2507548032, 3044460928, 2507595136, 2427008000, 2218240323, 2755122499,
            2218246467, 2755079491, 2218211651, 3181500943, 2274858249, 2811724041, 2274848009, 2811721993, 3103709194, 2692360065, 2641025934, 2634505409,
        };

        uint OnSoundEffect(const uint hash) override
        {
            if (std::ranges::find(replacementHashes, hash) == replacementHashes.end())
            {
                return hash;
            }

            return pewHashes[Random::i()->Uniform(0u, pewHashes.size() - 1)];
        }

    public:
        explicit LowBudgetSciFiMovie(const EffectInfo& info) : ActiveEffect(info)
        {
            pewHashes.clear();
            for (int i = 1; i <= 50; i++)
            {
                auto str = std::format("chaos_pew_{:0>2}", i);
                pewHashes.emplace_back(CreateID(str.c_str()));
            }

            if (pewHashes.empty())
            {
                Log("Failed to load any PEW SFX");
                pewHashes.emplace_back(CreateID("null"));
            }
        }
};

// clang-format off
SetupEffect(LowBudgetSciFiMovie, {
    .effectName = "Low Budget Sci-Fi Movie",
    .description = "Plays a comedic hit sound effect whenever any damage is applied.",
    .category = EffectType::Audio,
});