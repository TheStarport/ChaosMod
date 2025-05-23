#pragma once

#include "VanillaHudComponents/Interfaces.hpp"
#include "VanillaHudComponents/TText.hpp"

#define HudInterfaceId   "ec238653-37e7-4975-b328-5aec0817bba9"
#define PointerHandlerId "8ab0bab3-bb4a-439f-b5e3-6dec724f0dc4"

class __declspec(uuid(PointerHandlerId)) PointerHandler
{
    public:
        class HudInterface* hud;
};

class __declspec(uuid(HudInterfaceId)) HudInterface final : public IHud, public Component
{
        IHudFacility* facility = nullptr;
        using RemoteCallType = HRESULT(__stdcall*)(IHudFacility*& hudFacility);

        DWORD currentColor = 0x0;

        bool shouldFlip = false;
        std::map<TText*, std::wstring> flippedText;

        static void FlipTextControl(TControl*);

        struct ElementColors
        {
                Color originalColor;
                Color highlightedColor;
                Color interactiveColor1;
                Color interactiveColor2;
        };

        std::map<TText*, ElementColors> replacedColors;
        void OverrideColors(TControl* sender);
        void ReplaceColorOnControl(TControl* control);

        bool shouldMakeInterfaceBuggy = false;
        struct BuggyInterface
        {
                Vector originalPosition{};
                bool goingRight = false;
                bool goingDown = false;

                BuggyInterface()
                {
                    goingRight = Get<Random>()->Uniform(0u, 1u);
                    goingDown = Get<Random>()->Uniform(0u, 1u);
                }
        };
        std::optional<std::unordered_map<TControl*, BuggyInterface>> buggyInterface;

    protected:
        int refs = 1;
        HRESULT result = NOERROR;

    public:
        HRESULT __stdcall QueryInterface(const IID& rid, LPVOID* ppvObj) override
        {
            // Always set out parameter to NULL, validating it first.
            if (!ppvObj)
            {
                return E_INVALIDARG;
            }
            *ppvObj = nullptr;
            if (rid == __uuidof(IHud) || rid == __uuidof(PointerHandler))
            {
                // Increment the reference count and return the pointer.
                *ppvObj = static_cast<LPVOID>(this);
                result = NOERROR;
                _AddRef();
                return NOERROR;
            }

            return E_NOINTERFACE;
        }

        int __stdcall _AddRef() override
        {
            result = ++refs;
            return -1;
        }

        int __stdcall _Release() override
        {
            result = --refs;
            return -1;
        }

        HudInterface();
        ~HudInterface() override { FreeLibrary(GetModuleHandleA("Hud.dll")); }

        void __stdcall Draw(TControl* sender) override;
        void __stdcall Update(TControl* sender) override;
        void __stdcall Execute(TControl* sender) override;
        void __stdcall Terminate(TControl* sender) override;

        void ToggleFlipping();
        void SetUiColor(DWORD newColor);
        void SetBuggyInterface(bool buggy);
};

#undef HudInterfaceId
#undef PointerHandlerId
