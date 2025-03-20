#pragma once

#include <string>
#include <format>

#include <d3d11.h>
#ifndef PRE_COMPILE_SHADER
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#endif

#include "D3DDefinition.h"

struct ShaderCompiler {
    ID3D11Device* pDevice = nullptr;

    ShaderCompiler(ID3D11Device* pD) :pDevice(pD) {}

    std::string errorMsg = {};
    const std::string& GetLastError() const { return errorMsg; }

    // ------------------------------------------
    // Compile Shader
    // ------------------------------------------

    using Blob = ComPtr<ID3DBlob>;

    using VertexShader = ComPtr<ID3D11VertexShader>;
    using VertexShaderBundle = decltype(std::make_tuple(Blob{}, VertexShader{}));

    using PixelShader = ComPtr<ID3D11PixelShader>;
    using PixelShaderBundle = decltype(std::make_tuple(Blob{}, PixelShader{}));

    template<typename T>
    static T GetNullBundle() {
        return std::make_tuple(nullptr, nullptr);
    }

    // Targets:
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/specifying-compiler-targets
    constexpr static auto DEFAULT_ENTRYPOINT = "Main";
    constexpr static auto DEFAULT_TARGET = "5_0";

#ifndef PRE_COMPILE_SHADER
    bool CompileShader(const void* pData, const size_t sz,
       const char* pEntryPoint, const char* pTarget,
       Blob& shaderBlob) {
        constexpr UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

        Blob tempShaderBlob = nullptr;
        Blob errorBlob = nullptr;
        if (FAILED(D3DCompile(
            pData, sz,
            nullptr,
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            pEntryPoint, pTarget,
            compileFlags, 0,
            &tempShaderBlob, &errorBlob))) {
            errorMsg = "D3D11: Failed to read shader from buffer\n";
            if (errorBlob != nullptr) {
                errorMsg += std::format("D3D11: With message: {}", (const char*)errorBlob->GetBufferPointer());
            }

            return false;
        }

        shaderBlob = std::move(tempShaderBlob);

        return true;
    }

    bool CompileShader(const wchar_t* pFileName,
        const char* pEntryPoint, const char* pTarget,
        Blob& shaderBlob) {
        constexpr UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

        Blob tempShaderBlob = nullptr;
        Blob errorBlob = nullptr;
        if (FAILED(D3DCompileFromFile(
            pFileName,
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            pEntryPoint, pTarget,
            compileFlags, 0,
            &tempShaderBlob, &errorBlob))) {
            errorMsg = "D3D11: Failed to read shader from file\n";
            if (errorBlob != nullptr) {
                errorMsg += std::format("D3D11: With message: {}", errorBlob->GetBufferPointer());
            }

            return false;
        }

        shaderBlob = std::move(tempShaderBlob);

        return true;
    }
#endif

    // ------------------------------------------
    // Compile VertexShader
    // ------------------------------------------

#ifdef PRE_COMPILE_SHADER
    [[nodiscard]] VertexShaderBundle CreateVertexShader(const void* pShaderBytecode, SIZE_T bytecodeLength) {
        VertexShader vertexShader;
        if (FAILED(pDevice->CreateVertexShader(
            pShaderBytecode,
            bytecodeLength,
            nullptr,
            &vertexShader))) {
            errorMsg = "D3D11: Failed to compile vertex shader\n";
            return GetNullBundle<VertexShaderBundle>();
        }

        return std::make_tuple(nullptr, vertexShader);
    }
#else
    [[nodiscard]] VertexShaderBundle CreateVertexShader(const Blob& vertexShaderBlob) {
        VertexShader vertexShader;
        if (FAILED(pDevice->CreateVertexShader(
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            nullptr,
            &vertexShader))) {
            errorMsg = "D3D11: Failed to compile vertex shader\n";
            return GetNullBundle<VertexShaderBundle>();
        }

        return std::make_tuple(vertexShaderBlob, vertexShader);
    }

    [[nodiscard]] VertexShaderBundle CreateVertexShader(const void* pData, const size_t sz,
        const char* pEntryPoint = DEFAULT_ENTRYPOINT, const char* pTarget = DEFAULT_TARGET) {
        Blob vertexShaderBlob = nullptr;
        if (!CompileShader(pData, sz,
            pEntryPoint, std::format("vs_{}", pTarget).c_str(),
            vertexShaderBlob)) {
            return GetNullBundle<VertexShaderBundle>();
        }

        return CreateVertexShader(vertexShaderBlob);
    }

    [[nodiscard]] VertexShaderBundle CreateVertexShader(const wchar_t* pFileName,
        const char* pEntryPoint = DEFAULT_ENTRYPOINT, const char* pTarget = DEFAULT_TARGET) {
        Blob vertexShaderBlob = nullptr;
        if (!CompileShader(pFileName,
            pEntryPoint, std::format("vs_{}", pTarget).c_str(),
            vertexShaderBlob)) {
            return GetNullBundle<VertexShaderBundle>();
        }

        return CreateVertexShader(vertexShaderBlob);
    }
#endif

    // ------------------------------------------
    // Compile PixelShader
    // ------------------------------------------

#ifdef PRE_COMPILE_SHADER
    [[nodiscard]] PixelShaderBundle CreatePixelShader(const void* pShaderBytecode, SIZE_T bytecodeLength) {
        PixelShader pixelShader;
        if (FAILED(pDevice->CreatePixelShader(
            pShaderBytecode,
            bytecodeLength,
            nullptr,
            &pixelShader))) {
            errorMsg = "D3D11: Failed to compile pixel shader\n";
            return GetNullBundle<PixelShaderBundle>();
        }

        return std::make_tuple(nullptr, pixelShader);
    }
#else
    [[nodiscard]] PixelShaderBundle CreatePixelShader(const Blob& pixelShaderBlob) {
        PixelShader pixelShader;
        if (FAILED(pDevice->CreatePixelShader(
            pixelShaderBlob->GetBufferPointer(),
            pixelShaderBlob->GetBufferSize(),
            nullptr,
            &pixelShader))) {
            errorMsg = "D3D11: Failed to compile pixel shader\n";
            return GetNullBundle<PixelShaderBundle>();
        }

        return std::make_tuple(pixelShaderBlob, pixelShader);
    }

    [[nodiscard]] PixelShaderBundle CreatePixelShader(const void* pData, const size_t sz,
        const char* pEntryPoint = DEFAULT_ENTRYPOINT, const char* pTarget = DEFAULT_TARGET) {
        Blob pixelShaderBlob = nullptr;
        if (!CompileShader(pData, sz,
            pEntryPoint, std::format("ps_{}", pTarget).c_str(),
            pixelShaderBlob)) {
            return GetNullBundle<PixelShaderBundle>();
        }

        return CreatePixelShader(pixelShaderBlob);
    }

    [[nodiscard]] PixelShaderBundle CreatePixelShader(const wchar_t* pFileName,
        const char* pEntryPoint = DEFAULT_ENTRYPOINT, const char* pTarget = DEFAULT_TARGET) {
        Blob pixelShaderBlob = nullptr;
        if (!CompileShader(pFileName,
            pEntryPoint, std::format("ps_{}", pTarget).c_str(),
            pixelShaderBlob)) {
            return GetNullBundle<PixelShaderBundle>();
        }

        return CreatePixelShader(pixelShaderBlob);
    }
#endif
};