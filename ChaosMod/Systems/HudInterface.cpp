#include "PCH.hpp"

#include "HudInterface.hpp"

#include "HudComponents/TStatic.hpp"
#include "HudComponents/TText.hpp"

std::vector<std::string> textElementList = {
    "BtnAcceptText",
    "BtnDeclineText",
    "BtnLeaveText",
    "AttentionTitle",
    "LoadingProgressText",
    "WeaponNumberText0",
    "WeaponText0",
    "AmmoText0",
    "WeaponNumberText1",
    "WeaponText1",
    "AmmoText1",
    "WeaponNumberText2",
    "WeaponText2",
    "AmmoText2",
    "WeaponNumberText3",
    "WeaponText3",
    "AmmoText3",
    "WeaponNumberText4",
    "WeaponText4",
    "AmmoText4",
    "WeaponNumberText5",
    "WeaponText5",
    "AmmoText5",
    "WeaponNumberText6",
    "WeaponText6",
    "AmmoText6",
    "WeaponNumberText7",
    "WeaponText7",
    "AmmoText7",
    "WeaponNumberText8",
    "WeaponText8",
    "AmmoText8",
    "WeaponNumberText9",
    "WeaponText9",
    "AmmoText9",
    "RepairCountText",
    "BatteryCountText",
    "SpecialModeText",
    "SpecialModeTextCancel",
    "TargetShipName",
    "SubtargetName",
    "TargetRankText",
    "VelocityText",
    "ThrustText",
    "FormationTitle",
    "ContactListRangeText0",
    "ContactListNameText0",
    "ContactListRangeText1",
    "ContactListNameText1",
    "ContactListRangeText2",
    "ContactListNameText2",
    "ContactListRangeText3",
    "ContactListNameText3",
    "ContactListRangeText4",
    "ContactListNameText4",
    "ContactListRangeText5",
    "ContactListNameText5",
    "ContactListRangeText6",
    "ContactListNameText6",
    "ContactListRangeText7",
    "ContactListNameText7",
    "CruiseProgressText",
    "ReticleRangeText",
    "PauseTitle",
    "PauseButtonText0",
    "PauseButtonText1",
    "PauseButtonText2",
    "PauseButtonText3",
};

void HudInterface::FlipUi(TControl* control)
{
    FlipTextControl(control);

    auto child = control->GetChildControl();
    while (child)
    {
        FlipTextControl(child);
        auto childsChild = child->GetChildControl();

        while (childsChild)
        {
            FlipTextControl(childsChild);
            childsChild = childsChild->GetNextControl();
        }

        child = child->GetNextControl();
    }
}

void HudInterface::FlipTextControl(TControl* control)
{
    if (const auto name = control->GetName(); std::ranges::find(textElementList, name) == textElementList.end())
    {
        return;
    }

    auto q = control->GetName();
    const auto text = static_cast<TText*>(control); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    const auto el = flippedText.find(text);
    if (el != flippedText.end())
    {
        text->SetTextValue(el->second.c_str());
        return;
    }

    auto str = text->GetTextValue();
    const std::wstring rev = { str.rbegin(), str.rend() };

    if (!str.empty() && !rev.empty())
    {

        flippedText[text] = rev;
        text->SetTextValue(rev.c_str());
    }
}

void HudInterface::OverrideColors(TControl* sender)
{
    ReplaceColorOnControl(sender);
    auto child = sender->GetChildControl();
    while (child)
    {
        ReplaceColorOnControl(child);
        auto childsChild = child->GetChildControl();

        while (childsChild)
        {
            ReplaceColorOnControl(childsChild);
            childsChild = childsChild->GetNextControl();
        }

        child = child->GetNextControl();
    }
}

void HudInterface::ReplaceColorOnControl(TControl* control)
{
    if (const auto name = control->GetName(); std::ranges::find(textElementList, name) == textElementList.end())
    {
        return;
    }

    byte r = (currentColor >> 16) & 0xff;
    byte g = (currentColor >> 8) & 0xff;
    byte b = (currentColor)&0xff;

    const auto text = static_cast<TText*>(control); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    if (const auto el = replacedColors.find(text); el != replacedColors.end())
    {
        const Color col = { r, g, b, 0xFF };
        text->SetColor(col);
        text->SetColourHighlighted(col);
        text->SetInteractableColourValue(col);
        text->SetInteractableColourValue(col);
        return;
    }

    replacedColors[text] = { text->GetColor(), text->GetColorHighlighted(), text->GetInteractableColorValue(), text->GetInteractableColorUse() };
    const Color col = { r, g, b, 0xFF };
    text->SetColor(col);
    text->SetColourHighlighted(col);
    text->SetInteractableColourValue(col);
    text->SetInteractableColourValue(col);
}

