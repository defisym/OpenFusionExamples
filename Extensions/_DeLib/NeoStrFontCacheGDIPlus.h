#pragma once

#include <map>

#include "NeoStrFontCache.h"
#include "NeoStrDefinitionGDIPlus.h"

struct NeoStrFontCacheGDIPlus:public NeoStrFontCache {
    //using LogFontHash = size_t;
    //using FontCache = std::map<LogFontHash, Font*>;

    //FontCache* pFontCache = nullptr;
    //CharSizeCacheWithFont* pCharSzCacheWithFont = nullptr;
    //WordBreakHandler* pRegexCache = nullptr;
    //PrivateFontCollection* pFontCollection = nullptr;

    //inline bool CacheValid() const {
    //    return pFontCache && pCharSzCacheWithFont && pRegexCache;
    //}

    //inline void UpdateNeoStr(NeoStr* pNeoStr) const {
    //    pNeoStr->bExternalCache = true;

    //    pNeoStr->pFontCache = this->pFontCache;
    //    pNeoStr->pCharSzCacheWithFont = this->pCharSzCacheWithFont;
    //    pNeoStr->pRegexCache = this->pRegexCache;
    //}
};