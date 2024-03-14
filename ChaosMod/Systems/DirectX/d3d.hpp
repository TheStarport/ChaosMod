#pragma once
#define D3D_DEBUG_INFO
#include <string>
#include <vector>

#include <d3dx9.h>

#include "D3DTypes.hpp"
#include "Drawing.hpp"
#include "InterfaceQuery.hpp"

class AddressLookupTable;
class DECLSPEC_UUID("1DD9E8DA-1C77-4D40-B0CF-98FEFDFF9512") Direct3D8;
class DECLSPEC_UUID("7385E5DF-8FE8-41D5-86B6-D7B48547B6CF") Direct3DDevice8;
class DECLSPEC_UUID("928C088B-76B9-4C6B-A536-A590853876CD") Direct3DSwapChain8;
class DECLSPEC_UUID("1B36BB7B-09B7-410A-B445-7D1430D7B33F") Direct3DResource8;
class DECLSPEC_UUID("B4211CFA-51B9-4A9F-AB78-DB99B2BB678E") Direct3DBaseTexture8;
class DECLSPEC_UUID("E4CDD575-2866-4F01-B12E-7EECE1EC9358") Direct3DTexture8;
class DECLSPEC_UUID("3EE5B968-2ACA-4C34-8BB5-7E0C3D19B750") Direct3DCubeTexture8;
class DECLSPEC_UUID("4B8AAAFA-140F-42BA-9131-597EAFAA2EAD") Direct3DVolumeTexture8;
class DECLSPEC_UUID("B96EEBCA-B326-4EA5-882F-2FF5BAE021DD") Direct3DSurface8;
class DECLSPEC_UUID("BD7349F5-14F1-42E4-9C79-972380DB40C0") Direct3DVolume8;
class DECLSPEC_UUID("8AEEEAC7-05F9-44D4-B591-000B0DF1CB95") Direct3DVertexBuffer8;
class DECLSPEC_UUID("0E689C9A-053D-44A0-9D92-DB0E3D750F86") Direct3DIndexBuffer8;

class Direct3D8 : public IUnknown
{
        Direct3D8(const Direct3D8&) = delete;
        Direct3D8& operator=(const Direct3D8&) = delete;

    public:
        Direct3D8(IDirect3D9* ProxyInterface);
        ~Direct3D8();

        IDirect3D9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void* pInitializeFunction);
        virtual uint STDMETHODCALLTYPE GetAdapterCount();
        virtual HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(uint Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier);
        virtual uint STDMETHODCALLTYPE GetAdapterModeCount(uint Adapter);
        virtual HRESULT STDMETHODCALLTYPE EnumAdapterModes(uint Adapter, uint Mode, D3DDISPLAYMODE* pMode);
        virtual HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(uint Adapter, D3DDISPLAYMODE* pMode);
        virtual HRESULT STDMETHODCALLTYPE CheckDeviceType(uint Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat,
                                                          BOOL bWindowed);
        virtual HRESULT STDMETHODCALLTYPE CheckDeviceFormat(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType,
                                                            D3DFORMAT CheckFormat);
        virtual HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed,
                                                                     D3DMULTISAMPLE_TYPE MultiSampleType);
        virtual HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(uint Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat,
                                                                 D3DFORMAT DepthStencilFormat);
        virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps(uint Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps);
        virtual HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(uint Adapter);
        virtual HRESULT STDMETHODCALLTYPE CreateDevice(uint Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                                       D3DPRESENT_PARAMETERS8* pPresentationParameters, Direct3DDevice8** ppReturnedDeviceInterface);

    public:
        static IDirect3D9* ProxyInterface;
        static const uint MaxAdapters = 8;
        uint CurrentAdapterCount = 0;
        uint CurrentAdapterModeCount[MaxAdapters] = { 0 };
        std::vector<D3DDISPLAYMODE> CurrentAdapterModes[MaxAdapters];
};

class Direct3DDevice8 : public IUnknown
{
        Direct3DDevice8(const Direct3DDevice8&) = delete;
        Direct3DDevice8& operator=(const Direct3DDevice8&) = delete;
        HWND hwnd;

