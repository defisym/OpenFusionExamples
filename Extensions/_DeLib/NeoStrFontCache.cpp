#include "NeoStrFontCache.h"

bool NeoStrFontCache::FontName::NameIDValid(const std::uint16_t id) {
    do {
        if (id == static_cast<std::uint16_t>(FontNameID::Family)) { return true; }
        if (id == static_cast<std::uint16_t>(FontNameID::Subfamily)) { return true; }
        if (id == static_cast<std::uint16_t>(FontNameID::FullName)) { return true; }
        if (id == static_cast<std::uint16_t>(FontNameID::PreferredFamily)) { return true; }
        if (id == static_cast<std::uint16_t>(FontNameID::PreferredSubfamily)) { return true; }
    } while (false);

    return false;
}

bool NeoStrFontCache::FontName::HasName(const std::wstring& fontName) const {
    if (std::ranges::find(FullNames, fontName) != FullNames.end()) {
        return true;
    }
    if (std::ranges::find(FamilyNames, fontName) != FamilyNames.end()) {
        return true;
    }
    if (std::ranges::find(PreferredFamilyNames, fontName) != PreferredFamilyNames.end()) {
        return true;
    }

    return false;
}

bool NeoStrFontCache::FontName::HasName(const FontName& fontName) const {
    for (const auto& familyName : fontName.FamilyNames) {
        if (std::ranges::find(FamilyNames, familyName) != FamilyNames.end()) {
            return true;
        }
    }
    for (const auto& fullName : fontName.FullNames) {
        if (std::ranges::find(FullNames, fullName) != FullNames.end()) {
            return true;
        }
    }

    return false;
}


bool NeoStrFontCache::CacheValid() const {
    return pWordBreakCache != nullptr;
}

void NeoStrFontCache::Alloc() {
    NeoStrFontCache::Release();

    pWordBreakCache = new WordBreakHandler{};
}

void NeoStrFontCache::Release() {
    delete pWordBreakCache;
    pWordBreakCache = nullptr;
}

static constexpr std::uint16_t Swap16(std::uint16_t v) {
    return (v >> 8) | (v << 8);
}

static constexpr std::uint32_t Swap32(std::uint32_t v) {
    return
        ((v & 0x000000FF) << 24) |
        ((v & 0x0000FF00) << 8) |
        ((v & 0x00FF0000) >> 8) |
        ((v & 0xFF000000) >> 24);
}

NeoStrFontCache::FontName NeoStrFontCache::GetFontNamesFromFile(const std::filesystem::path& filePath) {
    FILE* fp = nullptr;

    _wfopen_s(&fp, filePath.wstring().c_str(), L"rb");
    if (fp == nullptr) { return FontName{}; }

    fseek(fp, 0, SEEK_END);
    const auto sz = ftell(fp);
    rewind(fp);

    auto pData = std::make_unique<char[]>(sz);
    fread(pData.get(), sz, 1, fp);

    fclose(fp);

    return GetFontNamesFromMemory(pData.get(), sz);
}

