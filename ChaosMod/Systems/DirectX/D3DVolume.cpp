#include "PCH.hpp"

#include "d3d.hpp"

Direct3DVolume8::Direct3DVolume8(Direct3DDevice8* Device, IDirect3DVolume9* ProxyInterface) : Device(Device), ProxyInterface(ProxyInterface)
{
    Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DVolume8::~Direct3DVolume8() {}

HRESULT STDMETHODCALLTYPE Direct3DVolume8::QueryInterface(REFIID rid, void** ppvObj)
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
ULONG STDMETHODCALLTYPE Direct3DVolume8::AddRef() { return ProxyInterface->AddRef(); }
ULONG STDMETHODCALLTYPE Direct3DVolume8::Release() { return ProxyInterface->Release(); }

HRESULT STDMETHODCALLTYPE Direct3DVolume8::GetDevice(Direct3DDevice8** ppDevice)
{
    if (ppDevice == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    Device->AddRef();
    *ppDevice = Device;

    return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
    return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
    return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::FreePrivateData(REFGUID refguid) { return ProxyInterface->FreePrivateData(refguid); }
HRESULT STDMETHODCALLTYPE Direct3DVolume8::GetContainer(REFIID rid, void** ppContainer)
{
    const HRESULT hr = ProxyInterface->GetContainer(ConvertREFIID(rid), ppContainer);
    if (SUCCEEDED(hr))
    {
        GenericQueryInterface(rid, ppContainer, Device);
    }

    return hr;
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::GetDesc(D3DVOLUME_DESC8* pDesc)
{
    if (pDesc == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    D3DVOLUME_DESC VolumeDesc;

    const HRESULT hr = ProxyInterface->GetDesc(&VolumeDesc);
    if (FAILED(hr))
    {
        return hr;
    }

    ConvertVolumeDesc(VolumeDesc, *pDesc);

    return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::LockBox(D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags)
{
    return ProxyInterface->LockBox(pLockedVolume, pBox, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVolume8::UnlockBox() { return ProxyInterface->UnlockBox(); }
