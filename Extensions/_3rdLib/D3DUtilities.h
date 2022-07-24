#pragma once

#include <dxgi1_4.h>

#pragma comment(lib,"DXGI.lib")

class D3DUtilities {
private:
    IDXGIFactory2* m_pDXGIFactory = nullptr;
    UINT dxgiFactoryFlags = 0;

    IDXGIAdapter3* m_pDXGIAdapter = nullptr;
    UINT64 m_LocalBudgetOverride = 0;

    UINT AdapterOrdinal = 0;
    DXGI_ADAPTER_DESC Desc;

    DXGI_QUERY_VIDEO_MEMORY_INFO m_LocalVideoMemoryInfo = { 0,0,0,0 };
    DXGI_QUERY_VIDEO_MEMORY_INFO m_NonLocalVideoMemoryInfo = { 0,0,0,0 };

public:
    D3DUtilities() {
        auto hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pDXGIFactory));
        if (FAILED(hr)) {
            //LOG_ERROR("Failed to create DXGI Factory, hr=0x%.8x", hr);
            throw false;
        }

        this->AdapterOrdinal = this->GetProcessAdapterOrdinal();
    }

    ~D3DUtilities() {
        if (m_pDXGIFactory != nullptr) {
            m_pDXGIFactory->Release();
        }

        if (m_pDXGIAdapter != nullptr) {
            m_pDXGIAdapter->Release();
        }
    }

    inline const DXGI_ADAPTER_DESC& GetDesc() {
        return this->Desc;
    }

    inline const DXGI_QUERY_VIDEO_MEMORY_INFO& GetLocalVideoMemoryInfo() {
        return this->m_LocalVideoMemoryInfo;
    }

    //inline const DXGI_QUERY_VIDEO_MEMORY_INFO& GetNonLocalVideoMemoryInfo() {
    //    return this->m_NonLocalVideoMemoryInfo;
    //}

    inline const UINT64 GetLocalBudgetMB() {
        return (GetLocalVideoMemoryInfo().Budget >> 20);
    }

    inline const UINT64 GetLocalCurrentUsageMB() {
        return (GetLocalVideoMemoryInfo().CurrentUsage >> 20);
    }

    inline const UINT64 GetLocalAvailableForReservationMB() {
        return (GetLocalVideoMemoryInfo().AvailableForReservation >> 20);
    }

    inline const UINT64 GetLocalCurrentReservationMB() {
        return (GetLocalVideoMemoryInfo().CurrentReservation >> 20);
    }

    inline HRESULT GetProcessAdapterOrdinal() {
        HRESULT hr;

        UINT AdapterOrdinal = 0;
        IDXGIAdapter* pTempAdapter;

        while (m_pDXGIFactory->EnumAdapters(AdapterOrdinal, &pTempAdapter) != DXGI_ERROR_NOT_FOUND) {
            hr = pTempAdapter->QueryInterface(&m_pDXGIAdapter);
            pTempAdapter->Release();

            if (FAILED(hr)) {
                //LOG_ERROR("Failed to query IDXGIAdapter3 interface from selected adapter.");
                return hr;
            }

            hr = m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &m_LocalVideoMemoryInfo);
            if (FAILED(hr)) {
                this->m_LocalVideoMemoryInfo = { 0,0,0,0 };
            }

            //hr = m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &m_NonLocalVideoMemoryInfo);
            //if (FAILED(hr)) {
            //    this->m_NonLocalVideoMemoryInfo = { 0,0,0,0 };
            //}

            auto usageLocal = this->m_LocalVideoMemoryInfo.CurrentUsage >> 20;
            //auto usageNonLocal = this->m_NonLocalVideoMemoryInfo.CurrentUsage >> 20;

            //if (usageLocal != 0 || usageNonLocal != 0) {
            //    return AdapterOrdinal;
            //}

            if (usageLocal != 0) {
                m_pDXGIAdapter->GetDesc(&Desc);

                return AdapterOrdinal;
            }

            ++AdapterOrdinal;
        }

        return S_OK;
    }

    //https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/TechniqueDemos/D3D12MemoryManagement/src/Framework.h
    //https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/TechniqueDemos/D3D12MemoryManagement/src/D3D12MemoryManagement.cpp
    inline HRESULT UpdateVideoMemoryInfo() {
        HRESULT hr;

        this->m_LocalVideoMemoryInfo = { 0,0,0,0 };
        //this->m_NonLocalVideoMemoryInfo = { 0,0,0,0 };

        //IDXGIAdapter* pTempAdapter;

        //hr = m_pDXGIFactory->EnumAdapters(AdapterOrdinal, &pTempAdapter);
        //if (hr == DXGI_ERROR_NOT_FOUND) {
        //    //LOG_WARNING("Invalid adapter ordinal");
        //    goto CleanUp;
        //}

        //hr = pTempAdapter->QueryInterface(&m_pDXGIAdapter);
        //pTempAdapter->Release();

        //if (FAILED(hr)) {
        //    //LOG_ERROR("Failed to query IDXGIAdapter3 interface from selected adapter.");
        //    goto CleanUp;
        //}

        //
        // QueryVideoMemoryInfo obtains up to date budgeting information from the graphics
        // kernel about video memory usage, budget, and reservation. This sample does not
        // use the reservation mechanics, but instead focuses on the proper techniques used
        // to manage the application's video memory usage and stay under budget through
        // trimming behavior.
        //

        //
        // Each call to QueryVideoMemoryInfo takes a memory segment group. Applications
        // should design their video memory usage around the local memory group budget,
        // and the non-local budget can usually be safely ignored. Non-local memory refers
        // to memory that is not located near the GPU, and as a result is typically much
        // slower than the local mmeory. On a discrete video card, the local and non-local
        // memory groups would represent VRAM and system memory, respectively. Accessing
        // system memory via PCI-e is far slower than accessing VRAM directly on-board,
        // and should typically be avoided in high performance graphics. Although some
        // small or low bandwidth allocations can effectively be placed here, it is not
        // recommended for normal texture memory. The local memory group always refers to
        // the fastest and most ideal placement for texture data.
        //
        hr = m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &m_LocalVideoMemoryInfo);
        if (FAILED(hr)) {
            //LOG_WARNING("Failed to update non-local video memory info. If the process budget has been lowered, trimming will not be done, hr=0x%.8x", hr);
            goto CleanUp;
        }

        //
        // Per the above comment, obtaining the non-local memory info is not strictly
        // required, and can typically be ignored completely. In some cases, such as on
        // integrated GPUs, non-local memory does not even exist, and QueryVideoMemoryInfo
        // will return all zero data. Non-local memory's best usage is often for a fallback
        // under high memory pressure in order to make forward progress on the system.
        //
        //hr = m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &m_NonLocalVideoMemoryInfo);
        //if (FAILED(hr)) {
        //    //LOG_WARNING("Failed to update local video memory info. If the process budget has been lowered, trimming will not be done, hr=0x%.8x", hr);
        //    goto CleanUp;
        //}

        if (m_LocalBudgetOverride != 0) {
            m_LocalVideoMemoryInfo.Budget = m_LocalBudgetOverride;
        }

        hr = S_OK;

    CleanUp:

        return hr;
    }
};

