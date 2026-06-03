#pragma once

#include <map>

#include "NeoStrFontCache.h"
#include "NeoStrDefinitionGDIPlus.h"

struct CharSizeCacheItem;
struct NeoStrFontGDIPlus :public NeoStrFont { 
    Font* pFont = nullptr; 

    NeoStrFontGDIPlus(Font* pFont);
};
struct NeoStrFontInfoGDIPlus :public NeoStrFontInfo { 
    LOGFONT logFont = {}; 

    NeoStrFontInfoGDIPlus(const LOGFONT& logFont);
};

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

    NeoStrFont GetFont(const NeoStrFontInfo& fontInfo) const override;
    NeoStrFont GetFontWithCache(const NeoStrFontInfo& fontInfo) const override;
    CharSize GetCharSizeWithCache(const wchar_t wChar,
        const NeoStrFontInfo& fontInfo) override;

    Font* GetFontPointer(const LOGFONT& logFont) const;
    Font* GetFontPointerWithCache(const LOGFONT& logFont) const;
    CharSize GetCharSizeWithCache(const wchar_t wChar,
         const LOGFONT& logFont);
    const CharSizeCacheItem& GetCharSizeCacheItem(const LOGFONT& logFont) const;

    static bool FontCollectionHasFont(const LPCWSTR pFaceName,
        const Gdiplus::FontCollection* pFontCollection);
    static int GetFontStyle(const LOGFONT& logFont);
    static CharSize GetCharSizeRaw(const wchar_t wChar, const HDC hdc);
    static size_t LogFontHasher(const LOGFONT& logFont);
};

struct CharSizeCacheItem {
    HDC hdc = nullptr;
    HGDIOBJ hOldObj = nullptr;
    HFONT hFont = nullptr;
    TEXTMETRIC tm = { 0 };

    NeoStrFontCache::CharSizeCache cache;

    CharSizeCacheItem() {};
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
