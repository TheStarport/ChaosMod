#include "PCH.hpp"

#include "d3d.hpp"

Direct3DSurface8::Direct3DSurface8(Direct3DDevice8* Device, IDirect3DSurface9* ProxyInterface) : Device(Device), ProxyInterface(ProxyInterface)
{
    Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DSurface8::~Direct3DSurface8() {}

HRESULT STDMETHODCALLTYPE Direct3DSurface8::QueryInterface(REFIID rid, void** ppvObj)
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
ULONG STDMETHODCALLTYPE Direct3DSurface8::AddRef() { return ProxyInterface->AddRef(); }
ULONG STDMETHODCALLTYPE Direct3DSurface8::Release() { return ProxyInterface->Release(); }

HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetDevice(Direct3DDevice8** ppDevice)
{
    if (ppDevice == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    Device->AddRef();

    *ppDevice = Device;

    return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
    return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
    return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::FreePrivateData(REFGUID refguid) { return ProxyInterface->FreePrivateData(refguid); }
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetContainer(REFIID rid, void** ppContainer)
{
    const HRESULT hr = ProxyInterface->GetContainer(ConvertREFIID(rid), ppContainer);
    if (SUCCEEDED(hr))
    {
        GenericQueryInterface(rid, ppContainer, Device);
    }

    return hr;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::GetDesc(D3DSURFACE_DESC8* pDesc)
{
    if (pDesc == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    D3DSURFACE_DESC SurfaceDesc;

    const HRESULT hr = ProxyInterface->GetDesc(&SurfaceDesc);
    if (FAILED(hr))
    {
        return hr;
    }

    ConvertSurfaceDesc(SurfaceDesc, *pDesc);

    return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
    return ProxyInterface->LockRect(pLockedRect, pRect, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DSurface8::UnlockRect() { return ProxyInterface->UnlockRect(); }
