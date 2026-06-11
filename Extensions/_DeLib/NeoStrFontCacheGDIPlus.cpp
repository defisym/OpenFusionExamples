#include "NeoStrFontCacheGDIPlus.h"

NeoStrFontInfoGDIPlus::NeoStrFontInfoGDIPlus(const LOGFONT& logFont) {
    this->logFont = logFont;
}

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

bool NeoStrFontCacheGDIPlus::HasFont(const NeoStrFontInfo& fontInfo) const {
    const auto& logFont = static_cast<const NeoStrFontInfoGDIPlus&>(fontInfo).logFont;
    return false;
}

NeoStrFont NeoStrFontCacheGDIPlus::GetFont(const NeoStrFontInfo& fontInfo) const {
    const auto& logFont = static_cast<const NeoStrFontInfoGDIPlus&>(fontInfo).logFont;
    return GetFontPointerWithCache(logFont);
}

CharSize NeoStrFontCacheGDIPlus::GetCharSize(const wchar_t wChar,
    const NeoStrFontInfo& fontInfo) {
    const auto& logFont = static_cast<const NeoStrFontInfoGDIPlus&>(fontInfo).logFont;
    return GetCharSizeWithCache(wChar, logFont);
}

Font* NeoStrFontCacheGDIPlus::GetFontPointer(const LOGFONT& logFont) const {
    const auto bFound = FontCollectionHasFont(logFont.lfFaceName, pFontCollection);

    return new Font(logFont.lfFaceName,
        (float)abs(logFont.lfHeight),
        GetFontStyle(logFont),
        Gdiplus::UnitWorld,
        bFound ? pFontCollection : nullptr);
}

Font* NeoStrFontCacheGDIPlus::GetFontPointerWithCache(const LOGFONT& logFont) const {
    const auto logFontHash = LogFontHasher(logFont);
    const auto it = pFontCache->find(logFontHash);

    if (it != pFontCache->end()) {
        return it->second;
    }
    else {
        const auto newFont = GetFontPointer(logFont);
        (*pFontCache)[logFontHash] = newFont;

        return newFont;
    }
}

CharSize NeoStrFontCacheGDIPlus::GetCharSizeWithCache(const wchar_t wChar, const LOGFONT& logFont) {
    const auto logFontHash = LogFontHasher(logFont);
    const auto it = pCharSzCacheWithFont->find(logFontHash);

    if (it != pCharSzCacheWithFont->end()) {
        auto& cacheSecond = it->second;
        auto& charCache = cacheSecond.cache;

        const auto charIt = charCache.find(wChar);
        if (charIt != charCache.end()) {
            return charIt->second;
        }
        else {
            const auto sz = GetCharSizeRaw(wChar, cacheSecond.hdc);
            charCache[wChar] = sz;

            return sz;
        }
    }
    else {
#ifdef COUNT_GDI_OBJECT
        GDIObjectCounter objectCounter;

        objectCounter.UpdateObjectCount();
#endif
        pCharSzCacheWithFont->emplace(logFontHash, logFont);

#ifdef COUNT_GDI_OBJECT
        const auto count = objectCounter.objectCount;

        objectCounter.UpdateObjectCount();
        if (count < objectCounter.objectCount) {
            const auto info = std::format(L"Add when Cache\n");
            OutputDebugStringW(info.c_str());
        }
#endif
        return GetCharSizeWithCache(wChar, logFont);
    }
}

const CharSizeCacheItem& NeoStrFontCacheGDIPlus::GetCharSizeCacheItem(const LOGFONT& logFont) const {
    const auto logFontHash = LogFontHasher(logFont);
    auto it = pCharSzCacheWithFont->find(logFontHash);

    if (it != pCharSzCacheWithFont->end()) { return it->second; }

    return CharSizeCacheItem{};
}