    public:
        Direct3DDevice8(Direct3D8* d3d, IDirect3DDevice9* ProxyInterface, BOOL EnableZBufferDiscarding = FALSE);
        ~Direct3DDevice8();

        IDirect3DDevice9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE TestCooperativeLevel();
        virtual uint STDMETHODCALLTYPE GetAvailableTextureMem();
        virtual HRESULT STDMETHODCALLTYPE ResourceManagerDiscardBytes(DWORD Bytes);
        virtual HRESULT STDMETHODCALLTYPE GetDirect3D(Direct3D8** ppD3D8);
        virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps(D3DCAPS8* pCaps);
        virtual HRESULT STDMETHODCALLTYPE GetDisplayMode(D3DDISPLAYMODE* pMode);
        virtual HRESULT STDMETHODCALLTYPE GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters);
        virtual HRESULT STDMETHODCALLTYPE SetCursorProperties(uint XHotSpot, uint YHotSpot, Direct3DSurface8* pCursorBitmap);
        virtual void STDMETHODCALLTYPE SetCursorPosition(uint XScreenSpace, uint YScreenSpace, DWORD Flags);
        virtual BOOL STDMETHODCALLTYPE ShowCursor(BOOL bShow);
        virtual HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS8* pPresentationParameters, Direct3DSwapChain8** ppSwapChain);
        virtual HRESULT STDMETHODCALLTYPE Reset(D3DPRESENT_PARAMETERS8* pPresentationParameters);
        virtual HRESULT STDMETHODCALLTYPE Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
        virtual HRESULT STDMETHODCALLTYPE GetBackBuffer(uint iBackBuffer, D3DBACKBUFFER_TYPE Type, Direct3DSurface8** ppBackBuffer);
        virtual HRESULT STDMETHODCALLTYPE GetRasterStatus(D3DRASTER_STATUS* pRasterStatus);
        virtual void STDMETHODCALLTYPE SetGammaRamp(DWORD Flags, const D3DGAMMARAMP* pRamp);
        virtual void STDMETHODCALLTYPE GetGammaRamp(D3DGAMMARAMP* pRamp);
        virtual HRESULT STDMETHODCALLTYPE CreateTexture(uint Width, uint Height, uint Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                        Direct3DTexture8** ppTexture);
        virtual HRESULT STDMETHODCALLTYPE CreateVolumeTexture(uint Width, uint Height, uint Depth, uint Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                              Direct3DVolumeTexture8** ppVolumeTexture);
        virtual HRESULT STDMETHODCALLTYPE CreateCubeTexture(uint EdgeLength, uint Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                            Direct3DCubeTexture8** ppCubeTexture);
        virtual HRESULT STDMETHODCALLTYPE CreateVertexBuffer(uint Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, Direct3DVertexBuffer8** ppVertexBuffer);
        virtual HRESULT STDMETHODCALLTYPE CreateIndexBuffer(uint Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, Direct3DIndexBuffer8** ppIndexBuffer);
        virtual HRESULT STDMETHODCALLTYPE CreateRenderTarget(uint Width, uint Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable,
                                                             Direct3DSurface8** ppSurface);
        virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(uint Width, uint Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                                                    Direct3DSurface8** ppSurface);
        virtual HRESULT STDMETHODCALLTYPE CreateImageSurface(uint Width, uint Height, D3DFORMAT Format, Direct3DSurface8** ppSurface);
        virtual HRESULT STDMETHODCALLTYPE CopyRects(Direct3DSurface8* pSourceSurface, const RECT* pSourceRectsArray, uint cRects,
                                                    Direct3DSurface8* pDestinationSurface, const POINT* pDestPointsArray);
        virtual HRESULT STDMETHODCALLTYPE UpdateTexture(Direct3DBaseTexture8* pSourceTexture, Direct3DBaseTexture8* pDestinationTexture);
        virtual HRESULT STDMETHODCALLTYPE GetFrontBuffer(Direct3DSurface8* pDestSurface);
        virtual HRESULT STDMETHODCALLTYPE SetRenderTarget(Direct3DSurface8* pRenderTarget, Direct3DSurface8* pNewZStencil);
        virtual HRESULT STDMETHODCALLTYPE GetRenderTarget(Direct3DSurface8** ppRenderTarget);
        virtual HRESULT STDMETHODCALLTYPE GetDepthStencilSurface(Direct3DSurface8** ppZStencilSurface);
        virtual HRESULT STDMETHODCALLTYPE BeginScene();
        virtual HRESULT STDMETHODCALLTYPE EndScene();
        virtual HRESULT STDMETHODCALLTYPE Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
        virtual HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);
        virtual HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
        virtual HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);
        virtual HRESULT STDMETHODCALLTYPE SetViewport(const D3DVIEWPORT8* pViewport);
        virtual HRESULT STDMETHODCALLTYPE GetViewport(D3DVIEWPORT8* pViewport);
        virtual HRESULT STDMETHODCALLTYPE SetMaterial(const D3DMATERIAL8* pMaterial);
        virtual HRESULT STDMETHODCALLTYPE GetMaterial(D3DMATERIAL8* pMaterial);
        virtual HRESULT STDMETHODCALLTYPE SetLight(DWORD Index, const D3DLIGHT8* pLight);
        virtual HRESULT STDMETHODCALLTYPE GetLight(DWORD Index, D3DLIGHT8* pLight);
        virtual HRESULT STDMETHODCALLTYPE LightEnable(DWORD Index, BOOL Enable);
        virtual HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD Index, BOOL* pEnable);
        virtual HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD Index, const float* pPlane);
        virtual HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD Index, float* pPlane);
        virtual HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
        virtual HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
        virtual HRESULT STDMETHODCALLTYPE BeginStateBlock();
        virtual HRESULT STDMETHODCALLTYPE EndStateBlock(DWORD* pToken);
        virtual HRESULT STDMETHODCALLTYPE ApplyStateBlock(DWORD Token);
        virtual HRESULT STDMETHODCALLTYPE CaptureStateBlock(DWORD Token);
        virtual HRESULT STDMETHODCALLTYPE DeleteStateBlock(DWORD Token);
        virtual HRESULT STDMETHODCALLTYPE CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD* pToken);
        virtual HRESULT STDMETHODCALLTYPE SetClipStatus(const D3DCLIPSTATUS8* pClipStatus);
        virtual HRESULT STDMETHODCALLTYPE GetClipStatus(D3DCLIPSTATUS8* pClipStatus);
        virtual HRESULT STDMETHODCALLTYPE GetTexture(DWORD Stage, Direct3DBaseTexture8** ppTexture);
        virtual HRESULT STDMETHODCALLTYPE SetTexture(DWORD Stage, Direct3DBaseTexture8* pTexture);
        virtual HRESULT STDMETHODCALLTYPE GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
        virtual HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
        virtual HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD* pNumPasses);
        virtual HRESULT STDMETHODCALLTYPE GetInfo(DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize);
        virtual HRESULT STDMETHODCALLTYPE SetPaletteEntries(uint PaletteNumber, const PALETTEENTRY* pEntries);
        virtual HRESULT STDMETHODCALLTYPE GetPaletteEntries(uint PaletteNumber, PALETTEENTRY* pEntries);
        virtual HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(uint PaletteNumber);
        virtual HRESULT STDMETHODCALLTYPE GetCurrentTexturePalette(uint* PaletteNumber);
        virtual HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, uint StartVertex, uint PrimitiveCount);
        virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, uint MinIndex, uint NumVertices, uint StartIndex,
                                                               uint PrimitiveCount);
        virtual HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, uint PrimitiveCount, const void* pVertexStreamZeroData,
                                                          uint VertexStreamZeroStride);
        virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, uint MinVertexIndex, uint NumVertexIndices,
                                                                 uint PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat,
                                                                 const void* pVertexStreamZeroData, uint VertexStreamZeroStride);
        virtual HRESULT STDMETHODCALLTYPE ProcessVertices(uint SrcStartIndex, uint DestIndex, uint VertexCount, Direct3DVertexBuffer8* pDestBuffer,
                                                          DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(const DWORD* pDeclaration, const DWORD* pFunction, DWORD* pHandle, DWORD Usage);
        virtual HRESULT STDMETHODCALLTYPE SetVertexShader(DWORD Handle);
        virtual HRESULT STDMETHODCALLTYPE GetVertexShader(DWORD* pHandle);
        virtual HRESULT STDMETHODCALLTYPE DeleteVertexShader(DWORD Handle);
        virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstant(DWORD Register, const void* pConstantData, DWORD ConstantCount);
        virtual HRESULT STDMETHODCALLTYPE GetVertexShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount);
        virtual HRESULT STDMETHODCALLTYPE GetVertexShaderDeclaration(DWORD Handle, void* pData, DWORD* pSizeOfData);
        virtual HRESULT STDMETHODCALLTYPE GetVertexShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData);
        virtual HRESULT STDMETHODCALLTYPE SetStreamSource(uint StreamNumber, Direct3DVertexBuffer8* pStreamData, uint Stride);
        virtual HRESULT STDMETHODCALLTYPE GetStreamSource(uint StreamNumber, Direct3DVertexBuffer8** ppStreamData, uint* pStride);
        virtual HRESULT STDMETHODCALLTYPE SetIndices(Direct3DIndexBuffer8* pIndexData, uint BaseVertexIndex);
        virtual HRESULT STDMETHODCALLTYPE GetIndices(Direct3DIndexBuffer8** ppIndexData, uint* pBaseVertexIndex);
        virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(const DWORD* pFunction, DWORD* pHandle);
        virtual HRESULT STDMETHODCALLTYPE SetPixelShader(DWORD Handle);
        virtual HRESULT STDMETHODCALLTYPE GetPixelShader(DWORD* pHandle);
        virtual HRESULT STDMETHODCALLTYPE DeletePixelShader(DWORD Handle);
        virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstant(DWORD Register, const void* pConstantData, DWORD ConstantCount);
        virtual HRESULT STDMETHODCALLTYPE GetPixelShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount);
        virtual HRESULT STDMETHODCALLTYPE GetPixelShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData);
        virtual HRESULT STDMETHODCALLTYPE DrawRectPatch(uint Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo);
        virtual HRESULT STDMETHODCALLTYPE DrawTriPatch(uint Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo);
        virtual HRESULT STDMETHODCALLTYPE DeletePatch(uint Handle);

        AddressLookupTable* ProxyAddressLookupTable;

    public:
        void ApplyClipPlanes();

        Direct3D8* const D3D;
        static IDirect3DDevice9* ProxyInterface;
        INT CurrentBaseVertexIndex = 0;
        const BOOL ZBufferDiscarding = FALSE;
        DWORD CurrentVertexShaderHandle = 0, CurrentPixelShaderHandle = 0;
        IDirect3DSurface9* pCurrentRenderTarget = nullptr;
        bool PaletteFlag = false;

        static constexpr size_t MAX_CLIP_PLANES = 6;
        float StoredClipPlanes[MAX_CLIP_PLANES][4] = {};
        DWORD ClipPlaneRenderState = 0;
};

