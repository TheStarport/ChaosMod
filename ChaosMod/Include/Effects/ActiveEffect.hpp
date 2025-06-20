#pragma once

struct MunitionImpactData;
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
    Movie,
    Visual,
    SendComm,
    FireResource,
    RefireDelay,
    GetMass,
    StarSphere,
};

class ActiveEffect
{
        inline static std::vector<ActiveEffect*> allEffects;

    public:
        struct EffectInfo
        {
                std::string effectName{};
                std::string description;
                EffectType category = EffectType::Audio;
                float timingModifier = 1.0f;
                float fixedTimeOverride = 0.0f;
                bool isTimed = true;
                EffectExclusion exclusion = EffectExclusion::None;
                uint weight = 5;
                bool continueOnBases = false;
        };

    private:
        EffectInfo effectInfo;

    public:
        virtual void Init() {}
        virtual void Begin() {}
        virtual bool CanSelect() { return true; }
        virtual void OnLoad() {}
        virtual void OnShipDestroyed(DamageList* dmgList, CShip* ship) {}
        virtual void OnMunitionHit(EqObj* hitObject, MunitionImpactData* impact, DamageList* dmgList) {}
        virtual void OnMunitionHitAfter(EqObj* hitObject, MunitionImpactData* impact, DamageList* dmgList) {}
        virtual bool OnExplosion(EqObj* hitObject, ExplosionDamageEvent* explosion, DamageList* dmgList) { return true; }
        virtual void OnConsumeFireResources(CELauncher* launcher) {};
        virtual uint OnSoundEffect(const uint hash) { return hash; }
        virtual void Update(float delta) {}
        virtual void FrameUpdate(float delta) {}
        virtual void End() {}
        virtual void Cleanup() {}
        // Called regardless of whether the effect is active or not
        virtual void OnSystemUnload() {}
        virtual void OnJumpInComplete() {}
        virtual ~ActiveEffect() = default;
        explicit ActiveEffect(EffectInfo effectInfo) : effectInfo(std::move(effectInfo)) { allEffects.emplace_back(this); }

        const EffectInfo& GetEffectInfo() const { return effectInfo; }
        static const std::vector<ActiveEffect*>& GetAllEffects() { return allEffects; }

        ActiveEffect(const ActiveEffect&) = delete;
        ActiveEffect& operator=(const ActiveEffect&) = delete;
        ActiveEffect& operator=(ActiveEffect&&) = delete;
        ActiveEffect(ActiveEffect&&) = delete;
};

#define SetupEffect(type, ...)                                                  \
    namespace                                                                   \
    {                                                                           \
        RegisterEffect effect(new type(ActiveEffect::EffectInfo(__VA_ARGS__))); \
    }

class RegisterEffect final
{
        ActiveEffect* effect;

    public:
        explicit RegisterEffect(ActiveEffect* effect) : effect(effect) {}
        ~RegisterEffect() { delete effect; }

        RegisterEffect(const RegisterEffect&) = delete;
        RegisterEffect& operator=(const RegisterEffect&) = delete;
        RegisterEffect& operator=(RegisterEffect&&) = delete;
        RegisterEffect(RegisterEffect&&) = delete;
};
