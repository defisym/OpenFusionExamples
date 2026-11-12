#pragma once

// Native WGC implementation remains isolated in its native-wchar_t .cpp.
#include "Common.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "CaptureCompletion.h"
#include "CaptureBlit.h"
#include "WgcCapture.h"

class Dx11FrameCapture : public FrameCapture {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> sourceTexture;
    CaptureCompletion completion;
    CaptureBlit blit;
protected:
    using Texture = Microsoft::WRL::ComPtr<ID3D11Texture2D>;
    explicit Dx11FrameCapture(CaptureBackend backend) : FrameCapture(backend) {}
    virtual HRESULT ReadSource(LPRDATA rdPtr, Texture& texture, D3D11_BOX& region) = 0;

    // Borrowed source frames must outlive their submitted GPU work.
    void Drain() {
        if (completion.InFlight()) {
            const HRESULT hr = completion.WaitForTeardown();
            if (FAILED(hr)) Report(hr, "teardown GPU failure");
        }
        sourceTexture.Reset();
        completion.Reset();
        ReleaseInput();
    }

    HRESULT CaptureSurface(LPRDATA rdPtr, std::unique_ptr<cSurface>& output) override {
        HRESULT hr = E_NOTIMPL;
        if (D3D11(rdPtr)) {
            Texture texture;
            D3D11_BOX region{};
            hr = ReadSource(rdPtr, texture, region);
            if (SUCCEEDED(hr)) hr = Normalize(rdPtr, texture.Get(), region, output);
        }
        if (completion.InFlight()) {
            const HRESULT finished = completion.Wait();
            if (FAILED(finished)) hr = finished;
        }
        sourceTexture.Reset();
        completion.Reset();
        ReleaseInput();
        if (FAILED(hr)) output.reset();
        return hr;
    }

    HRESULT Normalize(LPRDATA rdPtr, ID3D11Texture2D* source, const D3D11_BOX& region,
        std::unique_ptr<cSurface>& output) {
        if (!source) return E_POINTER;
        auto rh = rdPtr->rHo.hoAdRunHeader;
        // Client pixels identify the capture AREA, not NeoPic's scene-unit size.
        const auto size = OutputSize(rdPtr, {static_cast<LONG>(region.right - region.left), static_cast<LONG>(region.bottom - region.top)});
        const int width = size.cx, height = size.cy;
        if (width <= 0 || height <= 0) return E_INVALIDARG;
        if (DiagnosticsEnabled())
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
        output = std::move(candidate);
        sourceTexture = source;
        completion.Submit(); // Drain even a failed pass's intermediate GPU work.
        return rendered;
    }

public:
    ~Dx11FrameCapture() override { Drain(); }
};

class SecondaryFrameCapture : public Dx11FrameCapture {
protected:
    // A Surface's logical dimensions describe the valid image independently
    // of its native allocation padding and the physical window dimensions.
    HRESULT ReadSource(LPRDATA rdPtr, Texture& native, D3D11_BOX& region) override {
        auto rh = rdPtr->rHo.hoAdRunHeader;
        auto frame = rh->rhFrame;
        if (!frame) return E_PENDING;
        auto source = frame->m_pSecondarySurface;
        if (DiagnosticsEnabled())
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
        if (DiagnosticsEnabled())
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
            if (DiagnosticsEnabled())
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
        {
            RenderHelper reading{source, FALSE};
            const auto info = GetD3D11SurfaceInfo(source);
            if (info.m_nD3DVersion != 11 || !info.m_ppD3D11RenderTargetTexture) return E_NOINTERFACE;
            native = *static_cast<ID3D11Texture2D**>(info.m_ppD3D11RenderTargetTexture);
        }
        if (!native) return E_POINTER;
        region = {0, 0, 0, static_cast<UINT>(sourceWidth), static_cast<UINT>(sourceHeight), 1};
        return S_OK;
    }
public:
    explicit SecondaryFrameCapture(LPRDATA rdPtr) : Dx11FrameCapture(CaptureBackend::Secondary) { (void)rdPtr; }
};

class WgcFrameCapture : public Dx11FrameCapture {
    bool started = false;
    bool startAttempted = false;
    WgcCapture wgc;

    HRESULT Start(LPRDATA rdPtr, bool retry) {
        if (started) return S_OK;
        if (!D3D11(rdPtr)) return E_NOTIMPL;
        const HWND frame = rdPtr->rHo.hoAdRunHeader->rhHEditWin;
        const HWND root = GetAncestor(frame, GA_ROOT);
        if (!IsWindow(frame) || !IsWindowVisible(frame) || !IsWindowVisible(root)) return E_PENDING;
        if (startAttempted && !retry) return E_PENDING;
        startAttempted = true;
        const HRESULT hr = wgc.Start(frame, static_cast<ID3D11Device*>(GetD3D11Device(rdPtr)), DiagnosticsEnabled());
        started = SUCCEEDED(hr);
        Report(hr, "start");
        return hr;
    }
protected:
    HRESULT ReadSource(LPRDATA rdPtr, Texture& texture, D3D11_BOX& region) override {
        const HRESULT hr = Start(rdPtr, true);
        return FAILED(hr) ? hr : wgc.LatestTexture(texture.GetAddressOf(), &region);
    }
    void ReleaseInput() override { wgc.ReleaseCurrent(); }
public:
    explicit WgcFrameCapture(LPRDATA rdPtr) : Dx11FrameCapture(CaptureBackend::WindowsGraphicsCapture) { (void)rdPtr; }
    ~WgcFrameCapture() override { Drain(); } // Drain before the WGC member is destroyed.
    void Update(LPRDATA rdPtr) override {
        if (FAILED(Start(rdPtr, false))) return;
        Texture ignored;
        D3D11_BOX region{};
        wgc.LatestTexture(ignored.GetAddressOf(), &region);
    }
};
