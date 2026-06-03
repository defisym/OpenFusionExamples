#include "NeoStrFontCache.h"

#include "NeoStrFontCacheGDIPlus.h"

bool NeoStrFontCache::CacheValid() const {
    return pWordBreakCache != nullptr;
}

void NeoStrFontCache::Alloc() {
    NeoStrFontCache::Release();

    pWordBreakCache = new WordBreakHandler{};
}

void NeoStrFontCache::Release() {
    delete pWordBreakCache;
    pWordBreakCache = nullptr;
}

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
