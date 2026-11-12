#pragma once
// Test-only SDK stand-ins. Never on the plugin include path. Native D3D/Win32
// types are real; Fusion layout, rendering and pixel correctness are not emulated.
#define COMMON_H
#include <windows.h>
#include <d3d11.h>
#include <memory>
#include <cstdint>

constexpr int SD_D3D9 = 9, SD_D3D11 = 11, ST_HWA_RTTEXTURE = 5;
constexpr DWORD AH2OPT_RESAMPLESTRETCH = 1;
inline int mockSurfaceCount = 0;
class cSurface {
    int width, height;
public:
    int driver = SD_D3D11;
    int generation = 0;
    cSurface(int w = 1, int h = 1) : width(w), height(h) { ++mockSurfaceCount; }
    ~cSurface() { --mockSurfaceCount; }
    bool IsValid() const { return width > 0 && height > 0; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetDepth() const { return 32; }
    int GetDriver() const { return driver; }
    int GetType() const { return ST_HWA_RTTEXTURE; }
    void GetOrigin(POINT& p) const { p = {}; }
    void CreateAlpha() {}
    HDC GetDC() { return nullptr; }
    void ReleaseDC(HDC) {}
};
using LPSURFACE = cSurface*;
struct MockEffect { DWORD ffxInkEffect = 0; };
struct MockFrame {
    cSurface* m_pSecondarySurface = nullptr;
    bool m_bAlwaysUseSecondarySurface = false;
    void* m_pLayerEffects = nullptr;
    MockEffect* m_pEffect = nullptr;
    void* m_pEffectEx = nullptr;
    float m_angle = 0, m_xScale = 1, m_yScale = 1;
    POINT m_destPoint{}, m_hotSpot{};
};
struct MockHeader2 { DWORD dwOptions = 0; };
struct MockApp {
    cSurface* m_protoLogScreen = nullptr;
    MockHeader2* m_pHdr2 = nullptr;
    bool m_bAlwaysUseSecondarySurface = false;
    cSurface* m_pOldSecondarySurface = nullptr;
    MockFrame* m_Frame = nullptr;
};
struct MockRunHeader {
    HWND rhHEditWin = nullptr;
    MockApp* rhApp = nullptr;
    MockFrame* rhFrame = nullptr;
    struct { int rh3WindowSx = 640, rh3WindowSy = 480; } rh3;
};
struct MockRunData {
    struct { MockRunHeader* hoAdRunHeader = nullptr; } rHo;
    bool HWA = false, isLib = false;
    struct Display {
        int driver = SD_D3D11;
        int GetDriver() const { return driver; }
    } display;
    bool surfaceReady = true;
};
using LPRDATA = MockRunData*;
struct MockSurfaceInfo {
    int m_nD3DVersion = 11;
    void* m_pD3D11Device = nullptr;
    void* m_pD3D11Context = nullptr;
    void* m_ppD3D11RenderTargetTexture = nullptr;
    void* m_ppD3D11RenderTargetView = nullptr;
};
inline auto WinGetSurface(LPRDATA rdPtr) { return rdPtr->surfaceReady ? &rdPtr->display : nullptr; }
inline bool D3D11(LPRDATA rdPtr) { return rdPtr->display.driver == SD_D3D11; }
inline void* GetD3D11Device(LPRDATA) { return nullptr; }
inline MockSurfaceInfo GetD3D11Info(LPRDATA) { return {}; }
inline MockSurfaceInfo GetD3D11SurfaceInfo(cSurface*) { return {}; }
inline cSurface* CreateSurface(int, int w, int h) { return new cSurface(w, h); }
inline cSurface* CreateHWASurface(LPRDATA, int, int w, int h, int) { return new cSurface(w, h); }
inline cSurface* CreateCloneSurface(LPRDATA, cSurface* p) { return new cSurface(p->GetWidth(), p->GetHeight()); }
inline void _AddAlpha(cSurface*) {}
struct RenderHelper { RenderHelper(cSurface*, BOOL, std::uint32_t = 0) {} };
using RGBAREF = std::uint32_t;
constexpr DWORD LoadFromPointerFlags_TakeOwnership = 2;
inline std::unique_ptr<cSurface> mockPublished;
inline int mockHandoffs = 0;
inline bool mockOwnedHandoff = true;
inline void LoadFromPointer(LPRDATA, LPCWSTR name, cSurface* surface, DWORD flags) {
    mockOwnedHandoff &= flags == LoadFromPointerFlags_TakeOwnership;
    constexpr auto expected = L"_TempCapture";
    for (unsigned i = 0; i < 13; ++i) mockOwnedHandoff &= name[i] == expected[i];
    mockPublished.reset(surface);
    ++mockHandoffs;
}
#include "../FrameCapture.h"
