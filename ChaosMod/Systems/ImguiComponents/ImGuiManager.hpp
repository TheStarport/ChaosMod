#pragma once

enum class Font
{
    TitiliumWeb,
    TitiliumWebLarge,
    TitiliumWebBold,
    TitiliumWebBoldLarge,
};

class ActiveEffect;
enum class EffectType;
struct ImFont;
class ImGuiManager
{
        inline static std::map<EffectType, std::vector<ActiveEffect*>> allEffects;
        inline static std::map<Font, ImFont*> loadedFonts;

        static void SetupImGuiStyle();

    public:
        static void Init();
        ImGuiManager() = delete;

        static void SetProgressBarPercentage(float percentage);
        static void SetVotingChoices(const std::vector<std::string>& choices);
        static void ShowConfigurator();
        static void ShowEffectSelector();
        static void ShowDebugConsole();
        static void ShowPatchNotes();
        static void StartCredits();
        static void StopCredits();
        static void ToggleBoxOfChocolates(bool state);
        static void Render();

        static ImFont* GetFont(Font font);
};
