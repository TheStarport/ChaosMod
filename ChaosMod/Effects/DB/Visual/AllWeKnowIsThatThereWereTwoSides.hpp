#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/DirectX/Drawing.hpp"

#include <atlbase.h>
#include <dshow.h>
#include <stdio.h>
#include <thread>
#include <vmr9.h>

class AllWeKnowIsThatThereWereTwoSides final : public ActiveEffect
{
#pragma comment(lib, "Strmiids.lib");

        // locks a critical section, and unlocks it automatically
        // when the lock goes out of scope
        class CAutoLock final
        {
                // make copy constructor and assignment operator inaccessible
                CAutoLock(const CAutoLock& refAutoLock);
                CAutoLock& operator=(const CAutoLock& refAutoLock);

            protected:
                CRITICAL_SECTION* lock;

            public:
                CAutoLock(CRITICAL_SECTION* plock)
                {
                    lock = plock;
                    EnterCriticalSection(lock);
                };

                ~CAutoLock() { LeaveCriticalSection(lock); };
        };

        class VmrAllocator final : public IVMRSurfaceAllocator9, IVMRImagePresenter9
        {
                void DeleteSurfaces()
                {
                    CAutoLock Lock(section);

                    for (size_t i = 0; i < m_surfaces.size(); ++i)
                    {
                        m_surfaces[i] = nullptr;
                    }
                }

            public:
                VmrAllocator(CRITICAL_SECTION* section, IDirect3DDevice9* device, IDirect3DSurface9* surface)
                    : section(section), resultSurface(surface), device(device)
                {}

                // IVMRSurfaceAllocator9
                HRESULT __stdcall InitializeDevice(DWORD_PTR dwUserID, VMR9AllocationInfo* lpAllocInfo, DWORD* lpNumBuffers) override
                {
                    DWORD dwWidth = 1;
                    DWORD dwHeight = 1;

                    if (lpNumBuffers == nullptr)
                    {
                        return E_POINTER;
                    }

                    if (m_lpIVMRSurfAllocNotify == nullptr)
                    {
                        return E_FAIL;
                    }

                    HRESULT hr = S_OK;

                    auto res = DrawingHelper::i()->GetResolution();

                    lpAllocInfo->dwWidth = res.x;
                    lpAllocInfo->dwHeight = res.y;

                    // NOTE:
                    // we need to make sure that we create textures because
                    // surfaces can not be textured onto a primitive.
                    lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;

                    DeleteSurfaces();
                    m_surfaces.resize(*lpNumBuffers);
                    hr = m_lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, &m_surfaces.at(0));

                    return S_OK;
                }

                HRESULT __stdcall TerminateDevice(DWORD_PTR dwID) override { return S_OK; }

                HRESULT __stdcall GetSurface(DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9** lplpSurface) override
                {
                    if (lplpSurface == nullptr)
                    {
                        return E_POINTER;
                    }

                    if (SurfaceIndex >= m_surfaces.size())
                    {
                        return E_FAIL;
                    }

                    CAutoLock Lock(section);

                    return m_surfaces[SurfaceIndex].CopyTo(lplpSurface);
                }

                HRESULT __stdcall AdviseNotify(IVMRSurfaceAllocatorNotify9* lpIVMRSurfAllocNotify) override
                {
                    CAutoLock Lock(section);

                    m_lpIVMRSurfAllocNotify = lpIVMRSurfAllocNotify;

                    IDirect3D9* d3d;
                    device->GetDirect3D(&d3d);
                    HMONITOR hMonitor = d3d->GetAdapterMonitor(D3DADAPTER_DEFAULT);
                    m_lpIVMRSurfAllocNotify->SetD3DDevice(device, hMonitor);

                    d3d->Release();

                    return S_OK;
                }

                HRESULT __stdcall StartPresenting(DWORD_PTR dwUserID) override { return S_OK; }

                HRESULT __stdcall StopPresenting(DWORD_PTR dwUserID) override { return S_OK; }

