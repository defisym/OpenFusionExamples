#include "NeoStrFactory.h"

#ifdef _WIN32
#define BACKEND_GDIPLUS
#endif

#ifdef BACKEND_GDIPLUS
#include "NeoStrDefinitionGDIPlus.h"

#ifdef ENABLE_GDIPLUS
#define AVAILABLE_GDIPLUS

#include "NeoStrContextGDIPlus.h"
#include "NeoStrFontCacheGDIPlus.h"
#endif // ENABLE_GDIPLUS

#endif

bool NeoStrBackendSupport(const NeoStrBackendType type) {
    switch (type) {
#ifdef AVAILABLE_GDIPLUS
    case NeoStrBackendType::GDIPLUS: { return true; }
#endif
    default: return false;
    }

    return false;
}

std::unique_ptr<NeoStrContext> NeoStrContextFactory(const NeoStrBackendType type) {
    do {
#ifdef AVAILABLE_GDIPLUS
        if (type == NeoStrBackendType::GDIPLUS) {
            return std::make_unique<NeoStrContextGDIPlus>();
        }
#endif
    } while (false);

    return nullptr;
}

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type) {
    do {
#ifdef AVAILABLE_GDIPLUS
        if (type == NeoStrBackendType::GDIPLUS) {
            return std::make_unique<NeoStrFontCacheGDIPlus>();
        }
#endif
    } while (false);

    return nullptr;
}
