#include "CopyAdapter.h"

#include "Common.h"

#include "CopyAdapterBitmap.h"
#include "CopyAdapterD3D11.h"

bool CopyAdapter::TextureValid(LPSURFACE pSf, const int width, const int height) {
    return pSf != nullptr
        && pSf->IsValid()
        && pSf->GetWidth() == width
        && pSf->GetHeight() == height;
}

bool CopyAdapter::CopyValid(LPSURFACE pDst, const unsigned char* pData) {
    return pData != nullptr && pDst != nullptr;
}

bool CopyAdapterSupport(LPRDATA rdPtr, const AVHWDeviceType type) {
    do {
        if (type == AV_HWDEVICE_TYPE_NONE) { return true; }
        if (type == AV_HWDEVICE_TYPE_D3D11VA && D3D11(rdPtr)) { return true; }
    } while (false);

    return false;
}

template<typename Adapter, class... Types>
    requires std::is_base_of_v<CopyAdapter, Adapter>
void UpdateCopyAdapterImpl(CopyAdapter*& pCopyAdapter, Types&&... args) {
    if (dynamic_cast<Adapter*>(pCopyAdapter) == nullptr) {
        delete pCopyAdapter;
        pCopyAdapter = new Adapter{ std::forward<Types>(args)... };
    }
}

bool UpdateCopyAdapter(CopyAdapter*& pCopyAdapter,
    LPRDATA rdPtr, const AVHWDeviceType type) {
    switch (type) {
    case AV_HWDEVICE_TYPE_D3D11VA:
        if (D3D11(rdPtr)) { // valid to create
            UpdateCopyAdapterImpl<CopyAdapterD3D11>(pCopyAdapter, rdPtr);
            return true;
        }

        [[fallthrough]];
    case AV_HWDEVICE_TYPE_NONE: 
        [[fallthrough]];
    default:
        UpdateCopyAdapterImpl<CopyAdapterBitmap>(pCopyAdapter, rdPtr);
        return true;
    }
   
    return false;
}
