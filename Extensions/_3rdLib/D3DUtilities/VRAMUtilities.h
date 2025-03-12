#pragma once

#include "Adapter.h"

struct VRAMUtilities final :Adapter {
    struct VRAMInfo {
        DXGI_ADAPTER_DESC desc;
        UINT64 localBudgetOverride = 0;
        DXGI_QUERY_VIDEO_MEMORY_INFO localVideoMemoryInfo = {  };
        DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalVideoMemoryInfo = {  };
    };

    std::vector<VRAMInfo> infos;

    VRAMUtilities(DXGI& DXGI) :Adapter(DXGI) {
        for (auto& it : pAdapters) {
            infos.emplace_back();
            auto& info = infos.back();

            it->GetDesc(&info.desc);
            UpdateVideoMemoryInfo(it.Get(), info);
    }
    }
    ~VRAMUtilities() = default;

    inline size_t GetInfoCount() { return infos.size(); }

    inline const DXGI_ADAPTER_DESC& GetDesc(size_t index = 0) {
        return infos[index].desc;
    }

    inline const DXGI_QUERY_VIDEO_MEMORY_INFO& GetLocalVideoMemoryInfo(size_t index = 0) {
        return infos[index].localVideoMemoryInfo;
    }

    inline const DXGI_QUERY_VIDEO_MEMORY_INFO& GetNonLocalVideoMemoryInfo(size_t index = 0) {
        return infos[index].nonLocalVideoMemoryInfo;
    }

    inline const UINT64 GetLocalBudgetMB(size_t index = 0) {
        return (GetLocalVideoMemoryInfo().Budget >> 20);
    }

    inline const UINT64 GetLocalCurrentUsageMB(size_t index = 0) {
        return (GetLocalVideoMemoryInfo().CurrentUsage >> 20);
    }

    inline const UINT64 GetLocalAvailableForReservationMB(size_t index = 0) {
        return (GetLocalVideoMemoryInfo().AvailableForReservation >> 20);
    }

    inline const UINT64 GetLocalCurrentReservationMB(size_t index = 0) {
        return (GetLocalVideoMemoryInfo().CurrentReservation >> 20);
    }

    //https://blog.csdn.net/caoshangpa/article/details/78623044
    //https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/TechniqueDemos/D3D12MemoryManagement/src/Framework.h
    //https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/TechniqueDemos/D3D12MemoryManagement/src/Framework.cpp
    inline HRESULT UpdateVideoMemoryInfo(IDXGIAdapter3* pAdapter, VRAMInfo& info) {
        // QueryVideoMemoryInfo obtains up to date budgeting information from the graphics
        // kernel about video memory usage, budget, and reservation. This sample does not
        // use the reservation mechanics, but instead focuses on the proper techniques used
        // to manage the application's video memory usage and stay under budget through
        // trimming behavior.

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
        HRESULT hr;

        hr = pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info.localVideoMemoryInfo);
        if (FAILED(hr)) { return hr; }

        // Per the above comment, obtaining the non-local memory info is not strictly
        // required, and can typically be ignored completely. In some cases, such as on
        // integrated GPUs, non-local memory does not even exist, and QueryVideoMemoryInfo
        // will return all zero data. Non-local memory's best usage is often for a fallback
        // under high memory pressure in order to make forward progress on the system.
        hr = pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &info.nonLocalVideoMemoryInfo);
        if (FAILED(hr)) { return hr; }

        if (info.localBudgetOverride != 0) {
            info.localVideoMemoryInfo.Budget = info.localBudgetOverride;
        }

        return S_OK;
    }
    inline HRESULT UpdateVideoMemoryInfo(size_t index = 0) {        
        return UpdateVideoMemoryInfo(pAdapters[index].Get(), infos[index]);
    }
};