class AddressLookupTableObject
{
    public:
        virtual ~AddressLookupTableObject() {}

        void DeleteMe() { delete this; }
};

class Direct3DSwapChain8 : public IUnknown, public AddressLookupTableObject
{
        Direct3DSwapChain8(const Direct3DSwapChain8&) = delete;
        Direct3DSwapChain8& operator=(const Direct3DSwapChain8&) = delete;

    public:
        Direct3DSwapChain8(Direct3DDevice8* device, IDirect3DSwapChain9* ProxyInterface);
        ~Direct3DSwapChain8() override;

        IDirect3DSwapChain9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
        virtual HRESULT STDMETHODCALLTYPE GetBackBuffer(uint iBackBuffer, D3DBACKBUFFER_TYPE Type, Direct3DSurface8** ppBackBuffer);

    private:
        Direct3DDevice8* const Device;
        IDirect3DSwapChain9* const ProxyInterface;
};

class Direct3DResource8 : public IUnknown
{
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0;
        virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) = 0;
        virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) = 0;
        virtual DWORD STDMETHODCALLTYPE GetPriority() = 0;
        virtual void STDMETHODCALLTYPE PreLoad() = 0;
        virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType() = 0;
};

class Direct3DBaseTexture8 : public Direct3DResource8
{
    public:
        virtual DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) = 0;
        virtual DWORD STDMETHODCALLTYPE GetLOD() = 0;
        virtual DWORD STDMETHODCALLTYPE GetLevelCount() = 0;
};
class Direct3DTexture8 : public Direct3DBaseTexture8, public AddressLookupTableObject
{
        Direct3DTexture8(const Direct3DTexture8&) = delete;
        Direct3DTexture8& operator=(const Direct3DTexture8&) = delete;

