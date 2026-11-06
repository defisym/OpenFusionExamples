#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#if NEOPIC_CAPTURE_BACKEND == 2
#include "WgcCapture.h"
#endif

// Experimental paths deliberately do not fall back to DC or read pixels to CPU.
struct FrameCapture {
    std::unique_ptr<cSurface> snapshot;
    bool pending = false;
    int requestedLoop = 0;
    int attempts = 0;
#if NEOPIC_CAPTURE_BACKEND == 2
    WgcCapture wgc;
#endif

    static void Report(HRESULT hr, const char* stage) {
        char message[256];
        sprintf_s(message, "NeoPic capture backend=%d %s hr=0x%08lX\n",
            NEOPIC_CAPTURE_BACKEND, stage, static_cast<unsigned long>(hr));
        OutputDebugStringA(message);
    }

    explicit FrameCapture(LPRDATA rdPtr) {
#if NEOPIC_CAPTURE_BACKEND == 2
        if (D3D11(rdPtr)) {
            auto device = static_cast<ID3D11Device*>(GetD3D11Device(rdPtr));
            Report(wgc.Start(rdPtr->rHo.hoAdRunHeader->rhHEditWin, device), "start");
        } else Report(E_NOTIMPL, "DX11-required");
#else
        (void)rdPtr;
#endif
    }

    void Request(LPRDATA rdPtr) {
        if (pending) return;
        pending = true;
        attempts = 0;
        requestedLoop = rdPtr->rHo.hoAdRunHeader->rhLoopCount;
    }

    // Texture formats/sample counts must agree; CopyResource cannot resize or
    // convert. The WGC path supplies a client-area crop in source coordinates.
    HRESULT CopyTexture(LPRDATA rdPtr, ID3D11Texture2D* source, const D3D11_BOX* crop) {
        if (!source) return E_POINTER;
        D3D11_TEXTURE2D_DESC from{};
        source->GetDesc(&from);
        if (from.MipLevels != 1 || from.ArraySize != 1) return E_NOTIMPL;
        const UINT width = crop ? crop->right - crop->left : from.Width;
        const UINT height = crop ? crop->bottom - crop->top : from.Height;
        if (!width || !height || (crop && (crop->right > from.Width || crop->bottom > from.Height)))
            return E_INVALIDARG;
        auto candidate = std::unique_ptr<cSurface>(CreateHWASurface(rdPtr,
            32, static_cast<int>(width), static_cast<int>(height), ST_HWA_RTTEXTURE));
        if (!candidate || !candidate->IsValid()) return E_FAIL;
        // Fusion.h matrix: RTT native render-target texture is valid only while
        // rendering; its m_ppD3D11Texture must not be dereferenced.
        {
            RenderHelper rendering{candidate.get(), FALSE};
            const auto info = GetD3D11SurfaceInfo(candidate.get());
            if (info.m_nD3DVersion != 11 || !info.m_ppD3D11RenderTargetTexture
                || !info.m_pD3D11Context) return E_NOINTERFACE;
            auto target = *static_cast<ID3D11Texture2D**>(info.m_ppD3D11RenderTargetTexture);
            if (!target) return E_POINTER;
            Microsoft::WRL::ComPtr<ID3D11Device> sourceDevice;
            source->GetDevice(&sourceDevice);
            if (sourceDevice.Get() != info.m_pD3D11Device) return E_INVALIDARG;
            D3D11_TEXTURE2D_DESC to{};
            target->GetDesc(&to);
            if (from.Format != to.Format || to.MipLevels != 1 || to.ArraySize != 1
                || to.Width != width || to.Height != height || to.SampleDesc.Count != 1)
                return E_NOTIMPL;
            auto context = static_cast<ID3D11DeviceContext*>(info.m_pD3D11Context);
            if (from.SampleDesc.Count > 1) {
                if (crop) return E_NOTIMPL;
                UINT support = 0;
                const auto hr = sourceDevice->CheckFormatSupport(from.Format, &support);
                if (FAILED(hr) || !(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)) return E_NOTIMPL;
                context->ResolveSubresource(target, 0, source, 0, from.Format);
            } else {
                context->CopySubresourceRegion(target, 0, 0, 0, 0, source, 0, crop);
            }
        }
        snapshot = std::move(candidate);
        return S_OK;
    }

    bool Update(LPRDATA rdPtr) {
        using Microsoft::WRL::ComPtr;
        auto rh = rdPtr->rHo.hoAdRunHeader;
        ComPtr<ID3D11Texture2D> texture;
        D3D11_BOX crop{};
        HRESULT hr = E_PENDING;
#if NEOPIC_CAPTURE_BACKEND == 2
        // Drain WGC every Handle, even without a request, to avoid a startup frame
        // sitting in the pool. WgcCapture retains the frame through CopyTexture.
        hr = wgc.LatestTexture(texture.GetAddressOf(), &crop);
#endif
        if (!pending || rh->rhLoopCount <= requestedLoop) return false;
        if (!D3D11(rdPtr)) hr = E_NOTIMPL;
        else {
#if NEOPIC_CAPTURE_BACKEND == 1
            // Do not use WinGetSurface as the source: it can be a partial layer.
            auto sf = rh->rhFrame->m_pSecondarySurface;
            if (sf && sf->IsValid() && sf->GetType() == ST_HWA_RTTEXTURE) {
                {
                    RenderHelper rendering{sf, FALSE};
                    const auto info = GetD3D11SurfaceInfo(sf);
                    if (info.m_nD3DVersion == 11 && info.m_ppD3D11RenderTargetTexture)
                        texture = *static_cast<ID3D11Texture2D**>(info.m_ppD3D11RenderTargetTexture);
                }
                hr = CopyTexture(rdPtr, texture.Get(), nullptr);
            } // A missing secondary is an expected test result; never force flags.
#else
            if (SUCCEEDED(hr)) hr = CopyTexture(rdPtr, texture.Get(), &crop);
#endif
        }
        if (hr == E_PENDING && ++attempts < 120) return false;
        pending = false;
        Report(hr, SUCCEEDED(hr) ? "ready" : "failed (image unchanged)");
        return SUCCEEDED(hr);
    }

    LPSURFACE TakeSurface() { return snapshot.release(); }
};
