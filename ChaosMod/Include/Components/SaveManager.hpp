#pragma once
#include "Components/ConfigManager.hpp"
#include "Components/ChatConsole.hpp"
#include "Memory/OffsetHelper.hpp"

class SaveManager
{
        typedef bool(__fastcall* SavePlayerT)(char* playerData, void* edx, const CHARACTER_ID&, LPCWSTR, uint);
        inline static const SavePlayerT savePlayer = reinterpret_cast<SavePlayerT>(reinterpret_cast<DWORD>(pub::Save) - 0x6d5efa0 + 0x6d4c800);

        static char GetNextSaveSlot()
        {
            char dir[MAX_PATH];
            GetUserDataPath(dir);

            const std::string path = std::format("{}/Accts/SinglePlayer/", dir);
            char slot = 47; // 48 = '0'
            i64 lastFileWriteTime = 0;
            for (const auto& file : std::filesystem::directory_iterator(path))
            {
                auto stem = file.path().filename().stem().generic_string();
                if (stem.length() > 1u || !std::isdigit(stem[0]))
                {
                    continue;
                }

                if (const i64 fileTime = file.last_write_time().time_since_epoch().count(); fileTime > lastFileWriteTime)
                {
                    lastFileWriteTime = fileTime;
                    slot = stem[0];
                }
            }

            slot++;
            if (slot == ':')
            {
                slot = '0';
            }

            return slot;
        }

        static char SaveFile()
        {
            static const auto flStringBuffer = reinterpret_cast<LPWSTR>(0x66dc60);

            char slot = GetNextSaveSlot();
            CHARACTER_ID cid;
            sprintf_s(cid.charFilename, "%c.fl", slot);

            slot++;
            std::wstring slotStr(1, slot);
            if (slot == ':') // : comes after 9 in the ascii table
            {
                slotStr = L"10";
            }

            const std::wstring saveDescription = std::format(L"Chaos Autosave: {}", slotStr);
            wcscpy(flStringBuffer, saveDescription.c_str());

            savePlayer(Players.data, nullptr, cid, flStringBuffer, 0);
            return slot;
        }

        inline static float timer = 60 * 5;

    public:
        SaveManager() = delete;

        static void SaveTimer(float delta)
        {
            timer -= delta;
            if (timer <= 0.0f)
            {
                Save();
            }
        }

        static void Save()
        {
            const auto ship = Utils::GetCShip();
            if (!ship || OffsetHelper::IsInMission())
            {
                timer = static_cast<float>(Get<ConfigManager>()->timeBetweenSavesInSeconds);
                return;
            }

            if (!Get<ConfigManager>()->allowAutoSavesDuringCombat)
            {
                bool hostileNear = false;
                Utils::ForEachObject<CShip>(CObject::CSHIP_OBJECT,
                                            [ship, &hostileNear](auto next)
                                            {
                                                if (ship == next)
                                                {
                                                    return;
                                                }

                                                int otherShipRep;
                                                pub::SpaceObj::GetRep(next->id, otherShipRep);

                                                int playerRep;
                                                pub::SpaceObj::GetRep(ship->id, playerRep);

                                                float fAttitude = 0.0f;
                                                pub::Reputation::GetAttitude(otherShipRep, playerRep, fAttitude);

                                                if (auto distance = glm::length(ship->position - next->position);
                                                    std::abs(distance) < 2000.0f && fAttitude < -0.55f)
                                                {
                                                    hostileNear = true;
                                                }
                                            });

                if (hostileNear)
                {
                    timer = 30.0f;
                    return;
                }
            }

            timer = static_cast<float>(Get<ConfigManager>()->timeBetweenSavesInSeconds);

            auto slot = SaveFile();
            Get<ChatConsole>()->Tra(0xFFFFFF40, -1);
            Get<ChatConsole>()->Print(std::format(L"Autosaved into slot {}", slot), true);
        }
};