    public:
        Direct3DTexture8(Direct3DDevice8* device, IDirect3DTexture9* ProxyInterface);
        ~Direct3DTexture8() override;

        IDirect3DTexture9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) override;
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
        HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) override;
        DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) override;
        DWORD STDMETHODCALLTYPE GetPriority() override;
        void STDMETHODCALLTYPE PreLoad() override;
        D3DRESOURCETYPE STDMETHODCALLTYPE GetType() override;

        DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) override;
        DWORD STDMETHODCALLTYPE GetLOD() override;
        DWORD STDMETHODCALLTYPE GetLevelCount() override;

        virtual HRESULT STDMETHODCALLTYPE GetLevelDesc(uint Level, D3DSURFACE_DESC8* pDesc);
        virtual HRESULT STDMETHODCALLTYPE GetSurfaceLevel(uint Level, Direct3DSurface8** ppSurfaceLevel);
        virtual HRESULT STDMETHODCALLTYPE LockRect(uint Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE UnlockRect(uint Level);
        virtual HRESULT STDMETHODCALLTYPE AddDirtyRect(const RECT* pDirtyRect);

    private:
        Direct3DDevice8* const Device;
        IDirect3DTexture9* const ProxyInterface;
};
class Direct3DCubeTexture8 : public Direct3DBaseTexture8, public AddressLookupTableObject
{
        Direct3DCubeTexture8(const Direct3DCubeTexture8&) = delete;
        Direct3DCubeTexture8& operator=(const Direct3DCubeTexture8&) = delete;

