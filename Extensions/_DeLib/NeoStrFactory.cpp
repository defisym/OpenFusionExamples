#include "NeoStrFactory.h"

#ifdef _WIN32
#define BACKEND_GDIPLUS
#endif

#ifdef BACKEND_GDIPLUS
#include "NeoStrContextGDIPlus.h"
#include "NeoStrFontCacheGDIPlus.h"
#endif

bool NeoStrBackendSupport(const NeoStrBackendType type) {
    switch (type) {
#ifdef BACKEND_GDIPLUS
    case NeoStrBackendType::GDIPLUS: { return true; }
#endif
    default: return false;
    }

    return false;
}

std::unique_ptr<NeoStrContext> NeoStrContextFactory(const NeoStrBackendType type) {
    do {
#ifdef BACKEND_GDIPLUS
        if (type == NeoStrBackendType::GDIPLUS) {
            return std::make_unique<NeoStrContextGDIPlus>();
        }
#endif
    } while (false);

    return nullptr;
}

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type) {
    do {
#ifdef BACKEND_GDIPLUS
        if (type == NeoStrBackendType::GDIPLUS) {
            return std::make_unique<NeoStrFontCacheGDIPlus>();
        }
#endif
    } while (false);

    return nullptr;
}
