#pragma once

#ifdef __ANDROID__
#include "FFMpegAdapter.h"

// MediaCodec owns Surface buffers; the Java binding owns the Surface itself.
struct FFMpegAdapterAndroid final : FFMpegAdapter {
    using FFMpegAdapter::FFMpegAdapter;
    const AVCodec* SelectDecoder(const AVCodec* codec) override;
    AVPixelFormat PixelFormat(const AVCodec* codec) override;
    bool OwnsDecoderContext() const override { return true; }
    int InitDecoder(AVCodecContext* context, void* surface) override;
    void ReleaseDecoder(AVCodecContext* context) override;
    void convert_textureFrame(AVCodecContext* context, AVFrame* frame,
        const FrameDataCallBack& callback) override;
};
#endif
