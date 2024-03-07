#include "PCH.hpp"

#include "ActiveEffectsText.hpp"
#include "ChaosOptionText.hpp"
#include "Configurator.hpp"
#include "EffectSelector.hpp"
#include "ProgressBar.hpp"
#include "ScrollingCredits.hpp"

#include "ImGuiManager.hpp"

#include "../ImGuiHelpers/ImGuiDX9.hpp"
#include "../ImGuiHelpers/ImGuiWin32.hpp"

#include "Debug.hpp"
#include "PatchNotesWindow.hpp"
#include "imgui/imgui_internal.h"

void ImGuiManager::SetupImGuiStyle()
{
    constexpr auto bgColor = ImVec4(0.0784f, 0.0431f, 0.2235f, 0.7f);      // #140B39
    constexpr auto bgColorChild = ImVec4(0.0784f, 0.0431f, 0.2235f, 0.0f); // #140B39

    constexpr auto titleBgColor = ImVec4(0.00784314, 0.105882, 0.188235, 1); // #021B30
    constexpr auto borderColor = ImVec4(0.50f, 0.72f, 0.83f, 1.0f);
    constexpr auto textColor = ImVec4(0.529f, 0.764f, 0.878f, 1.0f);
    constexpr auto checkBoxCheckColor = ImVec4(0.16f, 0.59f, 0.16f, 1.0f);        // #28962B
    constexpr auto scrollBarColor = ImVec4(0.411f, 0.596f, 0.678f, 1.0f);         // #6998AE
    constexpr auto disabledColor = ImVec4(0.451f, 0.451f, 0.463f, 1.0f);          // #737276
    constexpr auto borderShadowColor = ImVec4(0.4549f, 0.6235f, 0.7569f, 1.0f);   // #749FC1
    constexpr auto frameBg = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);                      // Schwarz
    constexpr auto buttonNormalColor = ImVec4(0.00784314, 0.105882, 0.188235, 1); // #021B30
    constexpr auto buttonHoverColor = ImVec4(0.52f, 0.45f, 0.0f, 1.0f);           // Hex #857400
    constexpr auto comboHoverColor = ImVec4(0.52f, 0.45f, 0.0f, 1.0f);            // Hex #857400

    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.f, 4.0f);
    style.WindowMinSize = ImVec2(20.0f, 20.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.SeparatorTextAlign = ImVec2(.5f, .5);
    style.SeparatorTextPadding = ImVec2(20.f, 3.f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(20.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
    style.IndentSpacing = 20.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 20.0f;
    style.GrabMinSize = 16.0f;
    style.TabBorderSize = 1.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
    style.WindowBorderSize = 2.0f;
    style.FramePadding = ImVec2(6.f, 2.0f);
    style.CellPadding = ImVec2(3.f, 2.0f);

    style.WindowRounding = 5.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 6.0f;
    style.TabRounding = 5.0f;

    style.Colors[ImGuiCol_Text] = textColor;
    style.Colors[ImGuiCol_TextDisabled] = disabledColor;
    style.Colors[ImGuiCol_WindowBg] = bgColor;
    style.Colors[ImGuiCol_ChildBg] = bgColorChild;
    style.Colors[ImGuiCol_PopupBg] = bgColor;
    style.Colors[ImGuiCol_Border] = borderColor;
    style.Colors[ImGuiCol_BorderShadow] = borderShadowColor;
    style.Colors[ImGuiCol_FrameBg] = frameBg;
    style.Colors[ImGuiCol_FrameBgHovered] = frameBg;
    style.Colors[ImGuiCol_FrameBgActive] = frameBg;
    style.Colors[ImGuiCol_TitleBg] = titleBgColor;
    style.Colors[ImGuiCol_TitleBgActive] = titleBgColor;
    style.Colors[ImGuiCol_TitleBgCollapsed] = titleBgColor;

    style.Colors[ImGuiCol_MenuBarBg] = bgColor;
    style.Colors[ImGuiCol_ScrollbarBg] = bgColor;
    style.Colors[ImGuiCol_ScrollbarGrab] = scrollBarColor;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = comboHoverColor;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = comboHoverColor;
    style.Colors[ImGuiCol_CheckMark] = checkBoxCheckColor;
    style.Colors[ImGuiCol_SliderGrab] = scrollBarColor;
    style.Colors[ImGuiCol_SliderGrabActive] = comboHoverColor;
    style.Colors[ImGuiCol_Button] = buttonNormalColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoverColor;
    style.Colors[ImGuiCol_ButtonActive] = buttonHoverColor;
    style.Colors[ImGuiCol_Header] = bgColor;
    style.Colors[ImGuiCol_HeaderHovered] = comboHoverColor;
    style.Colors[ImGuiCol_HeaderActive] = comboHoverColor;
    style.Colors[ImGuiCol_Separator] = scrollBarColor;
    style.Colors[ImGuiCol_SeparatorHovered] = scrollBarColor;
    style.Colors[ImGuiCol_SeparatorActive] = scrollBarColor;
    style.Colors[ImGuiCol_ResizeGrip] = bgColor;
    style.Colors[ImGuiCol_ResizeGripHovered] = titleBgColor;
    style.Colors[ImGuiCol_ResizeGripActive] = bgColor;
    style.Colors[ImGuiCol_Tab] = titleBgColor;                                                       // Normaler Tab-Hintergrund
    style.Colors[ImGuiCol_TabHovered] = buttonHoverColor;                                            // Tab-Hintergrund, wenn die Maus darüber schwebt
    style.Colors[ImGuiCol_TabActive] = ImVec4(titleBgColor.x, titleBgColor.y, titleBgColor.z, 1.0f); // Aktiver Tab-Hintergrund, leicht geänderte Farbe
    style.Colors[ImGuiCol_TabUnfocused] = titleBgColor;                                              // Unfokussierter Tab-Hintergrund
    style.Colors[ImGuiCol_TabUnfocusedActive] = titleBgColor;                                        // Aktiver unfokussierter Tab-Hintergrund

    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = titleBgColor;
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}

void ImGuiManager::Init()
{
    for (const auto effects = ActiveEffect::GetAllEffects(); const auto& effect : effects)
    {
        auto& info = effect->GetEffectInfo();
        if (!allEffects.contains(info.category))
        {
            allEffects[info.category] = {};
        }

        allEffects[info.category].emplace_back(effect);
    }

    EffectSelector::allEffects = &allEffects;
    Configurator::allEffects = &allEffects;
    ScrollingCredits::Init();

    const auto& io = ImGui::GetIO();
    const auto font = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb.ttf", 28);
    ASSERT(font, "Unable to load Chaos fonts!!");

    loadedFonts[Font::TitiliumWeb] = font;
    loadedFonts[Font::TitiliumWebLarge] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb.ttf", 42);
    loadedFonts[Font::TitiliumWebBold] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb-Bold.ttf", 28);
    loadedFonts[Font::TitiliumWebBoldLarge] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb-Bold.ttf", 42);

    io.Fonts->Build();

    SetupImGuiStyle();
}

void ImGuiManager::SetProgressBarPercentage(float percentage) { ProgressBar::progress = percentage; }
void ImGuiManager::SetVotingChoices(const std::vector<std::string>& choices) { ChaosOptionText::options = choices; }
void ImGuiManager::ShowConfigurator() { Configurator::show = true; }
void ImGuiManager::ShowEffectSelector() { EffectSelector::show = true; }
void ImGuiManager::ShowDebugConsole() { DebugMenu::show = true; }
void ImGuiManager::ShowPatchNotes() { PatchNotesWindow::show = true; }
void ImGuiManager::StartCredits()
{
    ScrollingCredits::scrollAmount = 0.0f;
    ScrollingCredits::show = true;
}

void ImGuiManager::StopCredits() { ScrollingCredits::show = false; }
void ImGuiManager::ToggleBoxOfChocolates(const bool state) { ActiveEffectsText::boxOfChocolates = state; }

void ImGuiManager::Render()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::SetShortcutRouting(ImGuiMod_Ctrl | ImGuiKey_Tab, ImGuiKeyOwner_None);
    ImGui::SetShortcutRouting(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Tab, ImGuiKeyOwner_None);
    ImGui::SetShortcutRouting(ImGuiKey_Tab, ImGuiKeyOwner_None);
    ImGui::NewFrame();

    ImGui::PushFont(loadedFonts.begin()->second);

    ProgressBar::Render();
    Configurator::Render();
    EffectSelector::Render();
    ScrollingCredits::Render();
    ChaosOptionText::Render();
    ActiveEffectsText::Render();
    DebugMenu::Render();
    PatchNotesWindow::Render();

#ifdef _DEBUG
    ImGui::ShowDemoWindow();
#endif

    ImGui::PopFont();

    ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

ImFont* ImGuiManager::GetFont(const Font font)
{
    if (loadedFonts.empty())
    {
        // Return default if failed to load
        return ImGui::GetFont();
    }

    const auto iter = loadedFonts.find(font);
    if (iter == loadedFonts.end())
    {
        return nullptr;
    }

    return iter->second;
}
