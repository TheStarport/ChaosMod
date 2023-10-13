#include "PCH.hpp"

#include "HudInterface.hpp"

#include "HudComponents/TStatic.hpp"
#include "HudComponents/TText.hpp"

constexpr DWORD TextControlVTableAddress = 0x5e476c;
constexpr DWORD TextControl2VTableAddress = 0x5e4f24;

void HudInterface::FlipTextControl(TControl* control)
{
    auto q = control->GetName();
    if (const auto address = *reinterpret_cast<PDWORD>(control); address != TextControlVTableAddress && address != TextControl2VTableAddress)
    {
        return;
    }

    const auto text = static_cast<TText*>(control); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    const auto el = i()->flippedText.find(text);
    if (el != i()->flippedText.end())
    {
        text->SetTextValue(el->second.c_str());
        return;
    }

    auto str = text->GetTextValue();
    const std::wstring rev = { str.rbegin(), str.rend() };

    if (!str.empty() && !rev.empty())
    {

        i()->flippedText[text] = rev;
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
    if (const auto address = *reinterpret_cast<PDWORD>(control); address != TextControlVTableAddress && address != TextControl2VTableAddress)
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
                continue;
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
        sender->ForEachControl(FlipTextControl);
    }
    else if (!flippedText.empty())
    {
        for (auto it = flippedText.cbegin(); it != flippedText.cend();)
        {
            if (sender->ControlExists(it->first))
            {
                const std::wstring rev = { it->second.rbegin(), it->second.rend() };
                it->first->SetTextValue(rev.c_str());
                it = flippedText.erase(it);
                continue;
            }

            ++it;
        }
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
