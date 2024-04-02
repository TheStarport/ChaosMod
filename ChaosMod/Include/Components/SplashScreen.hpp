#pragma once

#define min std::min
#define max std::max

#include "Unknwn.h"
#include <gdiplus.h>

#undef min
#undef max

class SplashScreen final : public Component
{
        HANDLE event;
        HANDLE thread;
        DWORD threadId{};
        Gdiplus::Bitmap* image{};
        HWND splashWindow{};
        HWND progressWindow{};
        HWND parentWindow;

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static DWORD __stdcall ThreadProc(void* lpParameter);
        static Gdiplus::Bitmap* LoadImageFromResource(const wchar_t* resId, const wchar_t* type);

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;

    public:
        explicit SplashScreen(HWND parentWindow);
        ~SplashScreen() override;
        SplashScreen(const SplashScreen& other) = delete;
        void SetLoadingMessage(int progress);
};
