#pragma once

#pragma warning(disable : 4819)

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <memory>
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
    // copy shared resource in context handle
    virtual void CopyContext(TextureContextHandle src, TextureContextHandle dst) {};
};

bool FFMpegAdapterSupport(const AVHWDeviceType type);
std::unique_ptr<FFMpegAdapter> FFMpegAdapterFactory(const AVHWDeviceType type,
    AVHWDeviceContext* pHWDCtx);