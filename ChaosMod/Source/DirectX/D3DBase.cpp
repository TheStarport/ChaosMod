#include "PCH.hpp"

#include "DirectX/d3d.hpp"

IDirect3DDevice9* device;

static const D3DFORMAT AdapterFormats[] = { D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5 };

Direct3D8::Direct3D8(IDirect3D9* ProxyInterface)
{
    Direct3D8::ProxyInterface = ProxyInterface;
    D3DDISPLAYMODE pMode;

    CurrentAdapterCount = ProxyInterface->GetAdapterCount();
    if (CurrentAdapterCount > MaxAdapters)
    {
        CurrentAdapterCount = MaxAdapters;
    }

    for (uint Adapter = 0; Adapter < CurrentAdapterCount; Adapter++)
    {
        for (D3DFORMAT Format : AdapterFormats)
        {
            const uint ModeCount = ProxyInterface->GetAdapterModeCount(Adapter, Format);

            for (uint Mode = 0; Mode < ModeCount; Mode++)
            {
                ProxyInterface->EnumAdapterModes(Adapter, Format, Mode, &pMode);
                CurrentAdapterModes[Adapter].push_back(pMode);
                CurrentAdapterModeCount[Adapter]++;
            }
        }
    }
}
Direct3D8::~Direct3D8() {}

HRESULT STDMETHODCALLTYPE Direct3D8::QueryInterface(REFIID rid, void** ppvObj)
{
    if (ppvObj == nullptr)
    {
        return E_POINTER;
    }

    if (rid == __uuidof(this) || rid == __uuidof(IUnknown))
    {
        AddRef();
        *ppvObj = this;

        return S_OK;
    }

    return ProxyInterface->QueryInterface(ConvertREFIID(rid), ppvObj);
}
ULONG STDMETHODCALLTYPE Direct3D8::AddRef() { return ProxyInterface->AddRef(); }
ULONG STDMETHODCALLTYPE Direct3D8::Release()
{
    const ULONG LastRefCount = ProxyInterface->Release();

    if (LastRefCount == 0)
    {
        delete this;
    }

    return LastRefCount;
}

HRESULT STDMETHODCALLTYPE Direct3D8::RegisterSoftwareDevice(void* pInitializeFunction) { return ProxyInterface->RegisterSoftwareDevice(pInitializeFunction); }
uint STDMETHODCALLTYPE Direct3D8::GetAdapterCount() { return CurrentAdapterCount; }
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterIdentifier(uint Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier)
{
    if (pIdentifier == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    D3DADAPTER_IDENTIFIER9 AdapterIndentifier;

    if ((Flags & D3DENUM_NO_WHQL_LEVEL) == 0)
    {
        Flags |= D3DENUM_WHQL_LEVEL;
    }
    else
    {
        Flags ^= D3DENUM_NO_WHQL_LEVEL;
    }

    const HRESULT hr = ProxyInterface->GetAdapterIdentifier(Adapter, Flags, &AdapterIndentifier);
    if (FAILED(hr))
    {
        return hr;
    }

    ConvertAdapterIdentifier(AdapterIndentifier, *pIdentifier);

    return D3D_OK;
}
uint STDMETHODCALLTYPE Direct3D8::GetAdapterModeCount(uint Adapter) { return CurrentAdapterModeCount[Adapter]; }
HRESULT STDMETHODCALLTYPE Direct3D8::EnumAdapterModes(uint Adapter, uint Mode, D3DDISPLAYMODE* pMode)
{
    if (pMode == nullptr || !(Adapter < CurrentAdapterCount && Mode < CurrentAdapterModeCount[Adapter]))
    {
        return D3DERR_INVALIDCALL;
    }

    pMode->Format = CurrentAdapterModes[Adapter].at(Mode).Format;
    pMode->Height = CurrentAdapterModes[Adapter].at(Mode).Height;
    pMode->RefreshRate = CurrentAdapterModes[Adapter].at(Mode).RefreshRate;
    pMode->Width = CurrentAdapterModes[Adapter].at(Mode).Width;

    return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetAdapterDisplayMode(uint Adapter, D3DDISPLAYMODE* pMode)
{
    return ProxyInterface->GetAdapterDisplayMode(Adapter, pMode);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceType(uint Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
    return ProxyInterface->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, bWindowed);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceFormat(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType,
                                                       D3DFORMAT CheckFormat)
{
    if (CheckFormat == D3DFMT_UYVY || CheckFormat == D3DFMT_YUY2 || CheckFormat == MAKEFOURCC('Y', 'V', '1', '2') ||
        CheckFormat == MAKEFOURCC('N', 'V', '1', '2'))
    {
        return D3DERR_NOTAVAILABLE;
    }

    return ProxyInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDeviceMultiSampleType(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed,
                                                                D3DMULTISAMPLE_TYPE MultiSampleType)
{
    return ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, nullptr);
}
HRESULT STDMETHODCALLTYPE Direct3D8::CheckDepthStencilMatch(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat,
                                                            D3DFORMAT DepthStencilFormat)
{
    return ProxyInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}
HRESULT STDMETHODCALLTYPE Direct3D8::GetDeviceCaps(uint Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps)
{
    if (pCaps == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    D3DCAPS9 DeviceCaps;

    const HRESULT hr = ProxyInterface->GetDeviceCaps(Adapter, DeviceType, &DeviceCaps);
    if (FAILED(hr))
    {
        return hr;
    }

    ConvertCaps(DeviceCaps, *pCaps);

    return D3D_OK;
}
HMONITOR STDMETHODCALLTYPE Direct3D8::GetAdapterMonitor(uint Adapter) { return ProxyInterface->GetAdapterMonitor(Adapter); }
HRESULT STDMETHODCALLTYPE Direct3D8::CreateDevice(uint Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                                  D3DPRESENT_PARAMETERS8* pPresentationParameters, Direct3DDevice8** ppReturnedDeviceInterface)
{
    if (pPresentationParameters == nullptr || ppReturnedDeviceInterface == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    *ppReturnedDeviceInterface = nullptr;

    D3DPRESENT_PARAMETERS PresentParams;
    ConvertPresentParameters(*pPresentationParameters, PresentParams);

    // Get multisample quality level
    if (PresentParams.MultiSampleType != D3DMULTISAMPLE_NONE)
    {
        DWORD QualityLevels = 0;
        if (ProxyInterface->CheckDeviceMultiSampleType(
                Adapter, DeviceType, PresentParams.BackBufferFormat, PresentParams.Windowed, PresentParams.MultiSampleType, &QualityLevels) == S_OK &&
            ProxyInterface->CheckDeviceMultiSampleType(
                Adapter, DeviceType, PresentParams.AutoDepthStencilFormat, PresentParams.Windowed, PresentParams.MultiSampleType, &QualityLevels) == S_OK)
        {
            PresentParams.MultiSampleQuality = (QualityLevels != 0) ? QualityLevels - 1 : 0;
        }
    }

    IDirect3DDevice9* DeviceInterface = nullptr;

    const HRESULT hr = ProxyInterface->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &PresentParams, &DeviceInterface);
    if (FAILED(hr))
    {
        return hr;
    }

    DeviceInterface->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);

    *ppReturnedDeviceInterface = new Direct3DDevice8(this, DeviceInterface, (PresentParams.Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL) != 0);

    // Set default vertex declaration
    DeviceInterface->SetFVF(D3DFVF_XYZ);

    device = DeviceInterface;

    return D3D_OK;
}
