#include "NeoStrFontCacheFreeType.h"

void NeoStrFontCacheFreeType::SetContext(NeoStrContext* pCtx) {
    this->pCtx = dynamic_cast<NeoStrContextFreeType*>(pCtx);
}

bool NeoStrFontCacheFreeType::CacheValid() const {
    if (!NeoStrFontCache::CacheValid()) { return false; }
    if (pFontCache == nullptr) { return false; }

    return true;
}

void NeoStrFontCacheFreeType::Alloc() {
    NeoStrFontCacheFreeType::Release();

    NeoStrFontCache::Alloc();
    pFontCache = new FontCache{};
}

void NeoStrFontCacheFreeType::Release() {
    NeoStrFontCache::Release();

    delete pFontCache;
    pFontCache = nullptr;
}

bool NeoStrFontCacheFreeType::EmbedFontFromFile(const std::wstring& filePath) {    
    FT_Face face = {};
    const auto error = FT_New_Face(pCtx->library,
                     "/usr/share/fonts/truetype/arial.ttf",
                     0,
                     &face);
    if (error != FT_Err_Ok) { return false; }
    pFontCache->emplace_back(face);

    return true;
}

bool NeoStrFontCacheFreeType::EmbedFontFromMemory(const char* pData, const size_t sz) {
    FT_Face face = {};
    const auto error = FT_New_Memory_Face(pCtx->library,
                     reinterpret_cast<const FT_Byte*>(pData), sz,
                     0,
                     &face);
    if (error != FT_Err_Ok) { return false; }
    pFontCache->emplace_back(face);

    return true;
}

bool NeoStrFontCacheFreeType::HasFont(const NeoStrFontInfo& fontInfo) const {
    for (const auto& face : *pFontCache) {
        face->family_name;
    }

    return false;
}

NeoStrFont NeoStrFontCacheFreeType::GetFont(const NeoStrFontInfo& fontInfo) const {
    return NeoStrFont();
}

CharSize NeoStrFontCacheFreeType::GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) {
    return CharSize();
}
