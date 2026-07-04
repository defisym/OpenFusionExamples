#pragma once

#include <filesystem>

#include "NeoStrContext.h"
#include "NeoStrLayout.h"
#include "NeoStrWordBreakHandler.h"

using NeoStrFont = void*;
struct NeoStrFontInfo {};

struct NeoStrFontCache {
    using CharSizeCache = std::map<wchar_t, CharSize>;
    WordBreakHandler* pWordBreakCache = nullptr;

    enum class FontNameID :std::uint16_t {
        Family = 1,
        Subfamily = 2,
        FullName = 4,
        PreferredFamily = 16,
        PreferredSubfamily = 17,
    };

    using FontNameScore = std::uint16_t;
    consteval static FontNameScore GetFontNameScore(const FontNameID& type) {
        switch (type) {
        case FontNameID::PreferredFamily: return 50;
        case FontNameID::Family: return 80;
        case FontNameID::FullName: return 100;

        case FontNameID::Subfamily: return 20;
        case FontNameID::PreferredSubfamily: return 20;
        }

        return 0;   
    }

    struct FontName {
        std::vector<std::wstring> FamilyNames;
        std::vector<std::wstring> SubFamilyNames;
        std::vector<std::wstring> FullNames;
        std::vector<std::wstring> PreferredFamilyNames;
        std::vector<std::wstring> PreferredSubFamilyNames;

        static bool NameIDValid(const std::uint16_t id);
        bool HasName(const std::wstring& fontName) const;
        bool HasName(const FontName& fontName) const;

        FontNameScore GetMatchScore(const std::wstring& fontName) const;
    };

    using FontNames = std::vector<FontName>;
    FontNames embedFontList;

    virtual ~NeoStrFontCache() {};

    // some font lib init context globally
    // that's why this function is not an abstract function
    // or default impl with ptr
    virtual void SetContext(NeoStrContext* pCtx) {};

    virtual bool CacheValid() const;
    virtual void Alloc();
    virtual void Release();
    
    // read font names from file
    static FontName GetFontNamesFromFile(const std::filesystem::path& filePath);
    static FontName GetFontNamesFromMemory(const char* pData, const size_t sz);

    // return true if font names are added
    // do not call embed
    bool FontEmbed(const std::wstring& fontName) const;
    bool FontEmbed(const FontName& fontName) const;
    void AddEmbedFont(const FontName& fontName);

    const FontName* GetEmbedFontName(const std::wstring& fontName) const;

    virtual bool EmbedFontFromFile(const std::filesystem::path& filePath) = 0;
    virtual bool EmbedFontFromMemory(const char* pData, const size_t sz) = 0;

    virtual bool HasFont(const NeoStrFontInfo& fontInfo) const = 0;
    virtual NeoStrFont GetFont(const NeoStrFontInfo& fontInfo) const = 0;
    virtual CharSize GetCharSize(const wchar_t wChar, const NeoStrFontInfo& fontInfo) = 0;
};
