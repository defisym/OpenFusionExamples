#pragma once

// The capture adapter uses the NeoPic/Fusion SDK preamble. Keep WgcCapture.cpp
// independent: it includes WgcCapture.h, never this legacy-wchar_t adapter.
#include "Common.h"

#if NEOPIC_CAPTURE_BACKEND != 0

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <cstdint>
#include "CaptureCompletion.h"
#include "CaptureBlit.h"
#if NEOPIC_CAPTURE_BACKEND == 2
#include "WgcCapture.h"
#endif

// Experimental paths deliberately do not fall back to DC or read pixels to CPU.
struct FrameCapture {
    std::unique_ptr<cSurface> snapshot;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> sourceTexture;
    CaptureCompletion completion;
    CaptureBlit blit;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
    bool benchmarking = false;
    HRESULT lastResult = S_OK;
#endif
#if NEOPIC_CAPTURE_BACKEND == 2
    bool wgcStarted = false;
    bool startAttempted = false;
#endif
#if NEOPIC_CAPTURE_BACKEND == 2
    WgcCapture wgc;
#endif

    void Report(HRESULT hr, const char* stage) {
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        if (benchmarking) return;
#endif
        char message[256];
        sprintf_s(message, "NeoPic capture backend=%d %s hr=0x%08lX\n",
            NEOPIC_CAPTURE_BACKEND, stage, static_cast<unsigned long>(hr));
        OutputDebugStringA(message);
    }

    explicit FrameCapture(LPRDATA rdPtr) { (void)rdPtr; }

    ~FrameCapture() {
        if (completion.InFlight()) {
            Report(S_OK, "teardown waiting for GPU");
            const HRESULT hr = completion.WaitForTeardown();
            if (FAILED(hr)) Report(hr, "teardown GPU failure");
        }
        ReleaseCopySource();
    }

    void ReleaseCopySource() {
        sourceTexture.Reset();
#if NEOPIC_CAPTURE_BACKEND == 2
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        if (!benchmarking)
#endif
            wgc.ReleaseCurrent();
#endif
        completion.Reset();
    }

#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
    void SetBenchmarking(bool enabled) {
        if (!enabled && benchmarking) {
            if (completion.InFlight()) completion.Wait();
            benchmarking = false;
            ReleaseCopySource();
        } else benchmarking = enabled;
    }
    HRESULT GetLastResult() const { return lastResult; }
#endif

#if NEOPIC_CAPTURE_BACKEND == 2
    HRESULT StartWgc(LPRDATA rdPtr, bool retry) {
        if (wgcStarted) return S_OK;
        if (!D3D11(rdPtr)) return E_NOTIMPL;
        const HWND frame = rdPtr->rHo.hoAdRunHeader->rhHEditWin;
        const HWND root = GetAncestor(frame, GA_ROOT);
        if (!IsWindow(frame) || !IsWindowVisible(frame) || !IsWindowVisible(root)) return E_PENDING;
        if (startAttempted && !retry) return E_PENDING;
        startAttempted = true;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        const HRESULT hr = wgc.Start(frame, static_cast<ID3D11Device*>(GetD3D11Device(rdPtr)), !benchmarking);
#else
        const HRESULT hr = wgc.Start(frame, static_cast<ID3D11Device*>(GetD3D11Device(rdPtr)));
#endif
        wgcStarted = SUCCEEDED(hr);
        Report(hr, "start");
        return hr;
    }