    public:
        Direct3DCubeTexture8(Direct3DDevice8* device, IDirect3DCubeTexture9* ProxyInterface);
        ~Direct3DCubeTexture8() override;

        IDirect3DCubeTexture9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) override;
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
        HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) override;
        DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) override;
        DWORD STDMETHODCALLTYPE GetPriority() override;
        void STDMETHODCALLTYPE PreLoad() override;
        D3DRESOURCETYPE STDMETHODCALLTYPE GetType() override;

        DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) override;
        DWORD STDMETHODCALLTYPE GetLOD() override;
        DWORD STDMETHODCALLTYPE GetLevelCount() override;

        virtual HRESULT STDMETHODCALLTYPE GetLevelDesc(uint Level, D3DSURFACE_DESC8* pDesc);
        virtual HRESULT STDMETHODCALLTYPE GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, uint Level, Direct3DSurface8** ppCubeMapSurface);
        virtual HRESULT STDMETHODCALLTYPE LockRect(D3DCUBEMAP_FACES FaceType, uint Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE UnlockRect(D3DCUBEMAP_FACES FaceType, uint Level);
        virtual HRESULT STDMETHODCALLTYPE AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect);

    private:
        Direct3DDevice8* const Device;
        IDirect3DCubeTexture9* const ProxyInterface;
};
class Direct3DVolumeTexture8 : public Direct3DBaseTexture8, public AddressLookupTableObject
{
        Direct3DVolumeTexture8(const Direct3DVolumeTexture8&) = delete;
        Direct3DVolumeTexture8& operator=(const Direct3DVolumeTexture8&) = delete;

    public:
        Direct3DVolumeTexture8(Direct3DDevice8* device, IDirect3DVolumeTexture9* ProxyInterface);
        ~Direct3DVolumeTexture8() override;

