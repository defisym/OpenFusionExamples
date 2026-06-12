#pragma once

#include "NeoStrFontCache.h"
#include "NeoStrDefinitionFreeType.h"

struct CharSizeCacheItem;
struct NeoStrFontInfoFreeType :public NeoStrFontInfo {
 
};

struct NeoStrFontCacheGDIPlus :public NeoStrFontCache {
       bool CacheValid() const override;
    void Alloc() override;
    void Release() override;

    bool EmbedFontFromFile(const std::wstring& filePath) override;
    bool EmbedFontFromMemory(const char* pData, const size_t sz) override;

    bool HasFont(const NeoStrFontInfo& fontInfo) const override;
    NeoStrFont GetFont(const NeoStrFontInfo& fontInfo) const override;
    CharSize GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) override;
};
