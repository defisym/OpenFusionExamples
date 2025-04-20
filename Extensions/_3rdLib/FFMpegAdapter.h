#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "FFMpegDefinition.h"

using TextureContextHandle = void*;

struct FFMpegAdapter {
    virtual ~FFMpegAdapter() = default;
    virtual void gpu_flush(AVCodecContext* pCtx) {}
    virtual void gpu_wait() {}
    virtual TextureContextHandle AllocContext() = 0;
    virtual void FreeContext(TextureContextHandle& h) = 0;
    virtual void convert_textureFrame(AVCodecContext* pCodecContext,
        AVFrame* pFrame, const FrameDataCallBack& callBack) {};
};