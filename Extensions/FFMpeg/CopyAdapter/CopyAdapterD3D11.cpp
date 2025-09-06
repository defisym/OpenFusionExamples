#include "CopyAdapterD3D11.h"

#include "common.h"
#include "FFMpegAdapterD3D11.h"

CopyAdapterD3D11::CopyAdapterD3D11(LPRDATA p) :CopyAdapter(p) {
    auto pD3DDevice = (ID3D11Device*)GetD3D11Device(rdPtr);

    if (!pD3DSharedHandler) {
        pD3DSharedHandler = std::make_unique<D3DSharedHandler>(pD3DDevice, hInstLib);
    }

    pD3DLocalHandler = std::make_unique<D3DLocalHandler>(pD3DDevice);
}

bool CopyAdapterD3D11::InitTexture(LPSURFACE& pSf,
    const int width, const int height) {
    if (CopyAdapter::TextureValid(pSf, width, height) && IsHWA(pSf)) {
        return false;
    }

    delete pSf;
    pSf = CreateHWASurface(32, width, height, ST_HWA_RTTEXTURE);
    // alpha channel is ignored if surface type is ST_HWA_RTTEXTURE
    _AddAlpha(pSf);
    
    return true;
}

bool CopyAdapterD3D11::CopyTexture(LPSURFACE pRTTSf,
    const unsigned char* pData, const int width, const int height) {
    // 0. validate
    if (!CopyAdapter::CopyValid(pRTTSf, pData)) { return false; }

    // 1. basic info
    const auto pFFMpegCtx = (const CopyToTextureContext*)pData;
    HRESULT hr = S_OK;

    // Format:      DXGI_FORMAT_NV12 (YUV 4:2:0)
    //              DXGI_FORMAT_P010 (YUV 4:2:2)
    //              DXGI_FORMAT_P010 (YUV 4:2:2)
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  512 (D3D11_BIND_DECODER)
    //                  Set this flag to indicate that a 2D texture is used to 
    //                  receive output from the decoder API. The common way to 
    //                  create resources for a decoder output is by calling the 
    //                  ID3D11Device::CreateTexture2D method to create an array 
    //                  of 2D textures. However, you cannot use texture arrays 
    //                  that are created with this flag in calls to 
    //                  ID3D11Device::CreateShaderResourceView.
    auto sharedHandle = pFFMpegCtx->sharedHandle;
    auto textureFormat = pFFMpegCtx->textureFormat;

    // Fusion Context
    auto renderHelper = RenderHelper{ pRTTSf };     // ST_HWA_RTTEXTURE
    auto RTTInfo = GetD3D11SurfaceInfo(pRTTSf);

    auto pFusionDevice = (ID3D11Device*)RTTInfo.m_pD3D11Device;
    auto pFusionDeviceCtx = (ID3D11DeviceContext*)RTTInfo.m_pD3D11Context;

    // Format:      DXGI_FORMAT_B8G8R8A8_UNORM
    // Usage:       D3D11_USAGE_DEFAULT (GPU write & read)
    // Bind flags:  40 (D3D11_BIND_SHADER_RESOURCE & D3D11_BIND_RENDER_TARGET)
    //              D3D11_BIND_SHADER_RESOURCE
    //                  Bind a buffer or texture to a shader stage; this flag cannot 
    //                  be used with the D3D11_MAP_WRITE_NO_OVERWRITE flag.
    //              D3D11_BIND_RENDER_TARGET
    //                  Bind a texture as a render target for the output-merger stage.
    auto pRTTexture = *(ID3D11Texture2D**)(RTTInfo.m_ppD3D11RenderTargetTexture);
    auto pRTTextureView = *(ID3D11RenderTargetView**)(RTTInfo.m_ppD3D11RenderTargetView);

    // 2. vertex shader
    pD3DSharedHandler->vertexHelper.UpdateContext(pFusionDeviceCtx);

    // 3. pixel shader
    pD3DSharedHandler->pixelHelper.UpdateContext(pFusionDeviceCtx);

    // 4. resolution & shared texture
    pD3DLocalHandler->UpdateResolution(pFusionDeviceCtx, width, height);
    if (FAILED(pD3DLocalHandler->hr)) { return false; }
    pD3DLocalHandler->UpdateSharedTexture(pFusionDeviceCtx, sharedHandle, textureFormat);
    if (FAILED(pD3DLocalHandler->hr)) { return false; }

    pD3DLocalHandler->UpdateContext(pFusionDeviceCtx);

    // 5. render
    ID3D11RenderTargetView* rtvs[] = { pRTTextureView };
    pFusionDeviceCtx->OMSetRenderTargets(1, rtvs, nullptr);

    pFusionDeviceCtx->DrawIndexed(pD3DSharedHandler->vertexHelper.indicesSize, 0, 0);

    // 6. return
    return true;
}
