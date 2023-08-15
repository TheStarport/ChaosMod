// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/MemoryEffect.hpp"

class BossMusic final : public ActiveEffect
{
        PDWORD currentPlayingOffset = 0;
        PDWORD currentSystemOffset = 0;
        PDWORD currentBattleOffset = 0;

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

                MemUtils::WriteProcMem(currentBattleOffset, currentPlayingOffset, sizeof(uint));
                ID_String ids{};
                ids.id = target;
                callMusic(&ids, *unk1, *unk2);
            }
        }

        void End() override
        {
            // TODO: calculate if anything near by is hostile and play the right type of music

            MemUtils::ReadProcMem(reinterpret_cast<PDWORD>(RelOfs("freelancer.exe", AddressTable::CurrentSystemSpace)), currentPlayingOffset, sizeof(uint));
        };

    public:
        BossMusic()
        {
            currentPlayingOffset = reinterpret_cast<PDWORD>(RelOfs("freelancer.exe", AddressTable::CurrentPlayingMusic));
            currentSystemOffset = reinterpret_cast<PDWORD>(RelOfs("freelancer.exe", AddressTable::CurrentSystemSpace));
            currentBattleOffset = reinterpret_cast<PDWORD>(RelOfs("freelancer.exe", AddressTable::CurrentSystemBattle));
        }

        DefEffectInfo("Why Do I Hear Boss Music?", 1.0f, EffectType::Audio);
};
