#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "FFMpegDefinition.h"

using TextureContextHandle = void*;

// Opaque data passed to AVCodecContext.opaque
struct AVCodecCtxOpaque {
    // used in AVCodecContext->get_format callback
    AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;
    // video & get use separate context to avoid conflict
    // e.g., if share the same texture, call get first, and it's result 
    // not updated in video decode, then the incorrect frame will be displayed
    TextureContextHandle hTextureCtx = nullptr;
};

struct FFMpegAdapter {
    AVHWDeviceContext* pHWDCtx = nullptr;

    FFMpegAdapter(AVHWDeviceContext* p) :pHWDCtx(p) {};
    virtual ~FFMpegAdapter() = default;
    virtual void gpu_flush() {}
    virtual void gpu_wait() {}
    virtual TextureContextHandle AllocContext() { return nullptr; }
    virtual void FreeContext(TextureContextHandle& h) { h = nullptr; }
    // do nothing, just pass D3D11 texture to callback, furture process not needed
    virtual void convert_textureFrame(AVCodecContext* pCodecContext,
        AVFrame* pFrame, const FrameDataCallBack& callBack) {
    };
};