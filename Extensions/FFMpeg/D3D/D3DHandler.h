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