    // Warm the asynchronous WGC producer, but never schedule a screenshot here.
    void Update(LPRDATA rdPtr) {
        if (FAILED(StartWgc(rdPtr, false))) return;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> ignored;
        D3D11_BOX crop{};
        wgc.LatestTexture(ignored.GetAddressOf(), &crop);
    }
#endif

#if NEOPIC_CAPTURE_BACKEND == 1
    // A Surface's logical dimensions describe the valid image independently
    // of its native allocation padding and the physical window dimensions.
    HRESULT CopySecondary(LPRDATA rdPtr) {
        auto rh = rdPtr->rHo.hoAdRunHeader;
        auto frame = rh->rhFrame;
        if (!frame) return E_PENDING;
        auto source = frame->m_pSecondarySurface;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        if (!benchmarking)
#endif
        {
            const auto app = rh->rhApp;
            char state[384];
            sprintf_s(state, "NeoPic secondary state appAlwaysSecondary=%d frameAlwaysSecondary=%d source=%p oldTransitionSurface=%p layerEffectData=%d appFrameMatches=%d\n",
                app ? int(app->m_bAlwaysUseSecondarySurface) : -1,
                int(frame->m_bAlwaysUseSecondarySurface), static_cast<void*>(source),
                static_cast<void*>(app ? app->m_pOldSecondarySurface : nullptr),
                frame->m_pLayerEffects != nullptr, app && app->m_Frame == frame);
            OutputDebugStringA(state);
        }
        // Existence/type checks do not establish that this conditional runtime
        // render target contains the last fully composed child-client image.
        if (!source || !source->IsValid()) {
            Report(E_PENDING, "secondary source unavailable");
            return E_PENDING;
        }
        if (source->GetDriver() != SD_D3D11 || source->GetType() != ST_HWA_RTTEXTURE) {
            Report(E_NOTIMPL, "secondary source is not a DX11 RTT");
            return E_NOTIMPL;
        }
        RECT client{};
        if (!GetClientRect(rh->rhHEditWin, &client)) return E_FAIL;
        const int width = client.right - client.left, height = client.bottom - client.top;
        const int sourceWidth = source->GetWidth(), sourceHeight = source->GetHeight();
        POINT origin{};
        source->GetOrigin(origin);
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        if (!benchmarking)
#endif
        {
            char details[384];
            sprintf_s(details, "NeoPic secondary logical=%dx%d depth=%d type=%d origin=%ld,%ld viewport=%dx%d rhHEditWin-client=%dx%d\n",
                sourceWidth, sourceHeight, source->GetDepth(), source->GetType(), origin.x, origin.y,
                rh->rh3.rh3WindowSx, rh->rh3.rh3WindowSy, width, height);
            OutputDebugStringA(details);
        }
        if (width <= 0 || height <= 0 || sourceWidth <= 0 || sourceHeight <= 0) return E_INVALIDARG;
        // Match the DC path's size contract: consume the full valid source
        // rectangle and stretch it to the output Surface. Client, source and
        // output dimensions/aspect ratios are independent, including drag resize.
        // A pre-effect/offscreen scene is not necessarily what rhHEditWin shows.
        // Refuse cases whose final composition cannot be reconstructed here.
        if (frame->m_pEffectEx || (frame->m_pEffect && frame->m_pEffect->ffxInkEffect != 0)
            || frame->m_angle != 0 || frame->m_xScale != 1.0f || frame->m_yScale != 1.0f
            || frame->m_destPoint.x != frame->m_hotSpot.x || frame->m_destPoint.y != frame->m_hotSpot.y
            || origin.x != 0 || origin.y != 0) {
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
            if (!benchmarking)
#endif
            {
                char composition[320];
                sprintf_s(composition, "NeoPic secondary rejected frameEffect=%u effectEx=%d scale=%.4f,%.4f angle=%.4f hotspot=%ld,%ld dest=%ld,%ld\n",
                    frame->m_pEffect ? unsigned(frame->m_pEffect->ffxInkEffect) : 0u, frame->m_pEffectEx != nullptr,
                    double(frame->m_xScale), double(frame->m_yScale), double(frame->m_angle),
                    frame->m_hotSpot.x, frame->m_hotSpot.y, frame->m_destPoint.x, frame->m_destPoint.y);
                OutputDebugStringA(composition);
            }
            Report(E_NOTIMPL, "secondary cannot establish final child-client composition");
            return E_NOTIMPL;
        }
        // Capture the resource while this source RTT is active, then keep its
        // native identity before activating the destination. Do not sample a
        // Surface through mutable/current-target bookkeeping during the draw.
        Microsoft::WRL::ComPtr<ID3D11Texture2D> native;
        {
            RenderHelper reading{source, FALSE};
            const auto info = GetD3D11SurfaceInfo(source);
            if (info.m_nD3DVersion != 11 || !info.m_ppD3D11RenderTargetTexture) return E_NOINTERFACE;
            native = *static_cast<ID3D11Texture2D**>(info.m_ppD3D11RenderTargetTexture);
        }
        if (!native) return E_POINTER;
        const D3D11_BOX region{0, 0, 0, static_cast<UINT>(sourceWidth), static_cast<UINT>(sourceHeight), 1};
        return Normalize(rdPtr, native.Get(), region);
    }
#endif

