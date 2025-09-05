#include "CopyAdapter.h"

#include "Common.h"

bool CopyAdapter::InitTexture(LPSURFACE& pSf, const int width, const int height) {
    return pSf == nullptr
        || pSf->GetWidth() != width || pSf->GetHeight() != height;
}
