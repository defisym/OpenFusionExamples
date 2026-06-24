#pragma once

#include "NeoStrFontCache.h"
#include "NeoStrContextFreeType.h"

struct CharSizeCacheItem;
struct NeoStrFontInfoFreeType :public NeoStrFontInfo {
 
};

struct NeoStrFontCacheFreeType :public NeoStrFontCache {
    using FontCache = std::vector<std::vector<FT_Face>>;

    FontCache* pFontCache = nullptr;

    NeoStrContextFreeType* pCtx = nullptr;

    void SetContext(NeoStrContext* pCtx) override;

    bool CacheValid() const override;
    void Alloc() override;
    void Release() override;

    bool EmbedFontFromFile(const std::filesystem::path& filePath) override;
    bool EmbedFontFromMemory(const char* pData, const size_t sz) override;

    bool HasFont(const NeoStrFontInfo& fontInfo) const override;
    NeoStrFont GetFont(const NeoStrFontInfo& fontInfo) const override;
    CharSize GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) override;
};
