#include "FFMpegAdapterAndroid.h"

#ifdef __ANDROID__
#include <string>
#include <stdexcept>
extern "C" {
#include <libavcodec/mediacodec.h>
#include <libavutil/opt.h>
}

const AVCodec* FFMpegAdapterAndroid::SelectDecoder(const AVCodec* codec) {
    const std::string name = std::string(avcodec_get_name(codec->id)) + "_mediacodec";
    const auto hardware = avcodec_find_decoder_by_name(name.c_str());
    return hardware ? hardware : codec;
}
AVPixelFormat FFMpegAdapterAndroid::PixelFormat(const AVCodec* codec) {
    const std::string name = codec->name;
    return name.ends_with("_mediacodec") ? AV_PIX_FMT_MEDIACODEC : AV_PIX_FMT_NONE;
}
int FFMpegAdapterAndroid::InitDecoder(AVCodecContext* context, void* surface) {
    if (!surface) return AVERROR(EINVAL);
    auto media = av_mediacodec_alloc_context();
    if (!media) return AVERROR(ENOMEM);
    // Use NDK codec discovery, including software codecs exposed on emulator devices.
    if (av_opt_set_int(context->priv_data, "ndk_codec", 1, 0) < 0) {
        av_free(media);
        return AVERROR(EINVAL);
    }
    const auto result = av_mediacodec_default_init(context, media, surface);
    if (result < 0) av_free(media);
    return result;
}
void FFMpegAdapterAndroid::ReleaseDecoder(AVCodecContext* context) {
    if (context && context->hwaccel_context) av_mediacodec_default_free(context);
}
void FFMpegAdapterAndroid::convert_textureFrame(AVCodecContext*, AVFrame* frame,
    const FrameDataCallBack& callback) {
    auto buffer = reinterpret_cast<AVMediaCodecBuffer*>(frame->data[3]);
    if (!buffer || av_mediacodec_release_buffer(buffer, 1) < 0)
        throw std::runtime_error("Cannot render MediaCodec Surface buffer");
    // No CPU pointer exists for a Surface frame. Signal presentation to the binding.
    callback(nullptr, frame->width, frame->height);
}
#endif
