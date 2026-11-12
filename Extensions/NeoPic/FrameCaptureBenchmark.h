#pragma once

// Optional example. Production sources do not include this header.
#include "FrameCaptureDC.h"
#include "FrameCaptureDX11.h"
#include "CaptureBenchmark.h"
#include <type_traits>

struct CaptureBenchmarkOptions {
    std::size_t iterations = 100;
    std::size_t warmup = 10;
};

template<class Base>
class FrameCaptureBenchmark : public Base {
    CaptureBenchmarkOptions options;
    bool running = false;

    void Stop() {
        running = false;
        Base::ReleaseInput();
    }

    HRESULT WaitForUpload(LPRDATA rdPtr) {
        if constexpr (std::is_base_of_v<DcFrameCapture, Base>) {
            if (rdPtr->HWA && D3D11(rdPtr)) {
                const auto info = GetD3D11Info(rdPtr);
                CaptureCompletion completed;
                const HRESULT hr = completed.Prepare(static_cast<ID3D11Device*>(info.m_pD3D11Device),
                    static_cast<ID3D11DeviceContext*>(info.m_pD3D11Context));
                if (FAILED(hr)) return hr;
                completed.Submit();
                return completed.Wait();
            }
        }
        return S_OK;
    }

    void Print(LPRDATA rdPtr, const neopic_capture::BenchmarkResult& result, SIZE output,
        HRESULT firstFailure) const {
        const auto rh = rdPtr->rHo.hoAdRunHeader;
        RECT client{};
        GetClientRect(rh->rhHEditWin, &client);
        const bool resample = rh->rhApp->m_pHdr2 && (rh->rhApp->m_pHdr2->dwOptions & AH2OPT_RESAMPLESTRETCH);
        char text[1800];
        sprintf_s(text, "NeoPic capture benchmark backend=%u iterations=%llu warmup=%llu HWA=%d library=%d viewport=%dx%d client=%ldx%ld output=%ldx%ld resample=%d\n"
            "warmup_ok=%llu warmup_failed=%llu first_ms=%.6f first_ok=%d first_failure_hr=0x%08lX\n"
            "attempted=%llu ok=%llu failed=%llu total_ms=%.3f status=%s\n",
            static_cast<unsigned>(this->Backend()), static_cast<unsigned long long>(options.iterations),
            static_cast<unsigned long long>(options.warmup), rdPtr->HWA ? 1 : 0, rdPtr->isLib ? 1 : 0,
            rh->rh3.rh3WindowSx, rh->rh3.rh3WindowSy, client.right - client.left, client.bottom - client.top,
            output.cx, output.cy, resample ? 1 : 0,
            static_cast<unsigned long long>(result.warmupSuccess), static_cast<unsigned long long>(result.warmupFailure),
            result.firstMs, result.firstSuccess ? 1 : 0, static_cast<unsigned long>(firstFailure),
            static_cast<unsigned long long>(result.success.count + result.failure.count),
            static_cast<unsigned long long>(result.success.count), static_cast<unsigned long long>(result.failure.count),
            result.totalMs, result.noReadySource ? "no_ready_source" : (result.success.count ? (result.failure.count ? "partial" : "ok") : "all_failed"));
        OutputDebugStringA(text);
        if (result.success.count) {
            sprintf_s(text, "success mean_ms=%.6f min_ms=%.6f median_ms=%.6f p95_ms=%.6f max_ms=%.6f copies_per_s=%.2f\n",
                result.success.mean, result.success.minimum, result.success.median, result.success.p95, result.success.maximum,
                result.totalMs > 0 ? static_cast<double>(result.success.count) * 1000.0 / result.totalMs : 0.0);
            OutputDebugStringA(text);
        } else OutputDebugStringA("success timing: n/a (no successful measured capture)\n");
        if (result.failure.count) {
            sprintf_s(text, "failure mean_ms=%.6f (excluded from success percentiles)\n", result.failure.mean);
            OutputDebugStringA(text);
        }
    }
protected:
    bool DiagnosticsEnabled() const override { return !running; }
    SIZE OutputSize(LPRDATA rdPtr, SIZE) const override {
        const auto rh = rdPtr->rHo.hoAdRunHeader;
        return {rh->rh3.rh3WindowSx, rh->rh3.rh3WindowSy};
    }
    void ReleaseInput() override {
        // During a batch, WGC may reuse its already-ready frame between copies.
        if (!running) Base::ReleaseInput();
    }
    HRESULT CaptureSurface(LPRDATA rdPtr, std::unique_ptr<cSurface>& output) override {
        if constexpr (std::is_base_of_v<DcFrameCapture, Base>) {
            const HRESULT hr = this->CopyBitmap(rdPtr);
            if (FAILED(hr)) return hr;
            if (!GdiFlush()) return this->LastWin32Error();
            return this->CloneBitmap(rdPtr, output);
        } else return Base::CaptureSurface(rdPtr, output);
    }
public:
    explicit FrameCaptureBenchmark(LPRDATA rdPtr, CaptureBenchmarkOptions value = {}) : Base(rdPtr), options(value) {
        if (!options.iterations || options.iterations > 100000 || options.warmup > 10000)
            throw std::invalid_argument("capture benchmark count");
    }

    HRESULT CaptureAndLoad(LPRDATA rdPtr, SIZE* outputSize = nullptr) override {
        // A nested capture remains a single operation, not another benchmark.
        if (running) return Base::CaptureAndLoad(rdPtr, outputSize);
        LARGE_INTEGER frequency{};
        if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0) {
            OutputDebugStringA("NeoPic capture benchmark: QPC unavailable; doing one capture\n");
            return Base::CaptureAndLoad(rdPtr, outputSize);
        }
        auto now = []() { LARGE_INTEGER ticks{}; QueryPerformanceCounter(&ticks); return ticks.QuadPart; };
        SIZE output{};
        HRESULT firstFailure = S_OK;
        auto capture = [&]() {
            HRESULT hr = Base::CaptureAndLoad(rdPtr, &output);
            if (SUCCEEDED(hr)) hr = WaitForUpload(rdPtr);
            if (FAILED(hr) && firstFailure == S_OK) firstFailure = hr;
            return SUCCEEDED(hr);
        };
        neopic_capture::BenchmarkResult result;
        {
            struct Session {
                FrameCaptureBenchmark& owner;
                explicit Session(FrameCaptureBenchmark& value) : owner(value) { owner.running = true; }
                ~Session() { owner.Stop(); }
            } session(*this);
            result = neopic_capture::RunBenchmark(options.iterations, options.warmup, capture, now, frequency.QuadPart);
        }
        if (outputSize) *outputSize = output;
        Print(rdPtr, result, output, firstFailure);
        return result.success.count ? S_OK : (FAILED(firstFailure) ? firstFailure : E_FAIL);
    }
};

inline std::unique_ptr<FrameCapture> CreateFrameCaptureBenchmark(LPRDATA rdPtr, CaptureBackend backend,
    CaptureBenchmarkOptions options = {}) {
    switch (backend) {
    case CaptureBackend::Secondary: return std::make_unique<FrameCaptureBenchmark<SecondaryFrameCapture>>(rdPtr, options);
    case CaptureBackend::WindowsGraphicsCapture: return std::make_unique<FrameCaptureBenchmark<WgcFrameCapture>>(rdPtr, options);
    default: return std::make_unique<FrameCaptureBenchmark<DcFrameCapture>>(rdPtr, options);
    }
}