NeoStrFontCache::FontName NeoStrFontCache::GetFontNamesFromMemory(const char* pData, const size_t sz) {
    struct TT_OFFSET_TABLE {
        std::uint16_t uMajorVersion;
        std::uint16_t uMinorVersion;
        std::uint16_t uNumOfTables;
        std::uint16_t uSearchRange;
        std::uint16_t uEntrySelector;
        std::uint16_t uRangeShift;
    };

    struct TT_TABLE_DIRECTORY {
        char szTag[4];			//table name
        std::uint32_t uCheckSum;			//Check sum
        std::uint32_t uOffset;			//Offset from beginning of file
        std::uint32_t uLength;			//length of the table in bytes
    };

    struct TT_NAME_TABLE_HEADER {
       std::uint16_t uFSelector;		//format selector. Always 0
       std::uint16_t uNRCount;			//Name Records count
       std::uint16_t uStorageOffset;	//Offset for strings storage, from start of the table
    };

    struct TT_NAME_RECORD {
       std::uint16_t uPlatformID;
       std::uint16_t uEncodingID;
       std::uint16_t uLanguageID;
       std::uint16_t uNameID;
       std::uint16_t uStringLength;
       std::uint16_t uStringOffset; //from start of storage area
    };

    struct MemoryReader {
        MemoryReader(const char* data, const size_t size)
            : m_data(data), m_size(size), m_pos(0) { }

        bool Read(void* dst, const size_t sz) {
            if (m_pos + sz > m_size) { return false; }
            memcpy(dst, m_data + m_pos, sz);
            m_pos += sz;

            return true;
        }

        bool Seek(const size_t pos) {
            if (pos > m_size) { return false; }
            m_pos = pos;
            
            return true;
        }

        size_t Tell() const {
            return m_pos;
        }

    private:
        const char* m_data;
        size_t m_size;
        size_t m_pos;
    };

    auto fontName = FontName{};
    auto reader = MemoryReader{ pData, sz };

    TT_OFFSET_TABLE ttOffsetTable = {};
    reader.Read(&ttOffsetTable, sizeof(TT_OFFSET_TABLE));

    ttOffsetTable.uNumOfTables = Swap16(ttOffsetTable.uNumOfTables);
    ttOffsetTable.uMajorVersion = Swap16(ttOffsetTable.uMajorVersion);
    ttOffsetTable.uMinorVersion = Swap16(ttOffsetTable.uMinorVersion);

    //check is this is a true type font and the version is 1.0
    if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0) {
        return fontName;
    }

    TT_TABLE_DIRECTORY tblDir = {};
    bool bFound = false;

    std::string szTag = {};

    for (int i = 0; i < ttOffsetTable.uNumOfTables; i++) {
        reader.Read(&tblDir, sizeof(TT_TABLE_DIRECTORY));
        if (_stricmp(std::string(tblDir.szTag, 4).c_str(), "name") == 0) {
            bFound = true;
            tblDir.uLength = Swap32(tblDir.uLength);
            tblDir.uOffset = Swap32(tblDir.uOffset);
            break;
        }
    }

    if (!bFound) { return fontName; }

    reader.Seek(tblDir.uOffset);

    TT_NAME_TABLE_HEADER ttNTHeader = {};
    reader.Read(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));

    ttNTHeader.uNRCount = Swap16(ttNTHeader.uNRCount);
    ttNTHeader.uStorageOffset = Swap16(ttNTHeader.uStorageOffset);

    TT_NAME_RECORD ttRecord = {};
    for (int i = 0; i < ttNTHeader.uNRCount; i++) {
        reader.Read(&ttRecord, sizeof(TT_NAME_RECORD));

        ttRecord.uNameID = Swap16(ttRecord.uNameID);
        if (!FontName::NameIDValid(ttRecord.uNameID)) { continue; }

        ttRecord.uPlatformID = Swap16(ttRecord.uPlatformID);
        ttRecord.uEncodingID = Swap16(ttRecord.uEncodingID);
        ttRecord.uLanguageID = Swap16(ttRecord.uLanguageID);
        ttRecord.uStringLength = Swap16(ttRecord.uStringLength);
        ttRecord.uStringOffset = Swap16(ttRecord.uStringOffset);

        auto nPos = reader.Tell();
        reader.Seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset);

        //bug fix: see the post by SimonSays to read more about it
        auto sz = ttRecord.uStringLength + 1;
        auto lpszNameBuf = new char[2 * sz];
        memset(lpszNameBuf, 0, 2 * sz);

        reader.Read(lpszNameBuf, ttRecord.uStringLength);

        // 用 uint16_t 是因为 Linux 的 wchar_t 为4字节
        // 而 TTF 文件中的信息是以2字节 wchar_t 记录的
        if (ttRecord.uPlatformID == 3) {
            // 将 pbuf 转换为固定2字节 wchar_t 的字符串指针
            uint16_t* pwch = (uint16_t*)lpszNameBuf;

            // 计算2字节 wchar_t 的字符个数（原因同上）
            uint16_t cwch = ttRecord.uStringLength / sizeof(uint16_t);
            uint16_t wSz = cwch + 1;

            // 为与平台相关的目标 wchar_t 指针分配空间
            wchar_t* pwstr = new wchar_t[wSz];
            memset(pwstr, 0, wSz * sizeof(wchar_t));

            // 将固定2字节的 wchar_t 转换字节序后
            // 赋值给与平台相关的 wchar_t 字符数组
            for (uint32_t iwch = 0; iwch < cwch; iwch++) {
                pwstr[iwch] = Swap16(*pwch);
                pwch++;
            }

            //wcstombs(lpszNameBuf, pwstr, (ttRecord.uStringLength + sizeof(char)) * 2);
            const auto type = static_cast<FontNameID>(ttRecord.uNameID);
            switch (type) {
            case FontNameID::Family: fontName.FamilyNames.emplace_back(pwstr); break;
            case FontNameID::Subfamily: fontName.SubFamilyNames.emplace_back(pwstr); break;
            case FontNameID::FullName: fontName.FullNames.emplace_back(pwstr); break;
            case FontNameID::PreferredFamily: fontName.PreferredFamilyNames.emplace_back(pwstr); break;
            case FontNameID::PreferredSubfamily: fontName.PreferredSubFamilyNames.emplace_back(pwstr); break;
            }

            delete[] pwstr;
        }

        delete[] lpszNameBuf;
        reader.Seek(nPos);
    }

    return fontName;
}

decltype(NeoStrFontCache::FontNames{}.cend()) NeoStrFontCache::GetEmbedFontNameIt(const std::wstring& fontName) const {
    for (auto it = embedFontList.cbegin(); it != embedFontList.cend(); it++) {
        if (it->HasName(fontName)) { return it; }
    }

    return embedFontList.cend();
}

bool NeoStrFontCache::FontEmbed(const std::wstring& fontName) const {
    for (const auto& fontNameItem : embedFontList) {
        if (fontNameItem.HasName(fontName)) { return true; }
    }

    return false;
}

bool NeoStrFontCache::FontEmbed(const FontName& fontName) const {
    for (const auto& fontNameItem : embedFontList) {
        if (fontNameItem.HasName(fontName)) { return true; }
    }

    return false;
}

void NeoStrFontCache::AddEmbedFont(const FontName& fontName) {
    embedFontList.push_back(fontName);
}
