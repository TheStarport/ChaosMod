#pragma once

struct IUnknown;

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define D3D_DEBUG_INFO
#include <d3d9.h>

#ifndef D3DX_FILTER_NONE
#define D3DX_FILTER_NONE 1
#endif

#define D3DXASM_DEBUG 0x0001
#define D3DXASM_SKIPVALIDATION  0x0010

#ifdef NDEBUG
#define D3DXASM_FLAGS  0
#else
#define D3DXASM_FLAGS D3DXASM_DEBUG
#endif // NDEBUG

/*struct D3DXMACRO
{
	LPCSTR Name;
	LPCSTR Definition;
};

typedef interface ID3DXBuffer* LPD3DXBUFFER;
typedef interface ID3DXInclude* LPD3DXINCLUDE;
typedef interface ID3DXSprite* LPD3DXSprite;

DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
	// IUnknown
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID * ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// ID3DXBuffer
	STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
	STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};*/

/*typedef HRESULT(WINAPI* PFN_D3DXAssembleShader)(LPCSTR pSrcData, uint SrcDataLen, const D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs);
typedef HRESULT(WINAPI* PFN_D3DXDisassembleShader)(const DWORD* pShader, BOOL EnableColorCode, LPCSTR pComments, LPD3DXBUFFER* ppDisassembly);
typedef HRESULT(WINAPI* PFN_D3DXLoadSurfaceFromSurface)(LPDIRECT3DSURFACE9 pDestSurface, const PALETTEENTRY* pDestPalette, const RECT* pDestRect, LPDIRECT3DSURFACE9 pSrcSurface, const PALETTEENTRY* pSrcPalette, const RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);
typedef HRESULT(WINAPI* PFN_D3DXCreateTextureFromFileInMemory)(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, uint srcDataSize, LPDIRECT3DTEXTURE9* ppTexture);
typedef HRESULT(WINAPI* PFN_D3DXCreateSprite)(LPDIRECT3DDEVICE9 pDevice, LPD3DXSprite* ppSprite);

extern PFN_D3DXAssembleShader D3DXAssembleShader;
extern PFN_D3DXDisassembleShader D3DXDisassembleShader;
extern PFN_D3DXLoadSurfaceFromSurface D3DXLoadSurfaceFromSurface;
extern PFN_D3DXCreateTextureFromFileInMemory D3DXCreateTextureFromFileInMemory;
extern PFN_D3DXCreateSprite D3DXCreateSprite;*/

#define D3DFMT_W11V11U10 65

#define D3DRS_LINEPATTERN 10
#define D3DRS_ZVISIBLE 30
#define D3DRS_EDGEANTIALIAS 40
#define D3DRS_ZBIAS 47
#define D3DRS_SOFTWAREVERTEXPROCESSING 153
#define D3DRS_PATCHSEGMENTS 164

#define D3DTSS_ADDRESSU 13
#define D3DTSS_ADDRESSV 14
#define D3DTSS_ADDRESSW 25
#define D3DTSS_BORDERCOLOR 15
#define D3DTSS_MAGFILTER 16
#define D3DTSS_MINFILTER 17
#define D3DTSS_MIPFILTER 18
#define D3DTSS_MIPMAPLODBIAS 19
#define D3DTSS_MAXMIPLEVEL 20
#define D3DTSS_MAXANISOTROPY 21

#define D3DVSD_TOKEN_STREAM 1
#define D3DVSD_TOKEN_STREAMDATA 2
#define D3DVSD_TOKEN_TESSELLATOR 3
#define D3DVSD_TOKEN_CONSTMEM 4
#define D3DVSD_TOKENTYPESHIFT 29
#define D3DVSD_TOKENTYPEMASK (0x7 << D3DVSD_TOKENTYPESHIFT)
#define D3DVSD_STREAMNUMBERSHIFT 0
#define D3DVSD_STREAMNUMBERMASK (0xF << D3DVSD_STREAMNUMBERSHIFT)
#define D3DVSD_VERTEXREGSHIFT 0
#define D3DVSD_VERTEXREGMASK (0x1F << D3DVSD_VERTEXREGSHIFT)
#define D3DVSD_VERTEXREGINSHIFT 20
#define D3DVSD_VERTEXREGINMASK (0xF << D3DVSD_VERTEXREGINSHIFT)
#define D3DVSD_DATATYPESHIFT 16
#define D3DVSD_DATATYPEMASK (0xF << D3DVSD_DATATYPESHIFT)
#define D3DVSD_SKIPCOUNTSHIFT 16
#define D3DVSD_SKIPCOUNTMASK (0xF << D3DVSD_SKIPCOUNTSHIFT)
#define D3DVSD_CONSTCOUNTSHIFT 25
#define D3DVSD_CONSTCOUNTMASK (0xF << D3DVSD_CONSTCOUNTSHIFT)
#define D3DVSD_CONSTADDRESSSHIFT 0
#define D3DVSD_CONSTADDRESSMASK (0x7F << D3DVSD_CONSTADDRESSSHIFT)
#define D3DVSD_END() 0xFFFFFFFF

