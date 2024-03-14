#pragma once

enum class Font
{
    TitiliumWeb,
    TitiliumWebLarge,
    TitiliumWebExtraLarge,
    TitiliumWebBold,
    TitiliumWebBoldLarge,
};

class ActiveEffect;
enum class EffectType;
struct ImFont;
class Configurator;
class ImGuiManager
{
        inline static std::map<EffectType, std::vector<ActiveEffect*>> allEffects;
        inline static std::map<Font, ImFont*> loadedFonts;

        static void SetupImGuiStyle();
        inline static Configurator* configurator;
        inline static Configurator* configImporter;

    public:
        static void Init();
        ImGuiManager() = delete;

        static void SetProgressBarPercentage(float percentage);
        static void SetVotingChoices(const std::vector<std::string>& choices);
        static void ShowConfigurator();
        static void ShowEffectSelector();
        static void ShowDebugConsole();
        static void ShowPatchNotes();
        static void ShowEffectHistory();
        static void AddToEffectHistory(std::string_view effectName, std::string_view description);
        static void StartCredits();
        static void StopCredits();
        static void ShowCargoSpawner();
        static void ToggleBoxOfChocolates(bool state);
        static void ToggleSelectionWheel();
        static void ImportConfig(const std::string& path);
        static void Render();

        static ImFont* GetFont(Font font);
};
