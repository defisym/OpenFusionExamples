// Standalone Windows test: no Fusion runtime or capture permissions required.
#include "../CaptureBlit.h"
#include "../CaptureCompletion.h"
#include <array>
#include <stdexcept>
#include <iostream>
#pragma comment(lib, "d3d11.lib")

static void Check(HRESULT hr, const char* stage) {
    if (FAILED(hr)) throw std::runtime_error(std::string(stage) + " HRESULT=" + std::to_string(hr));
}
static void Require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
int main() {
    try {
        using Microsoft::WRL::ComPtr;
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
            D3D11_SDK_VERSION, device.GetAddressOf(), nullptr, context.GetAddressOf()), "WARP device");
        std::array<unsigned char, 8 * 8 * 4> pixels{};
        const unsigned char colors[4][4]{{255,0,0,0}, {0,255,0,0}, {0,0,255,0}, {255,128,192,0}};
        for (unsigned y = 0; y < 8; ++y) for (unsigned x = 0; x < 8; ++x) {
            auto p = pixels.data() + (y * 8 + x) * 4;
            if (x < 4 && y < 4) std::memcpy(p, colors[(y / 2) * 2 + x / 2], 4);
            else { p[0] = p[1] = p[2] = 255; p[3] = 0; } // poisoned padding
        }
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = desc.Height = 8;
        desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        const D3D11_SUBRESOURCE_DATA data{pixels.data(), 8 * 4, 0};
        ComPtr<ID3D11Texture2D> source, target;
        Check(device->CreateTexture2D(&desc, &data, source.GetAddressOf()), "source");
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        Check(device->CreateTexture2D(&desc, nullptr, target.GetAddressOf()), "target");
        ComPtr<ID3D11RenderTargetView> view;
        Check(device->CreateRenderTargetView(target.Get(), nullptr, view.GetAddressOf()), "RTV");
        const D3D11_VIEWPORT oldViewport{3, 4, 20, 30, 0, 1};
        context->RSSetViewports(1, &oldViewport);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        const D3D11_BOX content{0, 0, 0, 4, 4, 1};
        CaptureBlit pass;
        CaptureCompletion completed;
        Check(completed.Prepare(device.Get(), context.Get()), "completion query");
        Check(pass.Draw(context.Get(), source.Get(), content, view.Get(), 2, 2, false), "normalize pass");
        completed.Submit();
        Check(completed.Wait(), "draw completion");
        completed.Reset();
        D3D11_VIEWPORT restored{};
        UINT count = 1;
        context->RSGetViewports(&count, &restored);
        D3D11_PRIMITIVE_TOPOLOGY topology{};
        context->IAGetPrimitiveTopology(&topology);
        Require(count == 1 && restored.TopLeftX == 3 && restored.TopLeftY == 4
            && restored.Width == 20 && restored.Height == 30
            && topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST, "Fusion pipeline state was not restored");
        auto invalid = content;
        invalid.right = 9;
        Require(FAILED(pass.Draw(context.Get(), source.Get(), invalid, view.Get(), 2, 2, false)), "invalid source region accepted");
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        ComPtr<ID3D11Texture2D> staging;
        Check(device->CreateTexture2D(&desc, nullptr, staging.GetAddressOf()), "test readback texture");
        Check(completed.Prepare(device.Get(), context.Get()), "readback query");
        context->CopyResource(staging.Get(), target.Get());
        completed.Submit();
        Check(completed.Wait(), "readback completion");
        completed.Reset();
        D3D11_MAPPED_SUBRESOURCE map{};
        Check(context->Map(staging.Get(), 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &map), "nonblocking test Map");
        bool match = true;
        for (unsigned y = 0; y < 2; ++y) for (unsigned x = 0; x < 2; ++x) {
            const auto actual = static_cast<const unsigned char*>(map.pData) + y * map.RowPitch + x * 4;
            const auto expected = colors[y * 2 + x];
            match &= actual[0] == expected[2] && actual[1] == expected[1]
                && actual[2] == expected[0] && actual[3] == 255;
        }
        const auto padding = static_cast<const unsigned char*>(map.pData) + 3 * map.RowPitch + 3 * 4;
        match &= padding[0] == 0 && padding[1] == 0 && padding[2] == 0 && padding[3] == 255;
        context->Unmap(staging.Get(), 0);
        Require(match, "logical size/crop/orientation/channel order/opaque alpha pixel mismatch");
        std::cout << "PASS: source padding, logical resize, RGBA-to-BGRA, opaque alpha, orientation, completion and state restore\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