HudInterface::HudInterface()
{
    const HINSTANCE dll = LoadLibraryA("Hud.dll");
    if (dll == nullptr)
    {
        MessageBoxA(nullptr, "HUD Loading Error", "Unable to load hud.dll - this is required for hud manipulation.", MB_ICONERROR);
        throw std::runtime_error("Failure to load Hud.dll");
    }

    const auto hudInterface = reinterpret_cast<RemoteCallType>(GetProcAddress(dll, "_IHud"));
    if (hudInterface == nullptr)
    {
        MessageBoxA(nullptr, "HUD Loading Error", "Unable to load hud.dll - this is required for hud manipulation.", MB_ICONERROR);
        throw std::runtime_error("Failure to load Hud.dll");
    }

    hudInterface(facility);
    facility->AddHud(this);
}

void HudInterface::Draw(TControl* sender) {}

void HudInterface::Update(TControl* sender)
{
    if (shouldMakeInterfaceBuggy)
    {
        if (!buggyInterface.has_value())
        {
            buggyInterface = std::unordered_map<TControl*, BuggyInterface>();
        }
        auto& map = buggyInterface.value();

        if (!map.contains(sender))
        {
            sender->ForEachControl([&map](auto control) { map[control].originalPosition = control->GetPosition(); });
        }

        for (auto& el : buggyInterface.value())
        {
            auto& bug = el.second;
            if (sender->ControlExists(el.first))
            {
                Vector pos = el.first->GetPosition();
                pos.x += bug.goingRight ? 0.001f : -0.001f;
                pos.y += bug.goingDown ? 0.001f : -0.001f;
                el.first->SetPosition(pos);

                if (pos.x > 0.5f)
                {
                    bug.goingRight = false;
                }
                else if (pos.x < -0.5f)
                {
                    bug.goingRight = true;
                }

                if (pos.y > 0.5f)
                {
                    bug.goingDown = false;
                }
                else if (pos.y < -0.5f)
                {
                    bug.goingDown = true;
                }
            }
        }
    }
    else if (buggyInterface.has_value())
    {
        auto& map = buggyInterface.value();

        for (auto it = map.cbegin(); it != map.cend();)
        {
            if (sender->ControlExists(it->first))
            {
                it->first->SetPosition(it->second.originalPosition);
                it = map.erase(it);
            }

            ++it;
        }

        if (map.empty())
        {
            buggyInterface = std::nullopt;
        }
    }

    if (shouldFlip)
    {
        FlipUi(sender);
    }
    else if (!flippedText.empty())
    {
        for (auto& text : flippedText)
        {
            if (!sender->ControlExists(text.first))
            {
                continue;
            }

            const std::wstring rev = { text.second.rbegin(), text.second.rend() };
            text.first->SetTextValue(rev.c_str());
        }

        flippedText.clear();
    }

    if (currentColor)
    {
        OverrideColors(sender);
    }
    else if (!replacedColors.empty())
    {
        for (auto& element : replacedColors)
        {
            if (!sender->ControlExists(element.first))
            {
                continue;
            }

            element.first->SetColor(element.second.originalColor);
            element.first->SetColourHighlighted(element.second.highlightedColor);
            element.first->SetInteractableColourValue(element.second.interactiveColor1);
            element.first->SetInteractableColorUse(element.second.interactiveColor2);
        }

        replacedColors.clear();
    }
}
void HudInterface::Execute(TControl* sender) {}
void HudInterface::Terminate(TControl* sender) {}
void HudInterface::ToggleFlipping() { shouldFlip = !shouldFlip; }
void HudInterface::SetUiColor(const DWORD newColor) { currentColor = newColor; }
void HudInterface::SetBuggyInterface(const bool buggy) { shouldMakeInterfaceBuggy = buggy; }
