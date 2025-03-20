#pragma once

#include "D3DUtilities/VertexSharedHelper.h"
#include "D3DUtilities/PixelSharedHelper.h"

#ifdef PRE_COMPILE_SHADER
#include "Shader/Inc/VS.inc"
#include "Shader/Inc/YUVToBGRA.inc"
#endif

// shared by all instance
struct D3DSharedHandler {
#ifdef PRE_COMPILE_SHADER
    ShaderCompiler compiler;    
#else
    ShaderResourceCompiler compiler;
#endif
    VertexSharedHelper vertexHelper;
    PixelSharedHelper pixelHelper;

    D3DSharedHandler(ID3D11Device* pDevice, const HMODULE hInstLib) 
        :compiler(pDevice), 
#ifdef PRE_COMPILE_SHADER
        vertexHelper(&compiler, VS_VS, sizeof(VS_VS)),
        pixelHelper(&compiler, PS_YUVToBGRA, sizeof(PS_YUVToBGRA))
#else
        vertexHelper(&compiler, hInstLib, SHADER_VS_TEMPLATE),
        pixelHelper(&compiler, hInstLib, SHADER_PS_YUVToBGRA)
#endif
    {
        if (FAILED(vertexHelper.hr)) { return; }
        if (FAILED(pixelHelper.hr)) { return; }       
    }
};

// each instance have separate handler
struct D3DLocalHandler {
    HRESULT hr = S_OK;
    ID3D11Device* pDevice = nullptr;

    int width = 0;
    int height = 0;

    D3D11_VIEWPORT viewPort = {};

    // size of buffer must be multiple of 16
    struct alignas(16) PixelSizeBuffer {
        float fPixelWidth;
        float fPixelHeight;
    };
    PixelSizeBuffer psb = { };

    ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;

    D3DLocalHandler(ID3D11Device* pD) :pDevice(pD) {
        // viewport
        viewPort.TopLeftX = 0;
        viewPort.TopLeftY = 0;
        viewPort.Width = (FLOAT)width;
        viewPort.Height = (FLOAT)height;
        viewPort.MaxDepth = 1;
        viewPort.MinDepth = 0;

        // create constant buffer
        // make it local instead of shared as when playing two video
        // with differen resolution will trigger unnecessary update
        psb = { 1.0f / width, 1.0f / height };

        D3D11_BUFFER_DESC psbDesc = {};
        psbDesc.Usage = D3D11_USAGE_DEFAULT;
        psbDesc.ByteWidth = sizeof(PixelSizeBuffer);
        psbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        psbDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = &psb;

        hr = pDevice->CreateBuffer(&psbDesc, &initData, &pConstantBuffer);
        if (FAILED(hr)) { return; }
    }

    HANDLE sharedHandle = nullptr;
    ComPtr<ID3D11Texture2D> pSharedFrameTexture;
    ComPtr<ID3D11ShaderResourceView> pSrvY = nullptr;
    ComPtr<ID3D11ShaderResourceView> pSrvUV = nullptr;

    inline void UpdateSharedTexture(ID3D11DeviceContext* pDeviceCtx,
        HANDLE sharedHandle, DXGI_FORMAT textureFormat) {
        if (this->sharedHandle == sharedHandle) { return; }
        
        // shared texture points to the same memory as original texture
        // so don't need to re-open or update SRV when rendering
        this->sharedHandle = sharedHandle;
        hr = pDevice->OpenSharedResource(sharedHandle, IID_PPV_ARGS(&pSharedFrameTexture));
        if (FAILED(hr)) { return; }

        // create srv
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        // create new shader resource view
        DXGI_FORMAT yFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT uvFormat = DXGI_FORMAT_UNKNOWN;

        // YUV cannot create srv directly, split to two planes
        // uses UNORM in view & float in shader, may lose precision
        switch (textureFormat) {
        case DXGI_FORMAT_NV12:
        {
            yFormat = DXGI_FORMAT_R8_UNORM;
            uvFormat = DXGI_FORMAT_R8G8_UNORM;

            break;
        }
        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
        {
            yFormat = DXGI_FORMAT_R16_UNORM;
            uvFormat = DXGI_FORMAT_R16G16_UNORM;

            break;
        }
        // format not supported
        default:
        {
            return;
        }
        }

        srvDesc.Format = yFormat;
        hr = pDevice->CreateShaderResourceView(pSharedFrameTexture.Get(), &srvDesc, &pSrvY);
        if (FAILED(hr)) { return; }

        srvDesc.Format = uvFormat;
        hr = pDevice->CreateShaderResourceView(pSharedFrameTexture.Get(), &srvDesc, &pSrvUV);
        if (FAILED(hr)) { return; }
    }

    inline void UpdateResolution(ID3D11DeviceContext* pDeviceCtx,
        const int width, const int height) {
        if (this->width == width && this->height == height) { return; }

        // update resolution
        this->width = width; this->height = height;

        // viewport
        viewPort.Width = (FLOAT)width;
        viewPort.Height = (FLOAT)height;

        // constant buffer
        psb = { 1.0f / width, 1.0f / height };
        pDeviceCtx->UpdateSubresource(pConstantBuffer.Get(), 0, nullptr, &psb, 0, 0);
    }

    inline void UpdateContext(ID3D11DeviceContext* pDeviceCtx) {
        // rasterizer
        pDeviceCtx->RSSetViewports(1, &viewPort);

        // shader resource view
        ID3D11ShaderResourceView* srvs[] = { pSrvY.Get(), pSrvUV.Get() };
        pDeviceCtx->PSSetShaderResources(0, std::size(srvs), srvs);

        // constant buffer
        ID3D11Buffer* cb[] = { pConstantBuffer.Get() };
        pDeviceCtx->PSSetConstantBuffers(0, std::size(cb), cb);
    }
};