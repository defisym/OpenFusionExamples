#pragma once

#ifdef PRE_COMPILE_SHADER
#include "ShaderCompiler.h"
#else
#include "ShaderResourceCompiler.h"
#endif

struct PixelStaticHelper {
    HRESULT hr = S_OK;

    using PixelShaderBundle = ShaderCompiler::PixelShaderBundle;
    PixelShaderBundle psBundle = ShaderCompiler::GetNullBundle<PixelShaderBundle>();

    ComPtr<ID3D11SamplerState> pSamplerState = nullptr;

#ifdef PRE_COMPILE_SHADER
    PixelStaticHelper(ShaderCompiler* pCompiler, const void* pShaderBytecode, SIZE_T bytecodeLength) {
#else
    PixelStaticHelper(ShaderResourceCompiler* pCompiler, const HMODULE hInstLib, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
#endif
        // ------------------------------------
        // device
        // ------------------------------------
        auto pDevice = pCompiler->pDevice;

        // ------------------------------------
        // pixel shader
        // ------------------------------------

#ifdef PRE_COMPILE_SHADER
        psBundle = pCompiler->CreatePixelShader(pShaderBytecode, bytecodeLength);
#else
        psBundle = pCompiler->CreatePixelShader(hInstLib, resourceId, pEntryPoint, pTarget);
#endif

        // ------------------------------------
        // create sampler
        // ------------------------------------
        
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = pDevice->CreateSamplerState(&sampDesc, &pSamplerState);
        if (FAILED(hr)) { return; }
    }

    inline void UpdateContext(ID3D11DeviceContext* pDeviceCtx) {
        auto& [psBlob, ps] = psBundle;
        pDeviceCtx->PSSetShader(ps.Get(), 0, 0);

        ID3D11SamplerState* sss[] = { pSamplerState.Get() };
        pDeviceCtx->PSSetSamplers(0, std::size(sss), sss);
    }
};