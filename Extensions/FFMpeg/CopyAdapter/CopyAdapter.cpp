#include "CopyAdapter.h"

#include "Common.h"

#include "CopyAdapterBitmap.h"
#include "CopyAdapterD3D11.h"

bool CopyAdapter::InitTexture(LPSURFACE& pSf, const int width, const int height) {
    return pSf == nullptr
        || pSf->GetWidth() != width || pSf->GetHeight() != height;
}

bool CopyAdapterSupport(const AVHWDeviceType type) {
    do {
        if (type == AV_HWDEVICE_TYPE_NONE) { return true; }
        if (type == AV_HWDEVICE_TYPE_D3D11VA) { return true; }
    } while (false);

    return false;
}

CopyAdapter* CopyAdapterFactory(LPRDATA rdPtr, const AVHWDeviceType type) {
    do {
        if (type == AV_HWDEVICE_TYPE_NONE) { 
            return new CopyAdapterBitmap{ rdPtr };
        }

        if (type == AV_HWDEVICE_TYPE_D3D11VA) {
            return new CopyAdapterD3D11{ rdPtr };
        }
    } while (false);

    return nullptr;
}

bool UpdateCopyAdapter(CopyAdapter*& pCopyAdapter,
    LPRDATA rdPtr, const AVHWDeviceType type) {
    do {
        if (type == AV_HWDEVICE_TYPE_NONE 
            && dynamic_cast<CopyAdapterBitmap*>(pCopyAdapter) == nullptr) {
            delete pCopyAdapter;
            pCopyAdapter = new CopyAdapterBitmap{ rdPtr };

            return true;
        }

        if (type == AV_HWDEVICE_TYPE_D3D11VA 
            && dynamic_cast<CopyAdapterD3D11*>(pCopyAdapter) == nullptr) {            
            delete pCopyAdapter;
            pCopyAdapter = new CopyAdapterD3D11{ rdPtr };

            return true;
        }
    } while (false);

    return false;
}