// Fix for MinGW headers which are missing defines in d3d9.h...
#ifndef D3DENUM_WHQL_LEVEL
#define D3DENUM_WHQL_LEVEL 0x00000002L
#endif
// Fix for MinGW headers which have some d3d8 defines in d3d9.h...
#ifndef D3DENUM_NO_WHQL_LEVEL
#define D3DENUM_NO_WHQL_LEVEL 0x00000002L
#endif
#define D3DSWAPEFFECT_COPY_VSYNC 4
#define D3DPRESENT_RATE_UNLIMITED 0x7FFFFFFF
// Fix for MinGW headers which have some d3d8 defines in d3d9.h...
#ifndef D3DCAPS2_CANRENDERWINDOWED
#define D3DCAPS2_CANRENDERWINDOWED 0x00080000L
#endif
#define D3DPRASTERCAPS_ZBIAS 0x00004000L

typedef D3DLIGHT9 D3DLIGHT8;
typedef D3DMATERIAL9 D3DMATERIAL8;
typedef D3DVIEWPORT9 D3DVIEWPORT8;
typedef D3DCLIPSTATUS9 D3DCLIPSTATUS8;

struct D3DCAPS8
{
	D3DDEVTYPE DeviceType;
	uint AdapterOrdinal;
	DWORD Caps, Caps2, Caps3;
	DWORD PresentationIntervals;
	DWORD CursorCaps;
	DWORD DevCaps;
	DWORD PrimitiveMiscCaps;
	DWORD RasterCaps;
	DWORD ZCmpCaps;
	DWORD SrcBlendCaps;
	DWORD DestBlendCaps;
	DWORD AlphaCmpCaps;
	DWORD ShadeCaps;
	DWORD TextureCaps;
	DWORD TextureFilterCaps;
	DWORD CubeTextureFilterCaps;
	DWORD VolumeTextureFilterCaps;
	DWORD TextureAddressCaps;
	DWORD VolumeTextureAddressCaps;
	DWORD LineCaps;
	DWORD MaxTextureWidth, MaxTextureHeight;
	DWORD MaxVolumeExtent;
	DWORD MaxTextureRepeat;
	DWORD MaxTextureAspectRatio;
	DWORD MaxAnisotropy;
	float MaxVertexW;
	float GuardBandLeft, GuardBandTop, GuardBandRight, GuardBandBottom;
	float ExtentsAdjust;
	DWORD StencilCaps;
	DWORD FVFCaps;
	DWORD TextureOpCaps;
	DWORD MaxTextureBlendStages;
	DWORD MaxSimultaneousTextures;
	DWORD VertexProcessingCaps;
	DWORD MaxActiveLights;
	DWORD MaxUserClipPlanes;
	DWORD MaxVertexBlendMatrices;
	DWORD MaxVertexBlendMatrixIndex;
	float MaxPointSize;
	DWORD MaxPrimitiveCount;
	DWORD MaxVertexIndex;
	DWORD MaxStreams;
	DWORD MaxStreamStride;
	DWORD VertexShaderVersion;
	DWORD MaxVertexShaderConst;
	DWORD PixelShaderVersion;
	float MaxPixelShaderValue;
};

struct D3DVOLUME_DESC8
{
	D3DFORMAT Format;
	D3DRESOURCETYPE Type;
	DWORD Usage;
	D3DPOOL Pool;
	uint Size;
	uint Width, Height, Depth;
};
struct D3DSURFACE_DESC8
{
	D3DFORMAT Format;
	D3DRESOURCETYPE Type;
	DWORD Usage;
	D3DPOOL Pool;
	uint Size;
	D3DMULTISAMPLE_TYPE MultiSampleType;
	uint Width, Height;
};

struct D3DPRESENT_PARAMETERS8
{
	uint BackBufferWidth, BackBufferHeight;
	D3DFORMAT BackBufferFormat;
	uint BackBufferCount;
	D3DMULTISAMPLE_TYPE MultiSampleType;
	D3DSWAPEFFECT SwapEffect;
	HWND hDeviceWindow;
	BOOL Windowed;
	BOOL EnableAutoDepthStencil;
	D3DFORMAT AutoDepthStencilFormat;
	DWORD Flags;
	uint FullScreen_RefreshRateInHz, FullScreen_PresentationInterval;
};

struct D3DADAPTER_IDENTIFIER8
{
	char Driver[MAX_DEVICE_IDENTIFIER_STRING];
	char Description[MAX_DEVICE_IDENTIFIER_STRING];
	LARGE_INTEGER DriverVersion;
	DWORD VendorId, DeviceId, SubSysId;
	DWORD Revision;
	GUID DeviceIdentifier;
	DWORD WHQLLevel;
};

bool SupportsPalettes();

void ConvertCaps(D3DCAPS9& input, D3DCAPS8& output);
void ConvertVolumeDesc(D3DVOLUME_DESC& input, D3DVOLUME_DESC8& output);
void ConvertSurfaceDesc(D3DSURFACE_DESC& input, D3DSURFACE_DESC8& output);
void ConvertPresentParameters(D3DPRESENT_PARAMETERS8& input, D3DPRESENT_PARAMETERS& output);
void ConvertAdapterIdentifier(D3DADAPTER_IDENTIFIER9& input, D3DADAPTER_IDENTIFIER8& output);