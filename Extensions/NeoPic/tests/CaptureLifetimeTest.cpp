// Real D3D debug-layer lifetime check, without Fusion or SDK stand-ins.
#include "../CaptureBlit.h"
#include "../CaptureCompletion.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;
static void Check(HRESULT hr, const char* stage) {
    if (FAILED(hr)) throw std::runtime_error(std::string(stage) + " HRESULT=" + std::to_string(hr));
}

static std::vector<std::string> CaptureObjects(ID3D11Debug* debug, ID3D11InfoQueue* queue) {
    queue->ClearStoredMessages();
    Check(debug->ReportLiveDeviceObjects(static_cast<D3D11_RLDO_FLAGS>(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL)), "live-object report");
    std::vector<std::string> result;
    const auto count = queue->GetNumStoredMessagesAllowedByRetrievalFilter();
    for (UINT64 i = 0; i < count; ++i) {
        SIZE_T bytes = 0;
        Check(queue->GetMessage(i, nullptr, &bytes), "message size");
        std::vector<unsigned char> storage(bytes);
        auto message = reinterpret_cast<D3D11_MESSAGE*>(storage.data());
        Check(queue->GetMessage(i, message, &bytes), "message");
        const std::string text(message->pDescription, message->DescriptionByteLength);
        if (text.find("NeoPic.Capture.") != std::string::npos) result.push_back(text);
    }
    return result;
}

static void Batch(ID3D11Device* device, ID3D11DeviceContext* context,
    ID3D11Debug* debug, ID3D11InfoQueue* queue, unsigned captures) {
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = desc.Height = 16;
    desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> source, output;
    ComPtr<ID3D11RenderTargetView> view;
    Check(device->CreateTexture2D(&desc, nullptr, source.GetAddressOf()), "source");
    Check(device->CreateTexture2D(&desc, nullptr, output.GetAddressOf()), "output");
    Check(device->CreateRenderTargetView(output.Get(), nullptr, view.GetAddressOf()), "output view");
    CaptureBlit pass;
    for (unsigned i = 0; i < captures; ++i) {
        CaptureCompletion completed;
        Check(completed.Prepare(device, context), "query");
        const D3D11_BOX box{0, 0, 0, 16, 16, 1};
        Check(pass.Draw(context, source.Get(), box, view.Get(), 8 + i % 8, 8, false), "capture");
        completed.Submit();
        Check(completed.Wait(), "completion");
    }
    if (CaptureObjects(debug, queue).empty())
        throw std::runtime_error("debug names/report collection did not detect the live capture objects");
}

int main() {
    try {
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        const HRESULT created = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, nullptr, 0,
            D3D11_SDK_VERSION, device.GetAddressOf(), nullptr, context.GetAddressOf());
        if (created == DXGI_ERROR_SDK_COMPONENT_MISSING) {
            std::cout << "SKIP: D3D debug layer unavailable; capture lifetime has NOT been verified\n";
            return 0;
        }
        Check(created, "debug WARP device");
        ComPtr<ID3D11Debug> debug;
        ComPtr<ID3D11InfoQueue> queue;
        Check(device.As(&debug), "debug interface");
        Check(device.As(&queue), "info queue");
        Check(queue->PushEmptyStorageFilter(), "empty storage filter");
        Check(queue->PushEmptyRetrievalFilter(), "empty retrieval filter");
        auto verify = [&](unsigned count) {
            Batch(device.Get(), context.Get(), debug.Get(), queue.Get(), count);
            // Do not ClearState, even in this standalone test: leaked capture
            // bindings must remain observable instead of being hidden by cleanup.
            context->Flush();
            const auto remaining = CaptureObjects(debug.Get(), queue.Get());
            if (!remaining.empty()) {
                for (const auto& text : remaining) std::cerr << text << '\n';
                throw std::runtime_error("capture objects still have external references after destruction");
            }
        };
        verify(1);
        verify(32);
        for (unsigned i = 0; i < 8; ++i) verify(2);
        queue->PopRetrievalFilter();
        queue->PopStorageFilter();
        std::cout << "PASS: real debug-layer capture objects released after 1/32 captures and repeated recreation, without ClearState\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
