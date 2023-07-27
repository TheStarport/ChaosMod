#pragma once
#include "Structures.hpp"

class TText : public TControl
{
        byte data[928]; // 0x3A0 in sub_58C120

    public:
        Color GetColor()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x348;
            return *reinterpret_cast<Color *>(state);
        }

        void SetColor(Color C)
        {
            const uint state = reinterpret_cast<uint>(this) + 0x348;
            *reinterpret_cast<Color *>(state) = C;
        }

        bool GetInteractable()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x35C;
            const bool value = *reinterpret_cast<bool *>(state);
            return value;
        }

        void SetInteractable(bool Value)
        {
            const uint data = reinterpret_cast<uint>(this) + 0x35C;
            *reinterpret_cast<bool *>(data) = Value;
        }

        Color GetInteractableColorUse()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x35D;
            const Color value = *reinterpret_cast<Color *>(state);
            return value;
        }

        void SetInteractableColorUse(const Color value)
        {
            const uint state = reinterpret_cast<uint>(this) + 0x35D;
            *reinterpret_cast<Color *>(state) = value;
        }

        Color GetColorHighlighted()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x354;
            const Color value = *reinterpret_cast<Color *>(state);
            return value;
        }

        void SetColourHighlighted(const Color value)
        {
            const uint state = reinterpret_cast<uint>(this) + 0x354;
            *reinterpret_cast<Color *>(state) = value;
        }

        Color GetInteractableColorValue()
        {
            const uint state = reinterpret_cast<uint>(this) + 0x35E;
            const Color value = *reinterpret_cast<Color *>(state);
            return value;
        }

        void SetInteractableColourValue(const Color c)
        {
            const uint state = reinterpret_cast<uint>(this) + 0x35E;
            *reinterpret_cast<Color *>(state) = c;
        }

        // bool HasText() { return static_cast<bool>(Perform(0x21, 0, 0)); }

        void SetTextValue(const wchar_t *message) { Perform(0x1D, uint(message), 0); }
        std::wstring GetTextValue()
        {
            // if (HasText())
            {
                auto test = reinterpret_cast<PWCHAR>(Perform(0x1F, 0, 0));
                if (!test)
                {
                    return L"";
                }

                return { test };
            }

            return L"";
        }

        virtual ~TText() = default;
};

using TTextFunction = TText *(__thiscall *)(const TControl *parent, LPCSTR description, LPCSTR font, const Vector &position, LPCSTR hardpoint, float sizeX,
                                            float sizeY, float sizeZ, uint resStrId, int align, uint tip, bool center);

static TTextFunction CreateTextControl = reinterpret_cast<TTextFunction>(0x5A0E30);
