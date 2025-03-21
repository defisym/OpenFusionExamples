#pragma once

#include <vector>

#ifdef PRE_COMPILE_SHADER
#include "ShaderCompiler.h"
#else
#include "ShaderResourceCompiler.h"
#endif

// ------------------------------------------------
// PixelSharedHelper
// ------------------------------------------------
// 
// this class holds a shader and it's sampler, which should be the same 
// for all instances that shares it.
// for constant buffer (pass params) and shader resource view (changed by texture)
// please create it for each instance as they should be different 
// and be frequently updated.
// you can create another class to handle that, a recommend name for that 
// is PixelLocalHandler or D3DLocalHandler.
// 
// Note: user should grantee that PixelSharedHelper MUST MATCH PixelLocalHandler 
//       when update context during render

struct PixelSharedHelper {
    HRESULT hr = S_OK;

    using PixelShaderBundle = ShaderCompiler::PixelShaderBundle;
    PixelShaderBundle psBundle = ShaderCompiler::GetNullBundle<PixelShaderBundle>();

    // for set sampler
    std::vector<ID3D11SamplerState*> pSamplerStateArr = {};
    std::vector<ComPtr<ID3D11SamplerState>> pSamplerStates = {};

#ifdef PRE_COMPILE_SHADER
    PixelSharedHelper(ShaderCompiler* pCompiler, const void* pShaderBytecode, SIZE_T bytecodeLength) {
#else
    PixelSharedHelper(ShaderResourceCompiler* pCompiler, const HMODULE hInstLib, const int resourceId,
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
        
        hr = CreateSampler(pDevice, 1, &sampDesc);
        if (FAILED(hr)) { return; }
    }

    inline void ClearSampler() {
        pSamplerStateArr.clear();
        pSamplerStates.clear();
    }

    // a default sampler with desc
    //      D3D11_SAMPLER_DESC sampDesc = {};
    //      sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //      sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //      sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //      sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    // will be created by default, usually enough for most cases.
    // call this function if you need to override it.
    inline HRESULT CreateSampler(ID3D11Device* pDevice,
        size_t numDesc, D3D11_SAMPLER_DESC* pDesc) {
        ClearSampler();

        for (size_t index = 0; index < numDesc; index++) {
            ComPtr<ID3D11SamplerState> pTempSamplerState = nullptr;
            hr = pDevice->CreateSamplerState(&pDesc[index], &pTempSamplerState);
            if (FAILED(hr)) { ClearSampler(); return hr; }

            // is safe here, as move ComPtr will copy pointer
            pSamplerStateArr.emplace_back(pTempSamplerState.Get());
            pSamplerStates.emplace_back(std::move(pTempSamplerState));
        }

        return S_OK;
    }

    inline void UpdateContext(ID3D11DeviceContext* pDeviceCtx) {
        auto& [psBlob, ps] = psBundle;
        pDeviceCtx->PSSetShader(ps.Get(), 0, 0);
                
        pDeviceCtx->PSSetSamplers(0, pSamplerStateArr.size(), pSamplerStateArr.data());
    }
};