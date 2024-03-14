#pragma once
#include "TText.hpp"

class TTextEdit : public TText
{
        byte data[1000];
};

typedef TTextEdit*(__thiscall* TTextEditFunction)(const TControl* parent, LPCSTR description, const Vector& position, float unknown2, float unknown3,
                                                  wchar_t* defaultText);
typedef void(__thiscall* TTextEditSetupFunction)(TTextEdit* control);

#define DefaultTextEditValues 0.52, 0.03
#define MaxCharacterCount     32
static TTextEditFunction CreateTextEditControl = reinterpret_cast<TTextEditFunction>(0x5A1900);
static TTextEditSetupFunction SetupTextEditControl = reinterpret_cast<TTextEditSetupFunction>(0x57C750);
