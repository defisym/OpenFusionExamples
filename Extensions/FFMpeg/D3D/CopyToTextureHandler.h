#pragma once

#include <directxmath.h>

#include "D3DUtilities/ShaderInterface.h"

struct CopyToTextureHandler {
    ShaderInterface inf;

    using VertexShaderBundle = ShaderCompiler::VertexShaderBundle;
    VertexShaderBundle vsBundle = ShaderCompiler::GetNullBundle<VertexShaderBundle>();
    using PixelShaderBundle = ShaderCompiler::PixelShaderBundle;
    PixelShaderBundle psBundle = ShaderCompiler::GetNullBundle<PixelShaderBundle>();

    ComPtr<ID3D11InputLayout> pInputLayout;
    ComPtr<ID3D11Buffer> pVertexBuffer;

    ComPtr<ID3D11SamplerState> pSamplerStateY = nullptr;
    ComPtr<ID3D11SamplerState> pSamplerStateUV = nullptr;

    struct VertexBufferInfo {
        UINT stride = 0u;
        UINT offset = 0u;
    } vertexBufferInfo = {};

    size_t indicesSize = 0u;
    ComPtr<ID3D11Buffer> pIndexBuffer;

    CopyToTextureHandler(LPRDATA rdPtr, const HMODULE hInstLib) :inf(rdPtr) {
        // result
        HRESULT hr = S_OK;
        auto pFusionDevice = (ID3D11Device*)GetD3DInfo(rdPtr).m_pD3D11Device;
        
        // vertex shader
        vsBundle = inf.CreateVertexShader(hInstLib, SHADER_VS_TEMPLATE, "Main");
        auto& [vsBlob, vs] = vsBundle;

        // input layouts

        // What we expect in shader
        // struct VSInput {
        //     float3 position : POSITION;
        //     float3 color : COLOR0;
        // };

        // We should then create an input layout exactly with this format
        using Position = DirectX::XMFLOAT3;
        using Color = DirectX::XMFLOAT3;

        struct VertexShaderInput {
            Position position = {};
            Color color = {};
        };

        D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] = {
            {
                "POSITION",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                offsetof(VertexShaderInput, position),
                D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
                0
            },
            {
                "COLOR",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                offsetof(VertexShaderInput, color),
                D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
                0
            }
        };
               
        hr = pFusionDevice->CreateInputLayout(vertexInputLayoutInfo, std::size(vertexInputLayoutInfo),
            vsBlob.Get()->GetBufferPointer(), vsBlob.Get()->GetBufferSize(), &pInputLayout);
        if (FAILED(hr)) { return; }

        // vertex buffer

        // Center: (0, 0)
        // 
        //  (-1, 1)©°©¤©¤©Ð©¤©¤©´(1, 1)
        //         ©À©¤©¤©à©¤©¤©È
        //  (-1,-1)©¸©¤©¤©Ø©¤©¤©¼(1,-1)
        //

        constexpr Position POSITION_LT = { -1, 1, 0 };  // 0
        constexpr Position POSITION_RT = { 1, 1, 0 };  // 1
        constexpr Position POSITION_RB = { 1,-1, 0 };  // 2
        constexpr Position POSITION_LB = { -1,-1, 0 };  // 3

        constexpr Color COLOR_WHITE = { 1.0f, 1.0f, 1.0f };

        constexpr VertexShaderInput vertices[] = {
            { POSITION_LT, COLOR_WHITE },
            { POSITION_RT, COLOR_WHITE },
            { POSITION_RB, COLOR_WHITE },
            { POSITION_LB, COLOR_WHITE },
        };

        D3D11_BUFFER_DESC vsibd = {};
        vsibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vsibd.ByteWidth = sizeof(vertices);
        vsibd.StructureByteStride = sizeof(VertexShaderInput);

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = vertices;

        hr = pFusionDevice->CreateBuffer(&vsibd, &sd, &pVertexBuffer);
        if (FAILED(hr)) { return; }

        vertexBufferInfo = {
            .stride = sizeof(VertexShaderInput),
            .offset = 0u
        };

        // indice buffer

        // D3D render triangle in clockwise order

        //
        //  (0)©°©¤©¤©Ð©¤©¤©´(1)
        //     ©À©¤©¤©à©¤©¤©È
        //  (3)©¸©¤©¤©Ø©¤©¤©¼(2)
        //

        const UINT16 indices[] = {
            0,1,2,      // first
            0,2,3,      // second
        };

        indicesSize = std::size(indices);
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.ByteWidth = sizeof(indices);
        ibd.StructureByteStride = sizeof(UINT16);

        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = indices;

        hr = pFusionDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
        if (FAILED(hr)) { return; }

        // 6. pixel shader
        psBundle = inf.CreatePixelShader(hInstLib, SHADER_PS_NV12ToBGRA, "Main");

        // create sampler
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = pFusionDevice->CreateSamplerState(&sampDesc, &pSamplerStateY);
        if (FAILED(hr)) { return; }

        hr = pFusionDevice->CreateSamplerState(&sampDesc, &pSamplerStateUV);
        if (FAILED(hr)) { return; }
    }
};