bool NeoStrFontCacheGDIPlus::FontCollectionHasFont(const LPCWSTR pFaceName, const Gdiplus::FontCollection* pFontCollection) {
    if (pFontCollection == nullptr) {
        return false;
    }

    const int n = pFontCollection->GetFamilyCount();

    if (n == 0) {
        return false;
    }

    FontFamily* ffs = new FontFamily[n];

    int found;
    pFontCollection->GetFamilies(n, ffs, &found);

    if (found == 0) {
        return false;
    }

    wchar_t name[LF_FACESIZE]{ 0 };

    const LANGID language = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);

    bool has = false;
    bool hasSuffixRegular = false;
    //bool hasSuffixNormal = false;

    const std::wstring withRegular = (std::wstring)pFaceName + (std::wstring)L" Regular";
    //std::wstring withNormal = (std::wstring)pFaceName + (std::wstring)L" Normal";

    for (int i = 0; i < n; i++) {
        has = false;
        hasSuffixRegular = false;
        //hasSuffixNormal = false;

        auto hasName = [&] (const LANGID language = (LANGID)0U) {
            memset(name, 0, LF_FACESIZE * sizeof(wchar_t));
            ffs[i].GetFamilyName(name, language);

            has |= (_wcsicmp(name, pFaceName) == 0);

            if (has) {
                return;
            }

            hasSuffixRegular |= (_wcsicmp(name, withRegular.c_str()) == 0);

            if (hasSuffixRegular) {
                return;
            }

            //hasSuffixNormal |= (_wcsicmp(name, withNormal.c_str()) == 0);
            };

        hasName();
        hasName(language);

        if (has || hasSuffixRegular/* || hasSuffixNormal*/) {
            //if (hasSuffixRegular) {
            //    wcscpy_s(pFaceName, LF_FACESIZE, withRegular.c_str());
            //}
            //if (hasSuffixNormal) {
            //	wcscpy_s(pFaceName, LF_FACESIZE, withNormal.c_str());
            //}

            break;
        }
    }

    delete[] ffs;

    return has || hasSuffixRegular/* || hasSuffixNormal*/;
}

int NeoStrFontCacheGDIPlus::GetFontStyle(const LOGFONT& logFont) {
    int fontStyle = Gdiplus::FontStyleRegular;

    if (logFont.lfWeight >= FW_SEMIBOLD) {
        fontStyle = fontStyle | Gdiplus::FontStyleBold;
    }
    if (logFont.lfItalic) {
        fontStyle = fontStyle | Gdiplus::FontStyleItalic;
    }
    if (logFont.lfUnderline) {
        fontStyle = fontStyle | Gdiplus::FontStyleUnderline;
    }
    if (logFont.lfStrikeOut) {
        fontStyle = fontStyle | Gdiplus::FontStyleStrikeout;
    }

    return fontStyle;
}

CharSize NeoStrFontCacheGDIPlus::GetCharSizeRaw(const wchar_t wChar, const HDC hdc) {
    SIZE sz = { 0,0 };
    GetTextExtentPoint32(hdc, &wChar, 1, &sz);

    // special: shouldn't have width to fix non-left align offset		
    if (wChar == L'\r' || wChar == L'\n' || wChar == L'\0') {
        sz.cx = 0;
    }
    // general: has both width & height
    else {

    }

    return *(CharSize*)&sz;
}

size_t NeoStrFontCacheGDIPlus::LogFontHasher(const LOGFONT& logFont) {
    constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
    constexpr auto HASHER_MOVE = [] (const size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); };

    size_t seed = 65535;

    seed ^= logFont.lfHeight + HASHER_MOVE(seed);
    seed ^= logFont.lfWidth + HASHER_MOVE(seed);
    seed ^= logFont.lfEscapement + HASHER_MOVE(seed);
    seed ^= logFont.lfOrientation + HASHER_MOVE(seed);
    seed ^= logFont.lfWeight + HASHER_MOVE(seed);
    seed ^= logFont.lfItalic + HASHER_MOVE(seed);
    seed ^= logFont.lfUnderline + HASHER_MOVE(seed);
    seed ^= logFont.lfStrikeOut + HASHER_MOVE(seed);
    seed ^= logFont.lfCharSet + HASHER_MOVE(seed);
    seed ^= logFont.lfOutPrecision + HASHER_MOVE(seed);
    seed ^= logFont.lfClipPrecision + HASHER_MOVE(seed);
    seed ^= logFont.lfQuality + HASHER_MOVE(seed);
    seed ^= logFont.lfPitchAndFamily + HASHER_MOVE(seed);

    for (auto i = 0; i < LF_FACESIZE; i++) {
        seed ^= logFont.lfFaceName[i] + HASHER_MOVE(seed);
    }

    return seed;
}
