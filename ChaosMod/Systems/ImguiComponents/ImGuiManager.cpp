#include "PCH.hpp"

#include "ActiveEffectsText.hpp"
#include "ChaosOptionText.hpp"
#include "Configurator.hpp"
#include "EffectSelector.hpp"
#include "EffectToggler.hpp"
#include "ProgressBar.hpp"
#include "ScrollingCredits.hpp"

#include "ImGuiManager.hpp"

#include "../ImGuiHelpers/ImGuiDX9.hpp"
#include "../ImGuiHelpers/ImGuiWin32.hpp"

#include "Debug.hpp"
#include "PatchNotesWindow.hpp"
#include "imgui/imgui_internal.h"

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
    EffectToggler::allEffects = &allEffects;
    ScrollingCredits::Init();

    const auto& io = ImGui::GetIO();
    const auto font = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb.ttf", 28);
    ASSERT(font, "Unable to load Chaos fonts!!");

    loadedFonts[Font::TitiliumWeb] = font;
    loadedFonts[Font::TitiliumWebLarge] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb.ttf", 42);
    loadedFonts[Font::TitiliumWebBold] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb-Bold.ttf", 28);
    loadedFonts[Font::TitiliumWebBoldLarge] = io.Fonts->AddFontFromFileTTF("../DATA/CHAOS/FONTS/TitilliumWeb-Bold.ttf", 42);

    io.Fonts->Build();
}

void ImGuiManager::SetProgressBarPercentage(float percentage) { ProgressBar::progress = percentage; }
void ImGuiManager::SetVotingChoices(const std::vector<std::string>& choices) { ChaosOptionText::options = choices; }
void ImGuiManager::ShowConfigurator() { Configurator::show = true; }
void ImGuiManager::ShowEffectSelector() { EffectSelector::show = true; }
void ImGuiManager::ShowEffectToggler() { EffectToggler::show = true; }
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

    Configurator::Render();
    EffectToggler::Render();
    EffectSelector::Render();
    ProgressBar::Render();
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
