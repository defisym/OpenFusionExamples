#include "CaptureAdapterMocks.h"
#include "../FrameCaptureBenchmark.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// WGC's real implementation has separate native-wchar_t compiler coverage.
// This test exercises factory/ownership/derived dispatch, not capture permission.
struct WgcCapture::State {};
WgcCapture::WgcCapture() = default;
WgcCapture::~WgcCapture() = default;
HRESULT WgcCapture::Start(HWND, ID3D11Device*, bool) noexcept { return E_NOTIMPL; }
HRESULT WgcCapture::LatestTexture(ID3D11Texture2D**, D3D11_BOX*) noexcept { return E_PENDING; }
void WgcCapture::ReleaseCurrent() noexcept {}

inline int scriptedDestructions = 0;
class ScriptedCapture : public FrameCapture {
protected:
    HRESULT CaptureSurface(LPRDATA rdPtr, std::unique_ptr<cSurface>& surface) override {
        ++calls;
        ReleaseInput();
        if (calls == throwAt) throw std::runtime_error("injected capture failure");
        if (failAll || calls == failAt) return E_PENDING;
        const auto size = OutputSize(rdPtr, {320, 240});
        surface = std::make_unique<cSurface>(size.cx, size.cy);
        surface->generation = calls;
        return S_OK;
    }
    void ReleaseInput() override { ++inputReleases; }
public:
    int calls = 0, inputReleases = 0, failAt = 0, throwAt = 0;
    bool failAll = false;
    explicit ScriptedCapture(LPRDATA) : FrameCapture(CaptureBackend::Secondary) {}
    ~ScriptedCapture() override { ++scriptedDestructions; }
};

int main() {
    MockRunHeader rh;
    MockHeader2 header;
    MockApp app;
    app.m_pHdr2 = &header;
    rh.rhApp = &app;
    MockRunData rd;
    rd.rHo.hoAdRunHeader = &rh;
    for (const auto backend : {CaptureBackend::Dc, CaptureBackend::Secondary, CaptureBackend::WindowsGraphicsCapture}) {
        auto capture = CreateFrameCapture(&rd, backend);
        assert(capture->Backend() == backend);
        auto measured = CreateFrameCaptureBenchmark(&rd, backend, {2, 1});
        assert(measured->Backend() == backend);
    }
    rd.display.driver = SD_D3D9;
    for (const auto backend : {CaptureBackend::Dc, CaptureBackend::Secondary, CaptureBackend::WindowsGraphicsCapture}) {
        auto capture = CreateFrameCapture(&rd, backend);
        auto measured = CreateFrameCaptureBenchmark(&rd, backend, {2, 1});
        assert(capture->Backend() == CaptureBackend::Dc && dynamic_cast<DcFrameCapture*>(capture.get()));
        assert(measured->Backend() == CaptureBackend::Dc
            && dynamic_cast<FrameCaptureBenchmark<DcFrameCapture>*>(measured.get()));
    }
    rd.surfaceReady = false;
    {
        cSurface prototype;
        prototype.driver = SD_D3D9;
        app.m_protoLogScreen = &prototype;
        assert(CreateFrameCapture(&rd, CaptureBackend::Secondary)->Backend() == CaptureBackend::Dc);
        assert(CreateFrameCaptureBenchmark(&rd, CaptureBackend::WindowsGraphicsCapture)->Backend() == CaptureBackend::Dc);
        app.m_protoLogScreen = nullptr;
        // Unknown readiness is not proof of DX9; do not guess a different mode.
        assert(CreateFrameCapture(&rd, CaptureBackend::Secondary)->Backend() == CaptureBackend::Secondary);
    }
    rd.surfaceReady = true;
    rd.display.driver = 0; // Other renderer modes retain the existing failure policy.
    assert(CreateFrameCapture(&rd, CaptureBackend::WindowsGraphicsCapture)->Backend() == CaptureBackend::WindowsGraphicsCapture);
    rd.display.driver = SD_D3D11;
    assert(CreateFrameCapture(&rd, CaptureBackend::Secondary)->Backend() == CaptureBackend::Secondary);
    std::cout << "PASS: DX9 normal/benchmark fallback to DC, prototype readiness and DX11 selection\n";
    {
        auto capture = std::make_unique<ScriptedCapture>(&rd);
        FrameCapture* base = capture.get();
        assert(base->CaptureAndLoad(&rd) == S_OK);
        assert(capture->calls == 1 && capture->inputReleases == 1 && mockHandoffs == 1);
    }
    mockPublished.reset();
    mockHandoffs = 0;
    {
        auto capture = std::make_unique<FrameCaptureBenchmark<ScriptedCapture>>(&rd, CaptureBenchmarkOptions{4, 2});
        capture->failAt = 3;
        std::unique_ptr<FrameCapture> base = std::move(capture);
        SIZE output{};
        assert(base->CaptureAndLoad(&rd, &output) == S_OK);
        const auto derived = static_cast<FrameCaptureBenchmark<ScriptedCapture>*>(base.get());
        assert(derived->calls == 6 && derived->inputReleases == 1 && mockHandoffs == 5);
        assert(mockOwnedHandoff && mockPublished->generation == 6 && mockSurfaceCount == 1);
        assert(output.cx == 640 && output.cy == 480);
        derived->failAll = true;
        const int published = mockHandoffs;
        assert(FAILED(base->CaptureAndLoad(&rd)));
        assert(derived->calls == 8 && derived->inputReleases == 2 && mockHandoffs == published);
        assert(mockPublished->generation == 6); // Failed warmup preserves the old snapshot.
    }
    {
        FrameCaptureBenchmark<ScriptedCapture> capture(&rd, {2, 2});
        capture.throwAt = 2;
        bool caught = false;
        try { capture.CaptureAndLoad(&rd); } catch (const std::runtime_error&) { caught = true; }
        assert(caught && capture.calls == 2 && capture.inputReleases == 1);
        capture.throwAt = 0;
        assert(capture.CaptureAndLoad(&rd) == S_OK);
        assert(capture.calls == 6 && capture.inputReleases == 2); // Session state recovered.
    }
    assert(scriptedDestructions == 3);
    mockPublished.reset();
    assert(mockSurfaceCount == 0);
    std::cout << "PASS: backend factory, virtual single/benchmark dispatch, owned handoff, failure and exception cleanup\n";
}
