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
#if NEOPIC_CAPTURE_BACKEND == 2
#include "WgcCapture.h"
#endif

// Experimental paths deliberately do not fall back to DC or read pixels to CPU.
struct FrameCapture {
    std::unique_ptr<cSurface> snapshot;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> sourceTexture;
    CaptureCompletion completion;
#if NEOPIC_CAPTURE_BACKEND == 2
    bool wgcStarted = false;
    bool startAttempted = false;
#endif
#if NEOPIC_CAPTURE_BACKEND == 2
    WgcCapture wgc;
#endif

    static void Report(HRESULT hr, const char* stage) {
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
        wgc.ReleaseCurrent();
#endif
        completion.Reset();
    }

#if NEOPIC_CAPTURE_BACKEND == 2
    HRESULT StartWgc(LPRDATA rdPtr, bool retry) {
        if (wgcStarted) return S_OK;
        if (!D3D11(rdPtr)) return E_NOTIMPL;
        const HWND frame = rdPtr->rHo.hoAdRunHeader->rhHEditWin;
        const HWND root = GetAncestor(frame, GA_ROOT);
        if (!IsWindow(frame) || !IsWindowVisible(frame) || !IsWindowVisible(root)) return E_PENDING;
        if (startAttempted && !retry) return E_PENDING;
        startAttempted = true;
        const HRESULT hr = wgc.Start(frame, static_cast<ID3D11Device*>(GetD3D11Device(rdPtr)));
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
    // Secondary is a Fusion Surface, not a raw texture. Let Fusion's GPU blit
    // interpret its type, logical dimensions, UV/padding and pixel format.
    HRESULT CopySecondary(LPRDATA rdPtr) {
        auto rh = rdPtr->rHo.hoAdRunHeader;
        auto frame = rh->rhFrame;
        auto source = frame->m_pSecondarySurface;
        if (!source || !source->IsValid()) return E_PENDING;
        if (source->GetDriver() != SD_D3D11 || source->GetType() != ST_HWA_RTTEXTURE) return E_NOTIMPL;
        RECT client{};
        if (!GetClientRect(rh->rhHEditWin, &client)) return E_FAIL;
        const int width = client.right - client.left, height = client.bottom - client.top;
        const int sourceWidth = source->GetWidth(), sourceHeight = source->GetHeight();
        POINT origin{};
        source->GetOrigin(origin);
        char details[384];
        sprintf_s(details, "NeoPic secondary logical=%dx%d depth=%d type=%d origin=%ld,%ld viewport=%dx%d rhHEditWin-client=%dx%d\n",
            sourceWidth, sourceHeight, source->GetDepth(), source->GetType(), origin.x, origin.y,
            rh->rh3.rh3WindowSx, rh->rh3.rh3WindowSy, width, height);
        OutputDebugStringA(details);
        if (width <= 0 || height <= 0 || sourceWidth <= 0 || sourceHeight <= 0) return E_INVALIDARG;
        // A pre-effect/offscreen scene is not necessarily what rhHEditWin shows.
        // Refuse cases whose final composition cannot be reconstructed here.
        if (frame->m_pEffectEx || (frame->m_pEffect && frame->m_pEffect->ffxInkEffect != 0)
            || frame->m_angle != 0 || frame->m_xScale != 1.0f || frame->m_yScale != 1.0f
            || frame->m_destPoint.x != frame->m_hotSpot.x || frame->m_destPoint.y != frame->m_hotSpot.y
            || origin.x != 0 || origin.y != 0
            || sourceWidth != rh->rh3.rh3WindowSx || sourceHeight != rh->rh3.rh3WindowSy
            || static_cast<std::int64_t>(width) * sourceHeight != static_cast<std::int64_t>(height) * sourceWidth) {
            char composition[320];
            sprintf_s(composition, "NeoPic secondary rejected frameEffect=%u effectEx=%d scale=%.4f,%.4f angle=%.4f hotspot=%ld,%ld dest=%ld,%ld\n",
                frame->m_pEffect ? unsigned(frame->m_pEffect->ffxInkEffect) : 0u, frame->m_pEffectEx != nullptr,
                double(frame->m_xScale), double(frame->m_yScale), double(frame->m_angle),
                frame->m_hotSpot.x, frame->m_hotSpot.y, frame->m_destPoint.x, frame->m_destPoint.y);
            OutputDebugStringA(composition);
            Report(E_NOTIMPL, "secondary cannot establish final child-client composition");
            return E_NOTIMPL;
        }
        auto candidate = std::unique_ptr<cSurface>(CreateHWASurface(rdPtr, 32, width, height, ST_HWA_RTTEXTURE));
        if (!candidate || !candidate->IsValid()) return E_FAIL;
        BOOL drawn = FALSE;
        {
            // COLORREFATORGBA(0, 255) is a signed int expression in the SDK;
            // brace initialization into RGBAREF rejects its narrowing conversion.
            constexpr RGBAREF opaqueBlack = static_cast<RGBAREF>(0xFF000000u);
            RenderHelper rendering{candidate.get(), TRUE, opaqueBlack};
            const auto info = GetD3D11SurfaceInfo(candidate.get());
            if (info.m_nD3DVersion != 11 || !info.m_pD3D11Device || !info.m_pD3D11Context) return E_NOINTERFACE;
            const HRESULT hr = completion.Prepare(static_cast<ID3D11Device*>(info.m_pD3D11Device),
                static_cast<ID3D11DeviceContext*>(info.m_pD3D11Context));
            if (FAILED(hr)) return hr;
            POINT center{0, 0};
            const DWORD stretchFlags = (rh->rhApp->m_pHdr2
                && (rh->rhApp->m_pHdr2->dwOptions & AH2OPT_RESAMPLESTRETCH)) ? STRF_RESAMPLE : 0;
            drawn = source->BlitEx(*candidate, 0.0f, 0.0f,
                static_cast<float>(width) / sourceWidth, static_cast<float>(height) / sourceHeight,
                0, 0, sourceWidth, sourceHeight, &center, 0.0f, BMODE_OPAQUE, BOP_COPY, 0, stretchFlags);
        }
        snapshot = std::move(candidate);
        completion.Submit(); // even failed blits may have submitted GPU work
        return drawn ? S_OK : E_FAIL;
    }

#endif

#if NEOPIC_CAPTURE_BACKEND == 2
    // Texture formats/sample counts must agree; CopyResource cannot resize or
    // convert. The WGC path supplies a client-area crop in source coordinates.
    HRESULT CopyTexture(LPRDATA rdPtr, ID3D11Texture2D* source, const D3D11_BOX* crop) {
        if (!source) return E_POINTER;
        D3D11_TEXTURE2D_DESC from{};
        source->GetDesc(&from);
        if (from.MipLevels != 1 || from.ArraySize != 1) return E_NOTIMPL;
        const UINT width = crop ? crop->right - crop->left : from.Width;
        const UINT height = crop ? crop->bottom - crop->top : from.Height;
        if (!width || !height || (crop && (crop->left >= crop->right || crop->top >= crop->bottom
            || crop->front != 0 || crop->back != 1 || crop->right > from.Width || crop->bottom > from.Height)))
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
            char details[256];
            sprintf_s(details, "NeoPic WGC native-source=%ux%u format=%u client=%ux%u native-target=%ux%u format=%u\n",
                from.Width, from.Height, unsigned(from.Format), width, height, to.Width, to.Height, unsigned(to.Format));
            OutputDebugStringA(details);
            if (from.Format != to.Format || to.MipLevels != 1 || to.ArraySize != 1
                || to.Width < width || to.Height < height || to.SampleDesc.Count != 1)
                return E_NOTIMPL;
            auto context = static_cast<ID3D11DeviceContext*>(info.m_pD3D11Context);
            if (from.SampleDesc.Count > 1) {
                if (crop || from.Width != to.Width || from.Height != to.Height) return E_NOTIMPL;
                UINT support = 0;
                const auto hr = sourceDevice->CheckFormatSupport(from.Format, &support);
                if (FAILED(hr) || !(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)) return E_NOTIMPL;
            }
            // Query allocation can fail: do it before submitting any GPU copy.
            const HRESULT prepared = completion.Prepare(sourceDevice.Get(), context);
            if (FAILED(prepared)) return prepared;
            if (from.SampleDesc.Count > 1) {
                context->ResolveSubresource(target, 0, source, 0, from.Format);
            } else {
                context->CopySubresourceRegion(target, 0, 0, 0, 0, source, 0, crop);
            }
        }
        snapshot = std::move(candidate);
        sourceTexture = source;
        completion.Submit();
        Report(S_OK, "submitted (GPU pending)");
        return S_OK;
    }

#endif

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
                if (SUCCEEDED(hr)) hr = CopyTexture(rdPtr, texture.Get(), &crop);
            }
#endif
        }
        if (completion.InFlight()) {
            const HRESULT finished = completion.Wait();
            if (FAILED(finished)) hr = finished;
        }
        ReleaseCopySource();
        if (FAILED(hr)) snapshot.reset();
        Report(hr, SUCCEEDED(hr) ? "ready (synchronous GPU completion)" : "failed (image unchanged)");
        return SUCCEEDED(hr);
    }

    LPSURFACE TakeSurface() { return completion.InFlight() ? nullptr : snapshot.release(); }
};

#endif // NEOPIC_CAPTURE_BACKEND != 0
