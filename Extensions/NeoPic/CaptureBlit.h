#pragma once
#include <d3d11_1.h>
#include <wrl/client.h>
#include <tuple>
#include <cstring>
#include "D3DUtilities/ShaderCompiler.h"

// Normalize a native RGB capture into a Fusion RTT: crop valid source pixels,
// resize to scene coordinates, convert channel layout and write opaque alpha.
// Reuse the repository shader compiler; isolate all changed pipeline state.
class CaptureBlit {
    template<class T> using Ptr = Microsoft::WRL::ComPtr<T>;
    Ptr<ID3D11Device> owner;
    Ptr<ID3DDeviceContextState> scratch;
    Ptr<ID3D11VertexShader> vertex;
    Ptr<ID3D11PixelShader> pixel;
    Ptr<ID3D11SamplerState> pointSampler, linearSampler;
    Ptr<ID3D11RasterizerState> rasterizer;
    Ptr<ID3D11BlendState> blend;
    Ptr<ID3D11DepthStencilState> depth;
    Ptr<ID3D11Texture2D> sampleTexture, resolved;
    Ptr<ID3D11ShaderResourceView> sampleView;

    HRESULT Initialize(ID3D11Device* device) {
        if (owner.Get() == device && scratch) return S_OK;
        owner.Reset();
        scratch.Reset();
        Ptr<ID3D11Device1> device1;
        HRESULT hr = device->QueryInterface(IID_PPV_ARGS(device1.GetAddressOf()));
        if (FAILED(hr)) return hr;
        const D3D_FEATURE_LEVEL level = device->GetFeatureLevel();
        D3D_FEATURE_LEVEL selected{};
        Ptr<ID3DDeviceContextState> nextState;
        hr = device1->CreateDeviceContextState(0, &level, 1, D3D11_SDK_VERSION,
            __uuidof(ID3D11Device), &selected, nextState.GetAddressOf());
        if (FAILED(hr)) return hr;
        constexpr const char* code = R"(
struct V { float4 p : SV_Position; float2 uv : TEXCOORD0; };
V VS(uint id : SV_VertexID) {
    V v; float2 uv = float2((id << 1) & 2, id & 2);
    v.p = float4(uv.x * 2 - 1, 1 - uv.y * 2, 0, 1); v.uv = uv; return v;
}
Texture2D<float4> image : register(t0);
SamplerState imageSampler : register(s0);
float4 PS(V v) : SV_Target { return float4(image.Sample(imageSampler, v.uv).rgb, 1); }
)";
        ShaderCompiler compiler(device);
        ShaderCompiler::Blob vs, ps;
        if (!compiler.CompileShader(code, std::strlen(code), "VS", "vs_4_0", vs)
            || !compiler.CompileShader(code, std::strlen(code), "PS", "ps_4_0", ps)) {
            OutputDebugStringA(compiler.GetLastError().c_str());
            return E_FAIL;
        }
        hr = device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, vertex.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        hr = device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, pixel.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        D3D11_SAMPLER_DESC sampler{};
        sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler.MaxLOD = D3D11_FLOAT32_MAX;
        sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        hr = device->CreateSamplerState(&sampler, pointSampler.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        hr = device->CreateSamplerState(&sampler, linearSampler.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        D3D11_RASTERIZER_DESC rs{};
        rs.FillMode = D3D11_FILL_SOLID; rs.CullMode = D3D11_CULL_NONE; rs.DepthClipEnable = TRUE;
        hr = device->CreateRasterizerState(&rs, rasterizer.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        D3D11_BLEND_DESC bs{};
        auto& rt = bs.RenderTarget[0];
        rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        rt.SrcBlend = rt.SrcBlendAlpha = D3D11_BLEND_ONE;
        rt.DestBlend = rt.DestBlendAlpha = D3D11_BLEND_ZERO;
        rt.BlendOp = rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        hr = device->CreateBlendState(&bs, blend.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        D3D11_DEPTH_STENCIL_DESC ds{};
        ds.DepthFunc = D3D11_COMPARISON_ALWAYS;
        ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        ds.FrontFace.StencilFailOp = ds.FrontFace.StencilDepthFailOp = ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        ds.BackFace = ds.FrontFace;
        hr = device->CreateDepthStencilState(&ds, depth.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        owner = device;
        scratch = nextState;
        return S_OK;
    }

    static DXGI_FORMAT SampleFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_UNORM;
        default: return DXGI_FORMAT_UNKNOWN; // no implicit YUV/HDR conversion
        }
    }
public:
    HRESULT Draw(ID3D11DeviceContext* context, ID3D11Texture2D* source,
        const D3D11_BOX& box, ID3D11RenderTargetView* destination,
        UINT width, UINT height, bool resample) {
        if (!context || !source || !destination || !width || !height) return E_INVALIDARG;
        D3D11_TEXTURE2D_DESC input{};
        source->GetDesc(&input);
        if (box.left >= box.right || box.top >= box.bottom || box.front != 0 || box.back != 1
            || box.right > input.Width || box.bottom > input.Height || input.ArraySize != 1) return E_INVALIDARG;
        const auto format = SampleFormat(input.Format);
        if (format == DXGI_FORMAT_UNKNOWN) return E_NOTIMPL;
        D3D11_RENDER_TARGET_VIEW_DESC targetView{};
        destination->GetDesc(&targetView);
        if (targetView.ViewDimension != D3D11_RTV_DIMENSION_TEXTURE2D
            || targetView.Texture2D.MipSlice != 0
            || (targetView.Format != DXGI_FORMAT_B8G8R8A8_UNORM
                && targetView.Format != DXGI_FORMAT_R8G8B8A8_UNORM)) return E_NOTIMPL;
        Ptr<ID3D11Device> device, inputDevice, outputDevice;
        context->GetDevice(device.GetAddressOf());
        source->GetDevice(inputDevice.GetAddressOf());
        destination->GetDevice(outputDevice.GetAddressOf());
        if (device.Get() != inputDevice.Get() || device.Get() != outputDevice.Get()) return E_INVALIDARG;
        Ptr<ID3D11Resource> resource;
        destination->GetResource(resource.GetAddressOf());
        Ptr<ID3D11Texture2D> output;
        HRESULT hr = resource.As(&output);
        if (FAILED(hr) || output.Get() == source) return E_INVALIDARG;
        D3D11_TEXTURE2D_DESC target{};
        output->GetDesc(&target);
        if (target.Width < width || target.Height < height || target.SampleDesc.Count != 1) return E_INVALIDARG;
        hr = Initialize(device.Get());
        if (FAILED(hr)) return hr;
        Ptr<ID3D11DeviceContext1> context1;
        hr = context->QueryInterface(IID_PPV_ARGS(context1.GetAddressOf()));
        if (FAILED(hr)) return hr;
        D3D11_TEXTURE2D_DESC sample{};
        sample.Width = box.right - box.left; sample.Height = box.bottom - box.top;
        sample.MipLevels = sample.ArraySize = sample.SampleDesc.Count = 1;
        sample.Format = format; sample.Usage = D3D11_USAGE_DEFAULT;
        sample.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        hr = device->CreateTexture2D(&sample, nullptr, sampleTexture.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        ID3D11Texture2D* copySource = source;
        if (input.SampleDesc.Count > 1) {
            UINT support = 0;
            hr = device->CheckFormatSupport(input.Format, &support);
            if (FAILED(hr) || !(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)) return E_NOTIMPL;
            auto resolve = input;
            resolve.MipLevels = 1; resolve.SampleDesc.Count = 1; resolve.SampleDesc.Quality = 0;
            resolve.Usage = D3D11_USAGE_DEFAULT; resolve.BindFlags = resolve.CPUAccessFlags = resolve.MiscFlags = 0;
            hr = device->CreateTexture2D(&resolve, nullptr, resolved.ReleaseAndGetAddressOf());
            if (FAILED(hr)) return hr;
            context->ResolveSubresource(resolved.Get(), 0, source, 0, input.Format);
            copySource = resolved.Get();
        }
        context->CopySubresourceRegion(sampleTexture.Get(), 0, 0, 0, 0, copySource, 0, &box);
        hr = device->CreateShaderResourceView(sampleTexture.Get(), nullptr, sampleView.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        struct Restore {
            ID3D11DeviceContext1* context;
            Ptr<ID3DDeviceContextState> saved;
            ~Restore() { context->SwapDeviceContextState(saved.Get(), nullptr); }
        } restore{context1.Get(), {}};
        context1->SwapDeviceContextState(scratch.Get(), restore.saved.GetAddressOf());
        const D3D11_VIEWPORT viewport{0, 0, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0, 1};
        context->RSSetViewports(1, &viewport);
        context->RSSetState(rasterizer.Get());
        context->OMSetRenderTargets(1, &destination, nullptr);
        context->OMSetBlendState(blend.Get(), nullptr, 0xffffffffu);
        context->OMSetDepthStencilState(depth.Get(), 0);
        context->IASetInputLayout(nullptr);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(vertex.Get(), nullptr, 0);
        context->PSSetShader(pixel.Get(), nullptr, 0);
        auto sampler = resample ? linearSampler.Get() : pointSampler.Get();
        context->PSSetSamplers(0, 1, &sampler);
        auto view = sampleView.Get();
        context->PSSetShaderResources(0, 1, &view);
        const FLOAT black[4]{0, 0, 0, 1};
        context->ClearRenderTargetView(destination, black);
        context->Draw(3, 0);
        view = nullptr;
        context->PSSetShaderResources(0, 1, &view);
        return S_OK;
    }
};
