#include "NeoStrFontCache.h"

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
