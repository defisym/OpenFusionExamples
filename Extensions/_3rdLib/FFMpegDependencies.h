#pragma once

#include <libavcodec/version_major.h>
#include <libavdevice/version_major.h>
#include <libavfilter/version_major.h>
#include <libavformat/version_major.h>
#include <libavutil/version.h>
#include <libswresample/version_major.h>
#include <libswscale/version_major.h>

#define OFE_FFMPEG_STRINGIFY_IMPL(value) #value
#define OFE_FFMPEG_STRINGIFY(value) OFE_FFMPEG_STRINGIFY_IMPL(value)
#define OFE_FFMPEG_WIDEN_IMPL(value) L##value
#define OFE_FFMPEG_WIDEN(value) OFE_FFMPEG_WIDEN_IMPL(value)
#define OFE_FFMPEG_DLL(name, major) \
    OFE_FFMPEG_WIDEN(name) L"-" OFE_FFMPEG_WIDEN(OFE_FFMPEG_STRINGIFY(major)) L".dll"

// Fusion requires a static list; the SDK headers supply each DLL's ABI version.
#define OFE_FFMPEG_DEPENDENCIES \
    OFE_FFMPEG_DLL("avcodec", LIBAVCODEC_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("avdevice", LIBAVDEVICE_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("avfilter", LIBAVFILTER_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("avformat", LIBAVFORMAT_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("avutil", LIBAVUTIL_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("swresample", LIBSWRESAMPLE_VERSION_MAJOR), \
    OFE_FFMPEG_DLL("swscale", LIBSWSCALE_VERSION_MAJOR)