        IDirect3DVolumeTexture9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) override;
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
        HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) override;
        DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) override;
        DWORD STDMETHODCALLTYPE GetPriority() override;
        void STDMETHODCALLTYPE PreLoad() override;
        D3DRESOURCETYPE STDMETHODCALLTYPE GetType() override;

        DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) override;
        DWORD STDMETHODCALLTYPE GetLOD() override;
        DWORD STDMETHODCALLTYPE GetLevelCount() override;

        virtual HRESULT STDMETHODCALLTYPE GetLevelDesc(uint Level, D3DVOLUME_DESC8* pDesc);
        virtual HRESULT STDMETHODCALLTYPE GetVolumeLevel(uint Level, Direct3DVolume8** ppVolumeLevel);
        virtual HRESULT STDMETHODCALLTYPE LockBox(uint Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE UnlockBox(uint Level);
        virtual HRESULT STDMETHODCALLTYPE AddDirtyBox(const D3DBOX* pDirtyBox);

    private:
        Direct3DDevice8* const Device;
        IDirect3DVolumeTexture9* const ProxyInterface;
};

class Direct3DSurface8 : public IUnknown, public AddressLookupTableObject
{
        Direct3DSurface8(const Direct3DSurface8&) = delete;
        Direct3DSurface8& operator=(const Direct3DSurface8&) = delete;

    public:
        Direct3DSurface8(Direct3DDevice8* device, IDirect3DSurface9* ProxyInterface);
        ~Direct3DSurface8() override;

        IDirect3DSurface9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice);
        virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
        virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
        virtual HRESULT STDMETHODCALLTYPE GetContainer(REFIID rid, void** ppContainer);
        virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC8* pDesc);
        virtual HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE UnlockRect();

    private:
        Direct3DDevice8* const Device;
        IDirect3DSurface9* const ProxyInterface;
};

class Direct3DVolume8 : public IUnknown, public AddressLookupTableObject
{
        Direct3DVolume8(const Direct3DVolume8&) = delete;
        Direct3DVolume8& operator=(const Direct3DVolume8&) = delete;

    public:
        Direct3DVolume8(Direct3DDevice8* Device, IDirect3DVolume9* ProxyInterface);
        ~Direct3DVolume8() override;

        IDirect3DVolume9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice);
        virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
        virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
        virtual HRESULT STDMETHODCALLTYPE GetContainer(REFIID rid, void** ppContainer);
        virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DVOLUME_DESC8* pDesc);
        virtual HRESULT STDMETHODCALLTYPE LockBox(D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE UnlockBox();

    private:
        Direct3DDevice8* const Device;
        IDirect3DVolume9* const ProxyInterface;
};

class Direct3DVertexBuffer8 : public Direct3DResource8, public AddressLookupTableObject
{
        Direct3DVertexBuffer8(const Direct3DVertexBuffer8&) = delete;
        Direct3DVertexBuffer8& operator=(const Direct3DVertexBuffer8&) = delete;

    public:
        Direct3DVertexBuffer8(Direct3DDevice8* Device, IDirect3DVertexBuffer9* ProxyInterface);
        ~Direct3DVertexBuffer8() override;

        IDirect3DVertexBuffer9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) override;
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
        HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) override;
        DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) override;
        DWORD STDMETHODCALLTYPE GetPriority() override;
        void STDMETHODCALLTYPE PreLoad() override;
        D3DRESOURCETYPE STDMETHODCALLTYPE GetType() override;

        virtual HRESULT STDMETHODCALLTYPE Lock(uint OffsetToLock, uint SizeToLock, BYTE** ppbData, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE Unlock();
        virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DVERTEXBUFFER_DESC* pDesc);

    private:
        Direct3DDevice8* const Device;
        IDirect3DVertexBuffer9* const ProxyInterface;
};

class Direct3DIndexBuffer8 : public Direct3DResource8, public AddressLookupTableObject
{
        Direct3DIndexBuffer8(const Direct3DIndexBuffer8&) = delete;
        Direct3DIndexBuffer8& operator=(const Direct3DIndexBuffer8&) = delete;

