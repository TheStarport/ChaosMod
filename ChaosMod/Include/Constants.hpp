#pragma once

class Constants final
{
        inline static std::array bannedBases = { CreateID("[br_m_beryllium_miner]"),
                                                 CreateID("[br_m_hydrocarbon_miner]"),
                                                 CreateID("[br_m_niobium_miner]"),
                                                 CreateID("[co_khc_copper_miner]"),
                                                 CreateID("[co_khc_cobalt_miner]"),
                                                 CreateID("[co_kt_hydrocarbon_miner]"),
                                                 CreateID("[co_shi_h-fuel_miner]"),
                                                 CreateID("[co_shi_water_miner]"),
                                                 CreateID("[co_ti_water_miner]"),
                                                 CreateID("[gd_gm_h-fuel_miner]"),
                                                 CreateID("[gd_im_oxygen_miner]"),
                                                 CreateID("[gd_im_copper_miner]"),
                                                 CreateID("[gd_im_silver_miner]"),
                                                 CreateID("[gd_im_water_miner]"),
                                                 CreateID("[rh_m_diamond_miner]"),
                                                 CreateID("intro3_base"),
                                                 CreateID("intro2_base"),
                                                 CreateID("intro1_base"),
                                                 CreateID("intro4_base"),
                                                 CreateID("st03b_01_base"),
                                                 CreateID("st02_01_base"),
                                                 CreateID("st01_02_base"),
                                                 CreateID("iw02_03_base"),
                                                 CreateID("rh02_07_base"),
                                                 CreateID("li04_06_base"),
                                                 CreateID("li01_15_base"),
                                                 CreateID("bw01_05_base"),
                                                 CreateID("li01_04_base") };

        inline static std::array bannedSystems{ CreateID("st01"),  CreateID("st02"),  CreateID("st03"),
                                                CreateID("st02c"), CreateID("st03b"), CreateID("FP7_System") };

        inline static std::array storyFactions{ MakeId("chaos_jesus"), MakeId("fc_ouk_grp"), MakeId("fc_q_grp"),  MakeId("fc_uk_grp"), MakeId("fc_f_grp"),
                                                MakeId("fc_or_grp"),   MakeId("fc_rn_grp"),  MakeId("fc_kn_grp"), MakeId("fc_ln_grp"), MakeId("fc_n_grp") };

    public:
        Constants() = delete;

        static const auto& BannedBases() { return bannedBases; }
        static const auto& BannedSystems() { return bannedSystems; }
        static const auto& StoryFactions() { return storyFactions; }
};
