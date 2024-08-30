#include "HookD3D.h"

#include <d3d11.h>
#pragma comment(lib, "D3D11.lib")

#include <Windows.h>
#include <psapi.h>

#include "Detours/detours.h"
#pragma comment(lib, "D3D/Detours/detours.lib")

decltype(D3D11CreateDevice)* addrD3D11CreateDevice = nullptr;
decltype(D3D11CreateDeviceAndSwapChain)* addrD3D11CreateDeviceAndSwapChain = nullptr;

HRESULT WINAPI D3D11CreateDeviceOverride(
    _In_opt_ IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    _COM_Outptr_opt_ ID3D11Device** ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
    _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext) {
#ifdef _DEBUG
    Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    return addrD3D11CreateDevice(pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        ppDevice, pFeatureLevel, ppImmediateContext);
}

HRESULT WINAPI D3D11CreateDeviceAndSwapChainOverride(
    _In_opt_ IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    _In_opt_ CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    _COM_Outptr_opt_ IDXGISwapChain** ppSwapChain,
    _COM_Outptr_opt_ ID3D11Device** ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
    _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext) {
    return addrD3D11CreateDeviceAndSwapChain(pAdapter,DriverType,Software,Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        pSwapChainDesc, ppSwapChain,
        ppDevice, pFeatureLevel, ppImmediateContext);
}

// References:
// https://www.unknowncheats.me/forum/d3d-tutorials-and-source/88297-directx-11-hooking-detours.html
void HookD3D::AttachCreateDevice() {
    HMODULE hD3D11 = nullptr;
    hD3D11 = GetModuleHandle(L"d3d11.dll");

    // handle the dll init order may be different
    if (!hD3D11) {
        hD3D11 = LoadLibrary(L"d3d11.dll");
    }

    MODULEINFO Info_D3D11 = {  };
    auto bRet = GetModuleInformation(GetCurrentProcess(), hD3D11, &Info_D3D11, sizeof(MODULEINFO));

    addrD3D11CreateDevice = reinterpret_cast<decltype(D3D11CreateDevice)*>(GetProcAddress(hD3D11,"D3D11CreateDevice"));
    addrD3D11CreateDeviceAndSwapChain = reinterpret_cast<decltype(D3D11CreateDeviceAndSwapChain)*>(GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain"));

    LONG detourResult = NO_ERROR;

	DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());        
    detourResult = DetourAttach(&(PVOID&)addrD3D11CreateDevice,
        (PVOID)D3D11CreateDeviceOverride);
    detourResult = DetourAttach(&(PVOID&)addrD3D11CreateDeviceAndSwapChain,
        (PVOID)D3D11CreateDeviceAndSwapChainOverride);
    DetourTransactionCommit();
}
void HookD3D::DetachCreateDevice() {}
