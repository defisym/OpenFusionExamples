#pragma once

#include <memory>

#include "NeoStrBackend.h"
#include "NeoStrLayout.h"
#include "NeoStrWordBreakHandler.h"

using NeoStrFont = void*;
struct NeoStrFontInfo {};

struct NeoStrFontCache {
    using CharSizeCache = std::map<wchar_t, CharSize>;
    WordBreakHandler* pWordBreakCache = nullptr;

    using FontNames = std::vector<std::wstring>;
    FontNames embedFontList;

    virtual ~NeoStrFontCache() {};

    virtual bool CacheValid() const;
    virtual void Alloc();
    virtual void Release();
    
    // return true if all font names are added
    // do not call embed
    bool FontEmbed(const FontNames& fontNames);
    void AddEmbedFont(const FontNames& fontNames);

    virtual bool EmbedFontFromFile(const std::wstring& filePath) = 0;
    virtual bool EmbedFontFromMemory(const char* pData, const size_t sz) = 0;

    virtual bool HasFont(const NeoStrFontInfo& fontInfo) const = 0;
    virtual NeoStrFont GetFont(const NeoStrFontInfo& fontInfo) const = 0;
    virtual CharSize GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) = 0;
};

std::unique_ptr<NeoStrFontCache> NeoStrFontCacheFactory(const NeoStrBackendType type);