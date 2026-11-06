#pragma once

#include <d3d11.h>
#include <memory>

// Keep C++/WinRT out of the legacy /Zc:wchar_t- Fusion translation units.
// WgcCapture.cpp is compiled separately with native wchar_t enabled.
class WgcCapture {
    struct State;
    std::unique_ptr<State> state;
public:
    WgcCapture();
    ~WgcCapture();
    WgcCapture(const WgcCapture&) = delete;
    WgcCapture& operator=(const WgcCapture&) = delete;
    HRESULT Start(HWND frameWindow, ID3D11Device* device) noexcept;
    // Returned texture remains backed by a retained capture frame. Do not call
    // again while its GPU copy is pending. ReleaseCurrent only after completion.
    // All methods are called on Fusion's runtime thread.
    HRESULT LatestTexture(ID3D11Texture2D** texture, D3D11_BOX* crop) noexcept;
    void ReleaseCurrent() noexcept;
};
