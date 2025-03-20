#pragma once

#include "ShaderCompiler.h"

// Helper class to work with Fusion runtime
struct ShaderResourceCompiler :ShaderCompiler {
    ShaderResourceCompiler(ID3D11Device* pD) :ShaderCompiler(pD) {}

    // #embed macro is not supported by MSVC neither in C/C++ for now
    // so the following method is MSVC exlcusive, by using resource files
    static auto GetShaderResource(const HMODULE hModule, const int resourceID) {
        void* pData = nullptr;
        size_t sz = 0;

        do {
            HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
            if (!hResource) { break; }

            HGLOBAL hMemory = LoadResource(hModule, hResource);
            if (!hMemory) { break; }

            pData = LockResource(hMemory);    // no need to unlock
            sz = SizeofResource(hModule, hResource);
        } while (false);

        return std::make_tuple(pData, sz);
    }

    [[nodiscard]] ShaderCompiler::VertexShaderBundle CreateVertexShader(const HMODULE hModule, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
        const auto [pData, sz] = GetShaderResource(hModule, resourceId);
        if (pData == nullptr) { return ShaderCompiler::GetNullBundle<ShaderCompiler::VertexShaderBundle>(); }

        return ShaderCompiler::CreateVertexShader(pData, sz, pEntryPoint, pTarget);
    }

    [[nodiscard]] ShaderCompiler::PixelShaderBundle CreatePixelShader(const HMODULE hModule, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
        const auto [pData, sz] = GetShaderResource(hModule, resourceId);
        if (pData == nullptr) { return ShaderCompiler::GetNullBundle<ShaderCompiler::PixelShaderBundle>(); }

        return ShaderCompiler::CreatePixelShader(pData, sz, pEntryPoint, pTarget);
    }
};
