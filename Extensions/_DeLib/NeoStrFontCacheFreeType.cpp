#include "NeoStrFontCacheFreeType.h"

#ifdef ENABLE_FREETYPE

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

bool NeoStrFontCacheFreeType::EmbedFontFromFile(const std::filesystem::path& filePath) {
    FILE* fp = nullptr;

    _wfopen_s(&fp, filePath.wstring().c_str(), L"rb");
    if (fp == nullptr) { return false; }

    fseek(fp, 0, SEEK_END);
    const auto sz = ftell(fp);
    rewind(fp);

    auto pData = std::make_unique<char[]>(sz);
    fread(pData.get(), sz, 1, fp);

    fclose(fp);

    return EmbedFontFromMemory(pData.get(), sz);
}

bool NeoStrFontCacheFreeType::EmbedFontFromMemory(const char* pData, const size_t sz) {
    const auto pBuf = reinterpret_cast<const FT_Byte*>(pData);

    FT_Face face = {};
    auto error = FT_New_Memory_Face(pCtx->library, pBuf, sz, -1, &face);
    if (error != FT_Err_Ok) { return false; }

    const auto num_faces = face->num_faces;
    FT_Done_Face(face);

    std::vector<FT_Face> faces = {};
    faces.reserve(num_faces);

    for (FT_Long i = 0; i < num_faces; i++) {
        auto error = FT_New_Memory_Face(pCtx->library, pBuf, sz, -1, &face);
        if (error != FT_Err_Ok) { continue; }

        faces.push_back(face);
    }

    if (faces.empty()) { return false; }
    pFontCache->push_back(std::move(faces));

    return true;
}

bool NeoStrFontCacheFreeType::HasFont(const NeoStrFontInfo& fontInfo) const {
    std::string name = {};

    for (const auto& faces : *pFontCache) {
        for (const auto& face : faces) {
            name = face->family_name;
        }
    }

    return false;
}

NeoStrFont NeoStrFontCacheFreeType::GetFont(const NeoStrFontInfo& fontInfo) const {
    return NeoStrFont();
}

CharSize NeoStrFontCacheFreeType::GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) {
    return CharSize();
}

#endif // ENABLE_FREETYPE