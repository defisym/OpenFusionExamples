#pragma once

#include "FFMpegAdapter.h"

#include <libavutil/hwcontext_d3d11va.h>

#include "Rule035.h"
#include "D3DUtilities/D3DDefinition.h"

struct CopyToTextureContext {
    AVD3D11VADeviceContext* pD3D11VADeciveCtx = nullptr;
    ComPtr<ID3D11Texture2D> pTexture = nullptr;
    HANDLE sharedHandle = nullptr;
    DXGI_FORMAT textureFormat = DXGI_FORMAT_UNKNOWN;

    CopyToTextureContext() = default;
    RULE_NO_COPY(CopyToTextureContext);
};

struct FFMpegAdapterD3D11 :FFMpegAdapter {
    // for wait gpu
    ComPtr<ID3D11Query> pEvent = nullptr;

    AVD3D11VADeviceContext* GetHWCtx();
    FFMpegAdapterD3D11(AVHWDeviceContext* pHWDCtx);
    ~FFMpegAdapterD3D11() = default;
    void gpu_flush() override;
    void gpu_wait() override;
    TextureContextHandle AllocContext() override;
    void FreeContext(TextureContextHandle& h) override;;
    void convert_textureFrame(AVCodecContext* pCodecContext,
        AVFrame* pFrame, const FrameDataCallBack& callBack) override;
    void CopyContext(TextureContextHandle src, TextureContextHandle dst) override;
};