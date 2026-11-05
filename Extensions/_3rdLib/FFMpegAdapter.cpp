#include "FFMpegAdapter.h"

#ifdef _WIN32
#include "FFMpegAdapterD3D11.h"
#elif defined(__ANDROID__)
#include "FFMpegAdapterAndroid.h"
#endif

bool FFMpegAdapterSupport(const AVHWDeviceType type) {
    do {
#ifdef __ANDROID__
        if (type == AV_HWDEVICE_TYPE_MEDIACODEC) { return true; }
#endif
#ifdef _WIN32
        if (type == AV_HWDEVICE_TYPE_D3D11VA) { return true; }
#endif
    } while (false);

    return false;
}

std::unique_ptr<FFMpegAdapter> FFMpegAdapterFactory(const AVHWDeviceType type,
    AVHWDeviceContext* pHWDCtx) {
    do {
#ifdef __ANDROID__
        if (type == AV_HWDEVICE_TYPE_MEDIACODEC) {
            return std::make_unique<FFMpegAdapterAndroid>(pHWDCtx);
        }
#endif
#ifdef _WIN32
        if (type == AV_HWDEVICE_TYPE_D3D11VA && pHWDCtx) {
            return std::make_unique<FFMpegAdapterD3D11>(pHWDCtx);
        }
#endif
    } while (false);

    return std::make_unique<FFMpegAdapter>(pHWDCtx);
}
