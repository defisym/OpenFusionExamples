#pragma once

#include <map>

#include "NeoStrFontCache.h"
#include "NeoStrDefinitionGDIPlus.h"

struct CharSizeCacheItem;

struct NeoStrFontCacheGDIPlus :public NeoStrFontCache {
    using LogFontHash = size_t;
    using FontCache = std::map<LogFontHash, Font*>;

    using CharSizeCacheWithFont = std::map<LogFontHash, CharSizeCacheItem>;

    FontCache* pFontCache = nullptr;
    CharSizeCacheWithFont* pCharSzCacheWithFont = nullptr;
    PrivateFontCollection* pFontCollection = nullptr;

    bool CacheValid() const override;  
    virtual void Alloc() override;
    virtual void Release() override;
};