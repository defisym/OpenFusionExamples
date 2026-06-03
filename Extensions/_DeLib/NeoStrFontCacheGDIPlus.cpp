#include "NeoStrFontCacheGDIPlus.h"

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

// reference:
//  https://stackoverflow.com/questions/42595856/fonts-added-with-addfontresourceex-are-not-working-in-gdi
//  https://docs.microsoft.com/zh-cn/windows/win32/api/gdiplusheaders/nf-gdiplusheaders-privatefontcollection-addfontfile
//  https://www.codeproject.com/Articles/42041/How-to-Use-a-Font-Without-Installing-it
//  https://blog.csdn.net/analogous_love/article/details/45845971

bool NeoStrFontCacheGDIPlus::EmbedFontFromFile(const std::wstring& filePath) {
    auto gdipRet = pFontCollection->AddFontFile(filePath.c_str());

    return gdipRet == Gdiplus::Status::Ok;
}

bool NeoStrFontCacheGDIPlus::EmbedFontFromMemory(const char* pData, const size_t sz) {
    auto gdipRet = pFontCollection->AddMemoryFont((const PBYTE)pData, (DWORD)sz);

    return gdipRet == Gdiplus::Status::Ok;
}
