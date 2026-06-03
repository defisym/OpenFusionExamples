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
    void Alloc() override;
    void Release() override;

    bool EmbedFontFromFile(const std::wstring& filePath) override;
    bool EmbedFontFromMemory(const char* pData, const size_t sz) override;
};

struct CharSizeCacheItem {
    HDC hdc = nullptr;
    HGDIOBJ hOldObj = nullptr;
    HFONT hFont = nullptr;
    TEXTMETRIC tm = { 0 };

    NeoStrFontCache::CharSizeCache cache;

    CharSizeCacheItem(CharSizeCacheItem& other) = delete;
    CharSizeCacheItem(CharSizeCacheItem&& other) = delete;
    CharSizeCacheItem& operator=(CharSizeCacheItem& other) = delete;
    CharSizeCacheItem& operator=(CharSizeCacheItem&& other) = delete;

    explicit CharSizeCacheItem(const LOGFONT& logFont) {
        hFont = CreateFontIndirect(&logFont);

        hdc = GetDC(nullptr);
        hOldObj = SelectObject(hdc, hFont);
        GetTextMetrics(hdc, &tm);
    }

    ~CharSizeCacheItem() {
        SelectObject(hdc, hOldObj);
        ReleaseDC(nullptr, hdc);
        DeleteObject(hFont);
    }
};
