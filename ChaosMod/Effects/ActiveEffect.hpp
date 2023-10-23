#pragma once

enum class EffectType
{
    Audio,
    Interface,
    Meta,
    Misc,
    Movement,
    Npc,
    Spawning,
    StatManipulation,
    Teleport,
    Visual,
};

enum class EffectExclusion
{
    None,
    Movie
};

class ActiveEffect
{
        static std::vector<std::unique_ptr<ActiveEffect>> allEffects;

    public:
        struct EffectInfo
        {
                std::string effectName{};
                bool isTimed;
                EffectType category;
                float timingModifier = 1.0f;
                float fixedTimeOverride = 0.0f;
                EffectExclusion exclusion;
                uint weight = 100;
        };

        virtual void Init() {}
        virtual void Begin() {}
        virtual void OnLoad() {}
        virtual void OnShipDestroyed(DamageList* dmgList, CShip* ship) {}
        virtual void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry) {}
        virtual void OnApplyDamageAfter(uint hitSpaceObj, DamageList* dmgList, const DamageEntry& dmgEntry) {}
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

    protected:
        struct EffectInfoBuilder
        {
                explicit EffectInfoBuilder(const std::string& effectName, EffectType effectType)
                {
                    name = effectName;
                    category = effectType;
                }

                EffectInfoBuilder& WithOneShot()
                {
                    relativeTime = 0.0f;
                    absoluteTime = 0.0f;
                    return *this;
                }

                EffectInfoBuilder& WithRelativeTime(const float time)
                {
                    relativeTime = time;
                    return *this;
                }

                EffectInfoBuilder& WithAbsoluteTime(const float time)
                {
                    absoluteTime = time;
                    return *this;
                }

                EffectInfoBuilder& WithWeight(const uint newWeight)
                {
                    weight = newWeight;
                    return *this;
                }

                EffectInfoBuilder& WithExclusion(const EffectExclusion effectExclusion)
                {
                    exclusion = effectExclusion;
                    return *this;
                }

                [[nodiscard]]
                EffectInfo Create() const
                {
                    return { name, !(relativeTime == 0.0f && absoluteTime == 0.0f), category, relativeTime, absoluteTime, exclusion, weight };
                }

            private:
                EffectType category;
                EffectExclusion exclusion = EffectExclusion::None;
                uint weight = 100;
                float relativeTime = 1.0f;
                float absoluteTime = 0.0f;
                std::string name;
        };
};

#define DefaultEffectInfo(name, category)                                        \
    const EffectInfo& GetEffectInfo() override                                   \
    {                                                                            \
        static const EffectInfo ef = EffectInfoBuilder(name, category).Create(); \
        return ef;                                                               \
    }

#define OneShotEffectInfo(name, category)                                                      \
    const EffectInfo& GetEffectInfo() override                                                 \
    {                                                                                          \
        static const EffectInfo ef = EffectInfoBuilder(name, category).WithOneShot().Create(); \
        return ef;                                                                             \
    }

#define RelativeEffectInfo(name, category, time)                                                        \
    const EffectInfo& GetEffectInfo() override                                                          \
    {                                                                                                   \
        static const EffectInfo ef = EffectInfoBuilder(name, category).WithRelativeTime(time).Create(); \
        return ef;                                                                                      \
    }

#define AbsoluteEffectInfo(name, category, time)                                                        \
    const EffectInfo& GetEffectInfo() override                                                          \
    {                                                                                                   \
        static const EffectInfo ef = EffectInfoBuilder(name, category).WithAbsoluteTime(time).Create(); \
        return ef;                                                                                      \
    }
