#pragma once

#include <string>
#include <format>

#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

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
    using PixelShader = ComPtr<ID3D11PixelShader>;

    // Targets:
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/specifying-compiler-targets

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
                errorMsg += std::format("D3D11: With message: {}", errorBlob->GetBufferPointer());
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

    // ------------------------------------------
    // Compile VertexShader
    // ------------------------------------------

    [[nodiscard]] VertexShader CreateVertexShader(const Blob& vertexShaderBlob) {
        VertexShader vertexShader;
        if (FAILED(pDevice->CreateVertexShader(
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            nullptr,
            &vertexShader))) {
            errorMsg = "D3D11: Failed to compile vertex shader\n";
            return nullptr;
        }

        return vertexShader;
    }

    [[nodiscard]] VertexShader CreateVertexShader(const void* pData, const size_t sz,
        const char* pEntryPoint = "Main", const char* pTarget = "vs_5_0") {
        Blob vertexShaderBlob = nullptr;
        if (!CompileShader(pData, sz, pEntryPoint, pTarget, vertexShaderBlob)) {
            return nullptr;
        }

        return CreateVertexShader(vertexShaderBlob);
    }

    [[nodiscard]] VertexShader CreateVertexShader(const wchar_t* pFileName,
        const char* pEntryPoint = "Main", const char* pTarget = "vs_5_0") {
        Blob vertexShaderBlob = nullptr;
        if (!CompileShader(pFileName, pEntryPoint, pTarget, vertexShaderBlob)) {
            return nullptr;
        }

        return CreateVertexShader(vertexShaderBlob);       
    }

    // ------------------------------------------
    // Compile PixelShader
    // ------------------------------------------

    [[nodiscard]] PixelShader CreatePixelShader(const Blob& pixelShaderBlob) {
        PixelShader pixelShader;
        if (FAILED(pDevice->CreatePixelShader(
            pixelShaderBlob->GetBufferPointer(),
            pixelShaderBlob->GetBufferSize(),
            nullptr,
            &pixelShader))) {
            errorMsg = "D3D11: Failed to compile pixel shader\n";
            return nullptr;
        }

        return pixelShader;
    }

    [[nodiscard]] PixelShader CreatePixelShader(const void* pData, const size_t sz,
        const char* pEntryPoint = "Main", const char* pTarget = "ps_5_0") {
        Blob pixelShaderBlob = nullptr;
        if (!CompileShader(pData, sz, pEntryPoint, pTarget, pixelShaderBlob)) {
            return nullptr;
        }

        return CreatePixelShader(pixelShaderBlob);
    }

    [[nodiscard]] PixelShader CreatePixelShader(const wchar_t* pFileName,
        const char* pEntryPoint = "Main", const char* pTarget = "ps_5_0") {
        Blob pixelShaderBlob = nullptr;
        if (!CompileShader(pFileName, pEntryPoint, pTarget, pixelShaderBlob)) {
            return nullptr;
        }

        return CreatePixelShader(pixelShaderBlob);
    }
};