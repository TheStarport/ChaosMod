#include "PCH.hpp"

#include "Components/SplashScreen.hpp"

#include "../../resource.h"

#include <commctrl.h>

#pragma comment(lib, "gdiplus.lib")

LRESULT SplashScreen::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto* pInstance = reinterpret_cast<SplashScreen*>(GetWindowLongPtr(hwnd, GWL_USERDATA));
    if (pInstance == nullptr)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        case WM_PAINT:
            {
                if (pInstance->image)
                {
                    Gdiplus::Graphics gdip(hwnd);
                    gdip.DrawImage(pInstance->image, 0, 0, pInstance->image->GetWidth(), pInstance->image->GetHeight());
                }
                ValidateRect(hwnd, nullptr);
                return 0;
            }
            break;
        case PBM_SETPOS:
            {
                // Обновление progress bar
                if (!IsWindow(pInstance->progressWindow)) // Создание, если ещё не создан
                {
                    RECT client;
                    GetClientRect(hwnd, &client);
                    pInstance->progressWindow = CreateWindow(
                        PROGRESS_CLASS, nullptr, WS_CHILD | WS_VISIBLE, 4, client.bottom - 20, client.right - 8, 16, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
                    SendMessage(pInstance->progressWindow, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
                }
                SendMessage(pInstance->progressWindow, PBM_SETPOS, wParam, 0);
                return 0;
            }
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD __stdcall SplashScreen::ThreadProc(void* lpParameter)
{
    auto* cls = static_cast<SplashScreen*>(lpParameter);
    if (cls->image == nullptr)
    {
        return 0;
    }

    // Register your unique class name
    WNDCLASS wndcls = { 0 };

    wndcls.style = CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = WndProc;
    wndcls.hInstance = GetModuleHandle(nullptr);
    wndcls.hCursor = LoadCursor(nullptr, IDC_APPSTARTING);
    wndcls.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wndcls.lpszClassName = L"SplashWnd";
    wndcls.hIcon = LoadIcon(wndcls.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClass(&wndcls))
    {
        if (GetLastError() != 0x00000582) // already registered)
        {
            OutputDebugString(L"Unable to register class SplashWnd\n");
            return 0;
        }
    }

    // try to find monitor where mouse was last time
    POINT point = { 0 };
    MONITORINFO mi = { sizeof(MONITORINFO), 0 };
    HMONITOR hMonitor = nullptr;
    RECT rcArea = { 0 };

    ::GetCursorPos(&point);
    hMonitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    if (::GetMonitorInfo(hMonitor, &mi))
    {
        rcArea.left = (mi.rcMonitor.right + mi.rcMonitor.left - static_cast<long>(cls->image->GetWidth())) / 2;
        rcArea.top = (mi.rcMonitor.top + mi.rcMonitor.bottom - static_cast<long>(cls->image->GetHeight())) / 2;
    }
    else
    {
        SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
        rcArea.left = static_cast<long>(rcArea.right + rcArea.left - cls->image->GetWidth()) / 2;
        rcArea.top = static_cast<long>(rcArea.top + rcArea.bottom - cls->image->GetHeight()) / 2;
    }

    const static std::wstring windowName = L"Chaos Mod Loader";
    cls->splashWindow = CreateWindowEx(!windowName.empty() ? 0 : WS_EX_TOOLWINDOW,
                                       L"SplashWnd",
                                       windowName.c_str(),
                                       WS_CLIPCHILDREN | WS_POPUP,
                                       rcArea.left,
                                       rcArea.top,
                                       cls->image->GetWidth(),
                                       cls->image->GetHeight(),
                                       cls->parentWindow,
                                       nullptr,
                                       wndcls.hInstance,
                                       nullptr);
    if (!cls->splashWindow)
    {
        OutputDebugString(L"Unable to create SplashWnd\n");
        return 0;
    }

    SetWindowLongPtr(cls->splashWindow, GWL_USERDATA, reinterpret_cast<LONG_PTR>(cls));
    ShowWindow(cls->splashWindow, SW_SHOWNOACTIVATE);

    MSG msg;
    BOOL bRet;

    PeekMessage(&msg, nullptr, 0, 0, 0); // invoke creating message queue
    SetEvent(cls->event);

    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (msg.message == WM_QUIT)
        {
            break;
        }
        if (msg.message == PBM_SETPOS)
        {
            SendMessage(cls->splashWindow, PBM_SETPOS, msg.wParam, msg.lParam);
            continue;
        }
        if (msg.message == PBM_SETSTEP)
        {
            SendMessage(cls->splashWindow, PBM_SETPOS, LOWORD(msg.wParam), 0); // initiate progress bar creation
            SendMessage(cls->progressWindow, PBM_SETSTEP, (HIWORD(msg.wParam) - LOWORD(msg.wParam)) / msg.lParam, 0);
            continue;
        }
        if (msg.message == PBM_SETBARCOLOR)
        {
            if (!IsWindow(cls->progressWindow))
            {
                SendMessage(cls->splashWindow, PBM_SETPOS, 0, 0); // initiate progress bar creation
            }
            SendMessage(cls->progressWindow, PBM_SETBARCOLOR, msg.wParam, msg.lParam);
            continue;
        }

        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    DestroyWindow(cls->splashWindow);

    return 0;
}

Gdiplus::Bitmap* SplashScreen::LoadImageFromResource(const wchar_t* resId, const wchar_t* type)
{
    IStream* pStream = nullptr;
    Gdiplus::Bitmap* pBmp = nullptr;
    HGLOBAL hGlobal = nullptr;

    const HMODULE mod = GetModuleHandleA("ChaosMod.dll");

    if (HRSRC src = FindResourceW(mod, resId, type))
    {
        if (const DWORD resSize = SizeofResource(mod, src); resSize > 0)
        {
            if (const HGLOBAL globalResource = LoadResource(mod, src))
            {
                const void* imageBytes = LockResource(globalResource); // get a pointer to the file bytes

                // copy image bytes into a real hglobal memory handle
                hGlobal = ::GlobalAlloc(GHND, resSize);
                if (hGlobal)
                {
                    if (void* buffer = ::GlobalLock(hGlobal))
                    {
                        memcpy(buffer, imageBytes, resSize);
                        if (const HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream); SUCCEEDED(hr))
                        {
                            // pStream now owns the global handle and will invoke GlobalFree on release
                            hGlobal = nullptr;
                            pBmp = new Gdiplus::Bitmap(pStream);
                        }
                    }
                }
            }
        }
    }

    if (pStream)
    {
        pStream->Release();
        pStream = nullptr;
    }

    if (hGlobal)
    {
        GlobalFree(hGlobal);
        hGlobal = nullptr;
    }

    return pBmp;
}

SplashScreen::SplashScreen(HWND parentWindow) : parentWindow(parentWindow)
{
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    image = LoadImageFromResource(MAKEINTRESOURCE(IDI_CHAOS_LOGO), L"PNG");

    event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    thread = CreateThread(nullptr, 0, ThreadProc, static_cast<LPVOID>(this), 0, &threadId);
    if (WaitForSingleObject(event, 5000) == WAIT_TIMEOUT)
    {
        OutputDebugString(L"Error starting SplashThread\n");
    }
}

SplashScreen::~SplashScreen()
{
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    if ( WaitForSingleObject(thread, 9000) == WAIT_TIMEOUT )
    {
        ::TerminateThread(thread, 2222 );
    }

    CloseHandle(thread);
    CloseHandle(event);

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void SplashScreen::SetLoadingMessage(int progress)
{
    PostThreadMessage(threadId, PBM_SETPOS, progress, reinterpret_cast<LPARAM>(nullptr));
}