    public:
        Direct3DIndexBuffer8(Direct3DDevice8* Device, IDirect3DIndexBuffer9* ProxyInterface);
        ~Direct3DIndexBuffer8() override;

        IDirect3DIndexBuffer9* GetProxyInterface() const { return ProxyInterface; }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID rid, void** ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        HRESULT STDMETHODCALLTYPE GetDevice(Direct3DDevice8** ppDevice) override;
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
        HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) override;
        DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) override;
        DWORD STDMETHODCALLTYPE GetPriority() override;
        void STDMETHODCALLTYPE PreLoad() override;
        D3DRESOURCETYPE STDMETHODCALLTYPE GetType() override;

        virtual HRESULT STDMETHODCALLTYPE Lock(uint OffsetToLock, uint SizeToLock, BYTE** ppbData, DWORD Flags);
        virtual HRESULT STDMETHODCALLTYPE Unlock();
        virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DINDEXBUFFER_DESC* pDesc);

    private:
        Direct3DDevice8* const Device;
        IDirect3DIndexBuffer9* const ProxyInterface;
};

class AddressLookupTable
{
        template <typename T>
        struct AddressCacheIndex;

    public:
        AddressLookupTable(class Direct3DDevice8* Device) : Device(Device) {}
        ~AddressLookupTable();

        template <typename T>
        T* FindAddress(void* pAddress9)
        {
            if (pAddress9 == nullptr)
            {
                return nullptr;
            }

            auto& cache = AddressCache[AddressCacheIndex<T>::CacheIndex];

            const auto it = cache.find(pAddress9);
            if (it != cache.end())
            {
                return static_cast<T*>(it->second);
            }

            return new T(Device, static_cast<typename AddressCacheIndex<T>::Type9*>(pAddress9));
        }

        template <typename T>
        void SaveAddress(T* pAddress8, void* pAddress9)
        {
            if (pAddress8 == nullptr || pAddress9 == nullptr)
            {
                return;
            }

            AddressCache[AddressCacheIndex<T>::CacheIndex][pAddress9] = pAddress8;
        }

        template <typename T>
        void DeleteAddress(T* pAddress8)
        {
            if (pAddress8 == nullptr)
            {
                return;
            }

            auto& cache = AddressCache[AddressCacheIndex<T>::CacheIndex];

            const auto it = std::find_if(
                cache.begin(), cache.end(), [pAddress8](std::pair<void*, class AddressLookupTableObject*> Map) -> bool { return Map.second == pAddress8; });
            if (it != cache.end())
            {
                cache.erase(it);
            }
        }

    private:
        Direct3DDevice8* const Device;
        std::unordered_map<void*, class AddressLookupTableObject*> AddressCache[8];
};

template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DSurface8>
{
        static constexpr unsigned int CacheIndex = 0;
        typedef IDirect3DSurface9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DTexture8>
{
        static constexpr unsigned int CacheIndex = 1;
        typedef IDirect3DTexture9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DVolumeTexture8>
{
        static constexpr unsigned int CacheIndex = 2;
        typedef IDirect3DVolumeTexture9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<class Direct3DCubeTexture8>
{
        static constexpr unsigned int CacheIndex = 3;
        typedef IDirect3DCubeTexture9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DVolume8>
{
        static constexpr unsigned int CacheIndex = 4;
        typedef IDirect3DVolume9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DVertexBuffer8>
{
        static constexpr unsigned int CacheIndex = 5;
        typedef IDirect3DVertexBuffer9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DIndexBuffer8>
{
        static constexpr unsigned int CacheIndex = 6;
        typedef IDirect3DIndexBuffer9 Type9;
};
template <>
struct AddressLookupTable::AddressCacheIndex<Direct3DSwapChain8>
{
        static constexpr unsigned int CacheIndex = 7;
        typedef IDirect3DSwapChain9 Type9;
};

void GenericQueryInterface(REFIID rid, LPVOID* ppvObj, class Direct3DDevice8* pDevice);

REFIID ConvertREFIID(REFIID rid);

extern IDirect3DDevice9* device;
