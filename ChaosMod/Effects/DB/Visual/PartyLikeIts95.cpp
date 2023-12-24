#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"
#include "Systems/DirectX/Drawing.hpp"

class PartyLikeIts95 final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("freelancer.exe", MaxDrawDistanceBases, sizeof(float))
        MemoryListItem("rendcomp.dll", MaxDrawDistancePlanets, sizeof(float))
        MemoryListItem("freelancer.exe", MaxDrawDistanceShips, sizeof(float))
        MemoryListEnd(offsets);
        // clang-format on

        void Begin() override
        {
            auto offset = offsets[0];
            float maxDistance = 4000.0f;
            MemUtils::WriteProcMem(offset.module + offset.offset, &maxDistance, sizeof(float));

            offset = offsets[1];
            MemUtils::WriteProcMem(offset.module + offset.offset, &maxDistance, sizeof(float));

            offset = offsets[2];
            maxDistance = 20'000.0f;
            MemUtils::WriteProcMem(offset.module + offset.offset, &maxDistance, sizeof(float));
        }

        void FrameUpdate(float delta) override
        {
            DrawingHelper::i()->SetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 7, true);
            DrawingHelper::i()->SetTextureStageState(1, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 7, true);
            DrawingHelper::i()->SetTextureStageState(2, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 7, true);
            DrawingHelper::i()->SetTextureStageState(3, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 7, true);
            DrawingHelper::i()->SetTextureStageState(4, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 7, true);
        }
        void End() override
        {
            MemoryEffect::End();
            DrawingHelper::i()->SetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 1, true);
            DrawingHelper::i()->SetTextureStageState(1, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 2, true);
            DrawingHelper::i()->SetTextureStageState(2, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 3, true);
            DrawingHelper::i()->SetTextureStageState(3, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 4, true);
            DrawingHelper::i()->SetTextureStageState(4, static_cast<D3DTEXTURESTAGESTATETYPE>(D3DTSS_MAXMIPLEVEL), 5, true);
        }

    public:
        explicit PartyLikeIts95(const EffectInfo& effectInfo) : MemoryEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PartyLikeIts95, {
    .effectName = "Party Like It's 95",
    .description = "Party like it's (Windows) 95! Your graphics settings seemed a little too high so I took care of it.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});