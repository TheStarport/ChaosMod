// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"

class BossMusic final : public ActiveEffect
{
        DWORD currentPlayingOffset = 0;
        DWORD currentSystemOffset = 0;
        DWORD currentBattleOffset = 0;

        using CallMusic = bool (*)(ID_String* ids, uint, float);
        CallMusic callMusic = reinterpret_cast<CallMusic>(0x4288E0);

        void Begin() override{};

        void Update(float delta) override
        {
            uint target;
            uint currentlyPlaying;
            MemUtils::ReadProcMem(currentPlayingOffset, &currentlyPlaying, sizeof(uint));
            MemUtils::ReadProcMem(currentBattleOffset, &target, sizeof(uint));

            if (target != currentlyPlaying)
            {
                const auto unk1 = reinterpret_cast<int*>(0x6798D0);
                const auto unk2 = reinterpret_cast<float*>(0x6798D4);

                MemUtils::WriteProcMem(currentBattleOffset, reinterpret_cast<const void*>(currentPlayingOffset), sizeof(uint));
                ID_String ids{};
                ids.id = target;
                callMusic(&ids, *unk1, *unk2);
            }
        }

        void End() override
        {
            // TODO: calculate if anything near by is hostile and play the right type of music

            MemUtils::ReadProcMem(RelOfs("freelancer.exe", AddressTable::CurrentSystemSpace), reinterpret_cast<void*>(currentPlayingOffset), sizeof(uint));
        };

    public:
        explicit BossMusic(const EffectInfo& info) : ActiveEffect(info)
        {
            currentPlayingOffset = RelOfs("freelancer.exe", AddressTable::CurrentPlayingMusic);
            currentSystemOffset = RelOfs("freelancer.exe", AddressTable::CurrentSystemSpace);
            currentBattleOffset = RelOfs("freelancer.exe", AddressTable::CurrentSystemBattle);
        }
};

// clang-format off
SetupEffect(BossMusic, {
    .effectName = "Why Do I Hear Boss Music?",
    .description = "Plays the combat music for the current system / mission.",
    .category = EffectType::Audio,
});