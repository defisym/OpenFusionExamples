#include "NeoStrFontCache.h"

#include "NeoStrFontCacheGDIPlus.h"

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type) {
    do {
#ifdef _WIN32
        if (type == NeoStrBackendType::GDIPLUS) {
            return std::make_unique<NeoStrFontCacheGDIPlus>();
        }
#endif
    } while (false);

    return nullptr;
}
