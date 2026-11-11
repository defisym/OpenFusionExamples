#include "Macro.h"
#if NEOPIC_CAPTURE_BACKEND == 2
#include "WgcCapture.h"
#include "CaptureCrop.h"
#include <roapi.h>
#include <cstdio>
#include <dwmapi.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

#pragma comment(lib, "windowsapp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "runtimeobject.lib")
#pragma comment(lib, "dwmapi.lib")

using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;

struct WgcCapture::State {
    // Declared first so COM is uninitialized after all WinRT members are released.
    struct Apartment {
        HRESULT result = RoInitialize(RO_INIT_SINGLETHREADED);
        ~Apartment() { if (SUCCEEDED(result)) RoUninitialize(); }
    } apartment;
    HWND frameWindow = nullptr;
    HWND rootWindow = nullptr;
    IDirect3DDevice device{nullptr};
    GraphicsCaptureItem item{nullptr};
    Direct3D11CaptureFramePool pool{nullptr};
    GraphicsCaptureSession session{nullptr};
    Direct3D11CaptureFrame current{nullptr};
    SizeInt32 size{};
    ~State() {
        try { if (current) current.Close(); } catch (...) {}
        try { if (session) session.Close(); } catch (...) {}
        try { if (pool) pool.Close(); } catch (...) {}
    }
};

WgcCapture::WgcCapture() = default;
WgcCapture::~WgcCapture() = default;

static void ReportWgcFailure(HRESULT hr, const char* stage, HWND frame, HWND root) noexcept {
    char text[320];
    sprintf_s(text, "NeoPic WGC %s failed hr=0x%08lX frame=%p root=%p frameVisible=%d rootVisible=%d\n",
        stage, static_cast<unsigned long>(hr), static_cast<void*>(frame), static_cast<void*>(root),
        IsWindowVisible(frame), IsWindowVisible(root));
    OutputDebugStringA(text);
}

HRESULT WgcCapture::Start(HWND frameWindow, ID3D11Device* device) noexcept {
    const HWND root = GetAncestor(frameWindow, GA_ROOT);
    const char* stage = "initialize";
    try {
        if (!IsWindow(frameWindow) || !IsWindow(root) || !device) return E_INVALIDARG;
        if (!IsWindowVisible(frameWindow) || !IsWindowVisible(root)) return E_PENDING;
        auto next = std::make_unique<State>();
        if (FAILED(next->apartment.result) && next->apartment.result != RPC_E_CHANGED_MODE)
            return next->apartment.result;
        if (!GraphicsCaptureSession::IsSupported()) return E_NOTIMPL;
        next->frameWindow = frameWindow;
        // Capture the top-level window and crop to Fusion's child client area.
        next->rootWindow = root;
        stage = "GraphicsCaptureItem factory";
        auto factory = winrt::get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
        stage = "CreateForWindow";
        // Handle an expected initialization failure as an HRESULT, rather than
        // explicitly throwing a hresult_error at this call site in the debugger.
        const HRESULT created = factory->CreateForWindow(next->rootWindow,
            winrt::guid_of<GraphicsCaptureItem>(), winrt::put_abi(next->item));
        if (FAILED(created)) {
            ReportWgcFailure(created, stage, frameWindow, root);
            return created;
        }
        stage = "DXGI device interop";
        auto dxgi = winrt::com_ptr<ID3D11Device>{};
        dxgi.copy_from(device);
        auto dxgiDevice = dxgi.as<IDXGIDevice>();
        winrt::com_ptr<IInspectable> inspectable;
        winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put()));
        next->device = inspectable.as<IDirect3DDevice>();
        next->size = next->item.Size();
        stage = "CreateFreeThreaded frame pool";
        next->pool = Direct3D11CaptureFramePool::CreateFreeThreaded(next->device,
            DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, next->size);
        stage = "CreateCaptureSession";
        next->session = next->pool.CreateCaptureSession(next->item);
        // Default OS capture border/cursor behavior is intentionally retained.
        stage = "StartCapture";
        next->session.StartCapture();
        state = std::move(next);
        return S_OK;
    } catch (...) {
        const HRESULT hr = winrt::to_hresult();
        ReportWgcFailure(hr, stage, frameWindow, root);
        state.reset();
        return hr;
    }
}

void WgcCapture::ReleaseCurrent() noexcept {
    if (!state || !state->current) return;
    try { state->current.Close(); } catch (...) {}
    state->current = nullptr;
}

HRESULT WgcCapture::LatestTexture(ID3D11Texture2D** texture, D3D11_BOX* crop) noexcept {
    if (!texture || !crop) return E_POINTER;
    *texture = nullptr;
    *crop = {};
    try {
        if (!state) return E_UNEXPECTED;
        if (!IsWindow(state->frameWindow)) return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
        if (IsIconic(state->rootWindow)) return E_PENDING;
        // At most two queued frames per Handle; no blocking wait or busy polling.
        for (int i = 0; i < 2; ++i) {
            auto next = state->pool.TryGetNextFrame();
            if (!next) break;
            if (state->current) state->current.Close();
            state->current = next;
        }
        if (!state->current) return E_PENDING;
        const auto size = state->current.ContentSize();
        if (size.Width <= 0 || size.Height <= 0) return E_PENDING;
        if (size.Width != state->size.Width || size.Height != state->size.Height) {
            state->current.Close();
            state->current = nullptr;
            state->pool.Recreate(state->device, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);
            state->size = size;
            return E_PENDING;
        }
        auto access = state->current.Surface().as<
            ::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
        winrt::com_ptr<ID3D11Texture2D> source;
        winrt::check_hresult(access->GetInterface(__uuidof(ID3D11Texture2D), source.put_void()));

        // DWM bounds and client origin must both be physical pixels, regardless
        // of the application's DPI mode. Restore the original thread context.
        struct DpiScope {
            DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            ~DpiScope() { if (previous) SetThreadDpiAwarenessContext(previous); }
        } dpi;
        RECT bounds{}, client{};
        POINT origin{};
        winrt::check_hresult(DwmGetWindowAttribute(state->rootWindow, DWMWA_EXTENDED_FRAME_BOUNDS, &bounds, sizeof(bounds)));
        if (!GetClientRect(state->frameWindow, &client) || !ClientToScreen(state->frameWindow, &origin))
            return E_FAIL;
        neopic_capture::Crop region{};
        if (!neopic_capture::ClientCrop(bounds.left, bounds.top,
            static_cast<std::int64_t>(bounds.right) - bounds.left,
            static_cast<std::int64_t>(bounds.bottom) - bounds.top,
            origin.x, origin.y, client.right, client.bottom, size.Width, size.Height, region))
            return E_PENDING;
        *crop = {region.left, region.top, 0, region.right, region.bottom, 1};
        *texture = source.detach();
        return S_OK;
    } catch (...) { return winrt::to_hresult(); }
}
#endif
