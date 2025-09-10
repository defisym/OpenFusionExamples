#include "FFMpegAdapter.h"

#include "FFMpegAdapterD3D11.h"

bool FFMpegAdapterSupport(const AVHWDeviceType type) {
    do {
#ifdef _WIN32
        if (type == AV_HWDEVICE_TYPE_D3D11VA) { return true; }
#endif
    } while (false);

    return false;
}

std::unique_ptr<FFMpegAdapter> FFMpegAdapterFactory(const AVHWDeviceType type,
    AVHWDeviceContext* pHWDCtx) {
    do {
#ifdef _WIN32
        if (type == AV_HWDEVICE_TYPE_D3D11VA) { 
            return std::make_unique<FFMpegAdapterD3D11>(pHWDCtx);
        }
#endif
    } while (false);

    return std::make_unique<FFMpegAdapter>(pHWDCtx);
}