    HRESULT Normalize(LPRDATA rdPtr, ID3D11Texture2D* source, const D3D11_BOX& region) {
        if (!source) return E_POINTER;
        auto rh = rdPtr->rHo.hoAdRunHeader;
        // Client pixels identify the capture AREA, not NeoPic's scene-unit size.
        const int width = rh->rh3.rh3WindowSx, height = rh->rh3.rh3WindowSy;
        if (width <= 0 || height <= 0) return E_INVALIDARG;
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        if (!benchmarking)
#endif
        {
            D3D11_TEXTURE2D_DESC input{};
            source->GetDesc(&input);
            char details[320];
            sprintf_s(details, "NeoPic capture native-source=%ux%u format=%u valid=%u,%u..%u,%u output-logical=%dx%d\n",
                input.Width, input.Height, unsigned(input.Format), region.left, region.top, region.right, region.bottom, width, height);
            OutputDebugStringA(details);
        }
        auto candidate = std::unique_ptr<cSurface>(CreateHWASurface(rdPtr, 32, width, height, ST_HWA_RTTEXTURE));
        if (!candidate || !candidate->IsValid()) return E_FAIL;
        // Mark the image as alpha-capable without initializing a CPU alpha plane;
        // the shader writes alpha=1 for the whole logical image.
        candidate->CreateAlpha();
        HRESULT rendered = E_FAIL;
        {
            constexpr RGBAREF opaqueBlack = static_cast<RGBAREF>(0xFF000000u);
            RenderHelper rendering{candidate.get(), TRUE, opaqueBlack};
            const auto info = GetD3D11SurfaceInfo(candidate.get());
            if (info.m_nD3DVersion != 11 || !info.m_pD3D11Device || !info.m_pD3D11Context
                || !info.m_ppD3D11RenderTargetView) return E_NOINTERFACE;
            auto target = *static_cast<ID3D11RenderTargetView**>(info.m_ppD3D11RenderTargetView);
            if (!target) return E_POINTER;
            auto context = static_cast<ID3D11DeviceContext*>(info.m_pD3D11Context);
            const HRESULT prepared = completion.Prepare(static_cast<ID3D11Device*>(info.m_pD3D11Device), context);
            if (FAILED(prepared)) return prepared;
            const bool resample = rh->rhApp->m_pHdr2
                && (rh->rhApp->m_pHdr2->dwOptions & AH2OPT_RESAMPLESTRETCH);
            rendered = blit.Draw(context, source, region, target, static_cast<UINT>(width), static_cast<UINT>(height), resample);
        }
        snapshot = std::move(candidate);
        sourceTexture = source;
        completion.Submit(); // Drain even a failed pass's intermediate GPU work.
        return rendered;
    }

    // Synchronous action contract: never defer publication to another Handle.
    // A ready WGC image can still be older than the current compositor frame.
    bool Capture(LPRDATA rdPtr) {
        snapshot.reset();
        HRESULT hr = E_NOTIMPL;
        if (D3D11(rdPtr)) {
#if NEOPIC_CAPTURE_BACKEND == 1
            hr = CopySecondary(rdPtr);
#else
            hr = StartWgc(rdPtr, true);
            if (SUCCEEDED(hr)) {
                Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
                D3D11_BOX crop{};
                hr = wgc.LatestTexture(texture.GetAddressOf(), &crop);
                if (SUCCEEDED(hr)) hr = Normalize(rdPtr, texture.Get(), crop);
            }
#endif
        }
        if (completion.InFlight()) {
            const HRESULT finished = completion.Wait();
            if (FAILED(finished)) hr = finished;
        }
        ReleaseCopySource();
        if (FAILED(hr)) snapshot.reset();
#if NEOPIC_CAPTURE_BENCHMARK_ENABLED
        lastResult = hr;
#endif
        Report(hr, SUCCEEDED(hr) ? "ready (synchronous GPU completion)" : "failed (image unchanged)");
        return SUCCEEDED(hr);
    }

    LPSURFACE TakeSurface() { return completion.InFlight() ? nullptr : snapshot.release(); }
};

#endif // NEOPIC_CAPTURE_BACKEND != 0
