// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Memory/ShipInfocardOverride.hpp"

class SprechenSieDeutsch final : public ActiveEffect
{
        inline static std::map<Language, std::vector<HMODULE>> dllMap;

        inline static Language currentLang = Language::English;

        using GetIdsName = int (*)(void* resourceHandle, uint ids, wchar_t* buffer, int length);
        inline static std::unique_ptr<FunctionDetour<GetIdsName>> getIdsNameDetour;

        void Begin() override
        {
            const auto current = currentLang;
            while (current == currentLang)
            {
                currentLang = static_cast<Language>(Get<Random>()->Uniform(0, static_cast<int>(Language::Chinese)));
            }
        }

        static std::pair<HMODULE, int> GetModuleByIds(const uint ids)
        {
            int index = -1;
            if (ids < 65535)
            {
                index = 0;
            }
            else if (ids < 131071)
            {
                // InfoCards.dll
                index = 1;
            }
            else if (ids < 196607)
            {
                // MiscText.dll
                index = 2;
            }
            else if (ids < 262143)
            {
                // NameResources.dll
                index = 3;
            }
            else if (ids < 327679)
            {
                // EquipResources.dll
                index = 4;
            }
            else if (ids < 393215)
            {
                // OfferBribeResources.dll
                index = 5;
            }
            else if (ids < 458751)
            {
                // MiscTextInfo2.dll
                index = 6;
            }
            else
            {
                return { nullptr, index };
            }

            return { dllMap[currentLang].at(index), index };
        }

        inline static uint infocardLength = 0;
        inline static std::array<char, 65535> infocardBuffer;
        static char* LoadCustomInfocard(uint ids)
        {
            const auto [module, index] = GetModuleByIds(ids);
            if (!module)
            {
                return nullptr;
            }

            ids = ids - (65536 * index);

            char* mem = nullptr;
            infocardLength = 0;

            if (const HRSRC resourceLoc = FindResourceExA(module, MAKEINTRESOURCEA(23), MAKEINTRESOURCEA(ids), 0); resourceLoc != nullptr)
            {
                if (const HGLOBAL resourceHandle = LoadResource(module, resourceLoc);
                    resourceHandle == nullptr || (mem = static_cast<char*>(LockResource(resourceHandle))) == nullptr)
                {
                    return nullptr;
                }

                infocardLength = (SizeofResource(module, resourceLoc) >> 1) * 2;

                if (infocardLength > 2)
                {
                    if (infocardLength % 2 == 1 && !mem[infocardLength - 1])
                    {
                        // snip extra null byte
                        infocardLength--;
                    }

                    // skip BOM
                    if (mem[0] == static_cast<char>(0xFF) && mem[1] == static_cast<char>(0xFE))
                    {
                        infocardLength -= 2;
                        mem = mem + 2;
                    }
                }

                memcpy_s(infocardBuffer.data(), infocardBuffer.size(), mem, infocardLength);
                mem = infocardBuffer.data();
            }

            return mem;
        }

        static int GetIdsNameOverride(void* resourceHandle, uint ids, wchar_t* buffer, int length)
        {
            auto res = LoadCustomIdsName(ids, buffer, length);
            if (!res)
            {
                getIdsNameDetour->UnDetour();
                res = getIdsNameDetour->GetOriginalFunc()(resourceHandle, ids, buffer, length);
                getIdsNameDetour->Detour(GetIdsNameOverride);
            }

            return res;
        }

        static char* __stdcall GetIdsInfocardOverride(const uint ids)
        {
            std::memset(infocardBuffer.data(), '\0', infocardBuffer.size());

            auto override = ShipInfocardOverride::OverrideIds(ids);
            if (override.has_value())
            {
                auto& str = override.value();
                infocardLength = str.size() * 2;
                memcpy_s(infocardBuffer.data(), infocardBuffer.size(), str.data(), infocardLength);
                return infocardBuffer.data();
            }

            if (const auto buffer = LoadCustomInfocard(ids))
            {
                return buffer;
            }

            return nullptr;
        }

