#pragma once

enum class EffectType
{
    Audio,
    Interface,
    Meta,
    Misc,
    Movement,
    Npc,
    StatManipulation,
    Teleport,
    Visual,
};

class ActiveEffect
{
        static std::vector<std::unique_ptr<ActiveEffect>> allEffects;

    public:
        struct EffectInfo
        {
                EffectInfo(std::string name, const float timeModifier, const EffectType category, const float fixedTimeOverride = 0.0f)
                    : effectName(std::move(name)), isTimed(timeModifier > 0.0f || fixedTimeOverride != 0.0f), category(category), timingModifier(timeModifier),
                      fixedTimeOverride(fixedTimeOverride)
                {}

                std::string effectName{};
                bool isTimed;
                EffectType category;
                float timingModifier = 1.0f;
                float fixedTimeOverride = 0.0f;
        };

        virtual void Init() {}
        virtual void Begin() {}
        virtual void Update(float delta) {}
        virtual void FrameUpdate(float delta) {}
        virtual void End() {}
        virtual ~ActiveEffect() = default;

        virtual const EffectInfo& GetEffectInfo() = 0;

        static std::vector<ActiveEffect*> GetAllEffects()
        {
            std::vector<ActiveEffect*> rawVector;
            rawVector.reserve(allEffects.size()); //  avoids unnecessary reallocations
            std::ranges::transform(allEffects, std::back_inserter(rawVector), [](auto& ptr) { return ptr.get(); });
            return rawVector;
        }

        template <class Effect>
            requires std::is_base_of_v<ActiveEffect, Effect>

        static void RegisterEffect()
        {
            allEffects.emplace_back(std::make_unique<Effect>(Effect()));
        }
};

#define DefEffectInfo(name, timeModifier, category)                    \
    const EffectInfo& GetEffectInfo()                                  \
    {                                                                  \
        static const EffectInfo ef = { name, timeModifier, category }; \
        return ef;                                                     \
    }
#define DefEffectInfoFixed(name, absoluteTime, category)                     \
    const EffectInfo& GetEffectInfo()                                        \
    {                                                                        \
        static const EffectInfo ef = { name, 0.0f, category, absoluteTime }; \
        return ef;                                                           \
    }
