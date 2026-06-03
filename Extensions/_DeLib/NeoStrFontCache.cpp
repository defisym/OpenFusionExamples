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

// return true if all font names are added
// do not call embed
bool NeoStrFontCache::FontEmbed(const FontNames& fontNames) {
    for (const auto& fontName : fontNames) {
        auto it = std::find(embedFontList.begin(), embedFontList.end(), fontName);
        if (it == embedFontList.end()) {
            return false;
        }
    }

    return true;
}

void NeoStrFontCache::AddEmbedFont(const FontNames& fontNames) {
    for (const auto& fontName : fontNames) {
        auto it = std::find(embedFontList.begin(), embedFontList.end(), fontName);
        if (it == embedFontList.end()) {
            embedFontList.push_back(fontName);
        }
    }
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
