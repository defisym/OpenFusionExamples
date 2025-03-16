#pragma once

#include "ShaderCompiler.h"
#include "ShaderResource.h"

// Helper class to work with Fusion runtime
struct ShaderInterface {
    D3D11SURFINFO info = { };
    ShaderCompiler* pCompiler = nullptr;

    ShaderInterface(LPRDATA rdPtr) {
        info = GetD3DInfo(rdPtr);
        pCompiler = new ShaderCompiler{ (ID3D11Device*)info.m_pD3D11Device };
    }
    ~ShaderInterface() {
        delete pCompiler;
    }

    [[nodiscard]] ShaderCompiler::VertexShaderBundle CreateVertexShader(const HMODULE hModule, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
        const auto [pData, sz] = GetShaderResource(hModule, resourceId);
        if (pData == nullptr) { return ShaderCompiler::GetNullBundle<ShaderCompiler::VertexShaderBundle>(); }

        return pCompiler->CreateVertexShader(pData, sz, pEntryPoint, pTarget);
    }

    [[nodiscard]] ShaderCompiler::PixelShaderBundle CreatePixelShader(const HMODULE hModule, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
        const auto [pData, sz] = GetShaderResource(hModule, resourceId);
        if (pData == nullptr) { return ShaderCompiler::GetNullBundle<ShaderCompiler::PixelShaderBundle>(); }

        return pCompiler->CreatePixelShader(pData, sz, pEntryPoint, pTarget);
    }
};