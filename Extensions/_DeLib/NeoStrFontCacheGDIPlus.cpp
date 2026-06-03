#include "NeoStrFontCacheGDIPlus.h"

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

bool NeoStrFontCacheGDIPlus::CacheValid() const {
    if (!NeoStrFontCache::CacheValid()) { return false; }
    if (pFontCache == nullptr) { return false; }
    if (pCharSzCacheWithFont == nullptr) { return false; }
    if (pFontCollection == nullptr) { return false; }
    
    return true;
}

void NeoStrFontCacheGDIPlus::Alloc() {
    NeoStrFontCacheGDIPlus::Release();

    NeoStrFontCache::Alloc();
    pFontCache = new FontCache{};
    pCharSzCacheWithFont = new CharSizeCacheWithFont{};
    pFontCollection = new PrivateFontCollection{};
}

void NeoStrFontCacheGDIPlus::Release() {
    NeoStrFontCache::Release();

    delete pFontCache;
    pFontCache = nullptr;

    delete pCharSzCacheWithFont;
    pCharSzCacheWithFont = nullptr;

    delete pFontCollection;
    pFontCollection = nullptr;
}