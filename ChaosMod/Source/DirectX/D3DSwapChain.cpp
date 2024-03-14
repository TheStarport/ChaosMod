#include "PCH.hpp"

#include "DirectX/d3d.hpp"

Direct3DSwapChain8::Direct3DSwapChain8(Direct3DDevice8* Device, IDirect3DSwapChain9* ProxyInterface) : Device(Device), ProxyInterface(ProxyInterface)
{
    Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DSwapChain8::~Direct3DSwapChain8() {}

HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::QueryInterface(REFIID rid, void** ppvObj)
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

    const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(rid), ppvObj);
    if (SUCCEEDED(hr))
    {
        GenericQueryInterface(rid, ppvObj, Device);
    }

    return hr;
}
ULONG STDMETHODCALLTYPE Direct3DSwapChain8::AddRef() { return ProxyInterface->AddRef(); }
ULONG STDMETHODCALLTYPE Direct3DSwapChain8::Release() { return ProxyInterface->Release(); }

HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
    UNREFERENCED_PARAMETER(pDirtyRegion);

    return ProxyInterface->Present(pSourceRect, pDestRect, hDestWindowOverride, nullptr, 0);
}
HRESULT STDMETHODCALLTYPE Direct3DSwapChain8::GetBackBuffer(uint iBackBuffer, D3DBACKBUFFER_TYPE Type, Direct3DSurface8** ppBackBuffer)
{
    if (ppBackBuffer == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    *ppBackBuffer = nullptr;

    IDirect3DSurface9* SurfaceInterface = nullptr;

    const HRESULT hr = ProxyInterface->GetBackBuffer(iBackBuffer, Type, &SurfaceInterface);
    if (FAILED(hr))
    {
        return hr;
    }

    *ppBackBuffer = Device->ProxyAddressLookupTable->FindAddress<Direct3DSurface8>(SurfaceInterface);

    return D3D_OK;
}
