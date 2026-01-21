#pragma once

#include <vector>

#ifdef PRE_COMPILE_SHADER
#include "ShaderCompiler.h"
#else
#include "ShaderResourceCompiler.h"
#endif

// ------------------------------------------------
// ComputeSharedHelper
// ------------------------------------------------

struct ComputeSharedHelper {
    HRESULT hr = S_OK;

    using ComputeShaderBundle = ShaderCompiler::ComputeShaderBundle;
    ComputeShaderBundle csBundle = ShaderCompiler::GetNullBundle<ComputeShaderBundle>();

    // for set sampler
    std::vector<ID3D11SamplerState*> pSamplerStateArr = {};
    std::vector<ComPtr<ID3D11SamplerState>> pSamplerStates = {};

#ifdef PRE_COMPILE_SHADER
    ComputeSharedHelper(ShaderCompiler* pCompiler, const void* pShaderBytecode, SIZE_T bytecodeLength) {
#else
    ComputeSharedHelper(ShaderResourceCompiler * pCompiler, const HMODULE hInstLib, const int resourceId,
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
        csBundle = pCompiler->CreateComputeShader(pShaderBytecode, bytecodeLength);
#else
        csBundle = pCompiler->CreateComputeShader(hInstLib, resourceId, pEntryPoint, pTarget);
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
        auto& [csBlob, cs] = csBundle;
        pDeviceCtx->CSSetShader(cs.Get(), nullptr, 0);

        pDeviceCtx->CSSetSamplers(0, pSamplerStateArr.size(), pSamplerStateArr.data());

    }

    // CS should clean up after dispatch
    // use this method to automatically clean up
    // https://learn.microsoft.com/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-dispatch
    inline void Dispatch(ID3D11DeviceContext* pDeviceCtx,
        UINT ThreadGroupCountX,
        UINT ThreadGroupCountY,
        UINT ThreadGroupCountZ) {
        // Dispatch
        pDeviceCtx->Dispatch(ThreadGroupCountX,
            ThreadGroupCountY,
            ThreadGroupCountZ);

        // Unbind the input textures from the CS for good housekeeping
        ID3D11ShaderResourceView* nullSRV[] = { NULL };
        pDeviceCtx->CSSetShaderResources(0, 1, nullSRV);

        // Unbind output from compute shader
        ID3D11UnorderedAccessView* nullUAV[] = { NULL };
        pDeviceCtx->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
    }
};