        static void GetIdsInfocardNaked();

        static void LoadLibraries()
        {
            // clang-format off
            const std::array<std::pair<std::string, Language>, 10> languages = {{
                { "cn", Language::Chinese },
                { "de", Language::German },
                { "fr", Language::French },
                { "uwu", Language::UwU },
                { "chef", Language::Chef },
                { "cockney", Language::Cockney },
                { "eleet", Language::Leet },
                { "lolcat", Language::LolCat },
                { "pirate", Language::Pirate },
                { "scottish", Language::Scottish },
            }};
            // clang-format on

            for (auto& lang : languages)
            {
                auto& map = dllMap[lang.second] = {};
                map.emplace_back(LoadLibraryA(std::format("langs/resources-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/infocards-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/misctext-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/nameresources-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/equipresources-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/offerbriberesources-{}.dll", lang.first).c_str()));
                map.emplace_back(LoadLibraryA(std::format("langs/misctextinfo2-{}.dll", lang.first).c_str()));

                assert(std::ranges::all_of(map, [](auto dll) { return dll != nullptr; }));
            }
        }

        void Init() override
        {
            dllMap[currentLang] = {
                GetModuleHandleA("resources.dll"),     GetModuleHandleA("infocards.dll"),      GetModuleHandleA("misctext.dll"),
                GetModuleHandleA("nameresources.dll"), GetModuleHandleA("equipresources.dll"), GetModuleHandleA("offerbriberesources.dll"),
                GetModuleHandleA("misctextinfo2.dll"),
            };

            getIdsNameDetour = std::make_unique<FunctionDetour<GetIdsName>>(reinterpret_cast<GetIdsName>(0x4347e0));
            getIdsNameDetour->Detour(GetIdsNameOverride);

            MemUtils::PatchAssembly(0x57DB1B, GetIdsInfocardNaked);
        }

    public:
        static void SetLanguage(const Language lang) { currentLang = lang; }

        static int LoadCustomIdsName(const uint ids, wchar_t* buffer, int length)
        {
            const auto [module, index] = GetModuleByIds(ids);
            if (!module)
            {
                return 0;
            }

            return LoadStringW(module, ids, buffer, length);
        }

        explicit SprechenSieDeutsch(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) { LoadLibraries(); }
};

std::string ChaosMod::GetInfocardName(const uint ids)
{
    static std::wstring wStr(65525, L'\0');

    int size = SprechenSieDeutsch::LoadCustomIdsName(ids, wStr.data(), static_cast<int>(wStr.capacity()));
    wStr.reserve(size);

    auto str = StringUtils::wstos(wStr);
    str.erase(std::ranges::find(str, '\0'), str.end());
    return str;
}

void ChaosMod::SetLanguage(Language lang) { SprechenSieDeutsch::SetLanguage(lang); }

constexpr DWORD NakedReturn = 0x57DB25;
__declspec(naked) void SprechenSieDeutsch::GetIdsInfocardNaked()
{
    __asm {
        push eax // XMLReader VTable or something..?
        push [esp+0x18] // ids number
        call GetIdsInfocardOverride
        test eax, eax
        jz normalOperation

        mov edi, eax // Replace their buffer with our own
        pop eax // Restore old xml reader
        mov ebx, [esp+0x10+0x8] // RDL
        mov ecx, infocardLength
        jmp done

        normalOperation:
        pop eax // Restore old xml reader
        mov ebx, [esp+0x10+0x8] // RDL
        lea ecx, [esi+esi]

        done: // Default operation
        push ecx
        push edi
        push ebx
        jmp NakedReturn
    }
}

// clang-format off
SetupEffect(SprechenSieDeutsch, {
    .effectName = "Sprechen Sie Deutsch?",
    .description = "Spin the wheel of languages, who knows what your game will become?",
    .category = EffectType::Interface,
    .timingModifier = 0.0f,
    .isTimed = false,
    .weight = 12
});