                HRESULT __stdcall PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo* lpPresInfo) override
                {
                    CAutoLock Lock(section);

                    device->StretchRect(lpPresInfo->lpSurf, nullptr, resultSurface.p, nullptr, D3DTEXF_NONE);

                    return S_OK;
                }

                // IUnknown
                HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
                {
                    HRESULT hr = E_NOINTERFACE;

                    if (ppvObject == nullptr)
                    {
                        hr = E_POINTER;
                    }
                    else if (riid == IID_IVMRSurfaceAllocator9)
                    {
                        *ppvObject = static_cast<IVMRSurfaceAllocator9*>(this);
                        AddRef();
                        hr = S_OK;
                    }
                    else if (riid == IID_IVMRImagePresenter9)
                    {
                        *ppvObject = static_cast<IVMRImagePresenter9*>(this);
                        AddRef();
                        hr = S_OK;
                    }
                    else if (riid == IID_IUnknown)
                    {
                        *ppvObject = static_cast<IUnknown*>(static_cast<IVMRSurfaceAllocator9*>(this));
                        AddRef();
                        hr = S_OK;
                    }

                    return hr;
                }

                ULONG __stdcall AddRef() override { return InterlockedIncrement(&refCount); }

                ULONG __stdcall Release() override
                {
                    ULONG ret = InterlockedDecrement(&refCount);
                    if (ret == 0)
                    {
                        delete this;
                    }

                    return ret;
                }

            private:
                // needed to make this a thread safe object
                CRITICAL_SECTION* section;
                long refCount = 0;

                CComPtr<IVMRSurfaceAllocatorNotify9> m_lpIVMRSurfAllocNotify;
                std::vector<CComPtr<IDirect3DSurface9>> m_surfaces;
                CComPtr<IDirect3DSurface9> resultSurface;
                IDirect3DDevice9* device;
        };

        DWORD_PTR g_userId = 0xACDCACDC;

        CComPtr<IGraphBuilder> g_graph;
        CComPtr<IBaseFilter> g_filter;
        CComPtr<IMediaControl> g_mediaControl;
        CComPtr<IVMRSurfaceAllocator9> g_allocator;

        const wchar_t* wFileName = L"C:\\Users\\laz\\Downloads\\fl_intro.mpg";

        void SetAllocatorPresenter(CComPtr<IBaseFilter> filter)
        {
            if (filter == nullptr)
            {
                return;
            }

            HRESULT hr;

            CComPtr<IVMRSurfaceAllocatorNotify9> lpIVMRSurfAllocNotify;
            filter->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify));

            const auto drawing = DrawingHelper::i();
            auto [surface, section] = drawing->GetVideoSurface();

            g_allocator.Attach(new VmrAllocator(section, drawing->GetDeviceHandle(), surface));

            // let the allocator and the notify know about each other
            lpIVMRSurfAllocNotify->AdviseSurfaceAllocator(g_userId, g_allocator);
            g_allocator->AdviseNotify(lpIVMRSurfAllocNotify);
        }

        std::thread thread;

        void InitMovie()
        {
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (g_mediaControl != nullptr)
            {
                OAFilterState state;
                do
                {
                    g_mediaControl->Stop();
                    g_mediaControl->GetState(0, &state);
                }
                while (state != State_Stopped);
            }

            HRESULT hr;

            g_graph.CoCreateInstance(CLSID_FilterGraph);

            g_filter.CoCreateInstance(CLSID_VideoMixingRenderer9, nullptr, CLSCTX_INPROC_SERVER);
            CComPtr<IVMRFilterConfig9> filterConfig;
            g_filter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&filterConfig));

            filterConfig->SetRenderingMode(VMR9Mode_Renderless);

            filterConfig->SetNumberOfStreams(2);

            SetAllocatorPresenter(g_filter);

            g_graph->AddFilter(g_filter, L"Video Mixing Renderer 9");

            g_graph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&g_mediaControl));

            g_graph->RenderFile(wFileName, nullptr);

            // g_mediaControl->Run();
        }

        void Begin() override
        {
            // thread = std::thread(&AllWeKnowIsThatThereWereTwoSides::InitMovie, this);
            InitMovie();
        }

        void End() override {}

    public:
        DefEffectInfo("Portrait Mode", 1.0f, EffectType::Visual);
};
