#pragma once

#include <vector>
#include <ranges>

#include "Encryption.h"
#include "GeneralDefinition.h"

#define NEWLINE     L"\r\n"
#define TAB         L"\t"
#define SPACE       L" "

#define ALL         L".*"

//Boost
#define BOOST
#include "RegexHelper.h"

constexpr auto RESERVE_DEFAULT = 50;
// usually the vec size is SplitSrcStrLength / RESERVE_MAGNUM
constexpr auto RESERVE_MAGNUM = 16;

constexpr auto DEFAULT_REGEXFLAG = ECMAScript | optimize;

struct SplitResult {
    std::wstring SplitDataStr;
    std::vector<std::wstring> SplitStrVec;
    std::vector<std::pair<size_t, std::wstring>> KeyWordPairVec;
};

class Split {
    //Unicode
    bool Unicode = true;

    std::wstring SplitSrcStr;
    std::wstring SplitDataStr;

    std::wstring ReplacedStr;

    //regex flags
    RegexFlag DefaultFlag = DEFAULT_REGEXFLAG;
    RegexFlag Flag = DefaultFlag;

    //Main regex, split lines
    wregex LineReg;
    std::wstring LineRegStr;
    bool bLineReg = false;

    //EmptyLine regex, remove empty lines
    wregex EmptyLineReg;
    std::wstring EmptyLineRegStr;
    bool RemoveEmptyLine = false;

    //Comment regex, remove comments
    wregex CommentReg;
    std::wstring CommentRegStr;
    bool RemoveComment = false;

    //Indent regex, escape SPACE and TAB at the beginning and ending of string
    wregex IndentReg;
    std::wstring IndentRegStr;
    bool RemoveIndent = false;

    //Result
    std::vector<std::wstring> SplitStrVec;

    //Keyword that you can quick get it's pos in vec
    wregex KeyWordReg;
    std::wstring KeyWordRegStr;
    bool KeyWord = false;

    //Keyword list
    using KeyWordPair = std::pair<size_t, std::wstring>;

    std::vector<KeyWordPair> KeyWordPairVec;

    //Match
    std::wstring MatchedStr;

    std::vector<std::wstring> SubStringVec;

    inline void Reserve(size_t size = RESERVE_DEFAULT) {
        SplitStrVec.reserve(size);
        SubStringVec.reserve(size);
        KeyWordPairVec.reserve(size);
    }

public:
    Split() = default;
    ~Split() = default;
    Split(const Split& obj) = default;
    Split& operator= (const Split& obj) = default;

    void ResetSplit();

    inline void SetUnicode(bool bUnicode) { this->Unicode = bUnicode; }

    //load file and decrypt it
    bool LoadFile(const wchar_t* pFilePath, const wchar_t* pKey, bool bUnicode = true);
    bool LoadFile(const std::wstring& FilePath, const std::wstring& Key, bool bUnicode = true);

    // load data and handle unicode
    static bool LoadData(std::wstring& output,
        const char* pSrc, size_t len,
        bool& bUnicode) {
        return ::LoadData(output, pSrc, len, bUnicode);
    }
    // save data and handle unicode    
    static bool SaveData(std::string& output,
        const wchar_t* pSrc, const size_t len,
        bool bUnicode = true){
        return ::SaveData(output, pSrc, len, bUnicode);
    }

    //load data from string
    bool LoadData(const std::string& Src);
    //load data from byte str (convert to wchar)
    bool LoadData(const char* Src);
    bool LoadData(const char* Src, size_t Len);     //Actual loading function
    //load data from wstring
    bool LoadData(const std::wstring& Src);
    //load data from wchar
    bool LoadData(const wchar_t* Src);
    bool LoadData(const wchar_t* Src, size_t Len);  //Actual loading function

    inline void InitRegexFlag() {
        this->DefaultFlag = DEFAULT_REGEXFLAG;
        this->Flag = DefaultFlag;
    }
    void ResetRegexFlag();

    //E.g. SetRegexFlag(ECMAScript | icase);
    void SetRegexFlag(RegexFlag Flag);

    void SetCaseInsensitive(bool Enable);

    inline RegexFlag GetRegexFlag() const { return this->Flag; }

    void InitSplit(const wchar_t* Split);
    void InitEmptyLine(const wchar_t* EmptyLine);
    void InitComment(const wchar_t* Comment);
    void InitIndent(const wchar_t* Indent);
    void InitKeyWord(const wchar_t* pKeyWord);

    void InitRegex(const wchar_t* Split,
        const wchar_t* EmptyLine,
        const wchar_t* Comment,
        const wchar_t* Indent,
        const wchar_t* pKeyWord);

    void SplitData();

    [[nodiscard]] size_t GetHash() const;
    void GetResult(SplitResult* pSplitResult) const;
    void SetResult(const SplitResult* pSplitResult);

    inline const wchar_t* GetSplitData() const {
        return this->SplitDataStr.empty()
            ? this->SplitSrcStr.c_str()
            : this->SplitDataStr.c_str();
    }

    //Replace string
    inline const wchar_t* ReplaceStr(const wchar_t* SubStr, const wchar_t* Replace) {
        return ReplaceStr(this->GetSplitData(), Replace, Replace);
    }
    inline const wchar_t* ReplaceStr(const wchar_t* Src, const wchar_t* SubStr, const wchar_t* Replace) {
        return ReplaceStr(std::wstring(Src), SubStr, Replace);
    }
    inline const wchar_t* ReplaceStr(const std::wstring& Src, const wchar_t* SubStr, const wchar_t* Replace) {
        const wregex SubString(SubStr, this->Flag);
        this->ReplacedStr = regex_replace(Src, SubString, Replace);
        return this->ReplacedStr.c_str();
    }

    //Get the pos of string that occurs sub times, default = 0
    //Returns -1 if no match or not enough occurs
    //Set sub to -1 to get the last occurrence's pos
    int GetSubStringPos(const wchar_t* SubStr, size_t Sub = 0);
    int GetSubStringPos(const wchar_t* Src, const wchar_t* SubStr, size_t Sub = 0);
    int GetSubStringPos(const std::wstring& Src, const wchar_t* SubStr, size_t Sub = 0, bool SaveAll = false);

    void GetAllSubString(const wchar_t* SubStr);
    void GetAllSubString(const wchar_t* Src, const wchar_t* SubStr);
    void GetAllSubString(const std::wstring& Src, const wchar_t* SubStr);

    //Type == true search
    //Type == false match
    inline bool StringMatchRegex(const wchar_t* SubStr, bool Type = false) const {
        return this->StringMatchRegex(this->GetSplitData(), SubStr, Type);
    }
    inline bool StringMatchRegex(const wchar_t* Src, const wchar_t* SubStr, bool Type = false) const {
        return this->StringMatchRegex(std::wstring(Src), SubStr, Type);
    }
    inline bool StringMatchRegex(const std::wstring& Src, const wchar_t* SubStr, bool Type = false) const {
        return Type ? regex_search(Src, wregex(SubStr, this->Flag)) : regex_match(Src, wregex(SubStr, this->Flag));
    }

    inline size_t GetSubStringSize() const {
        return this->SubStringVec.size();
    }
    inline const std::vector<std::wstring>* GetSubStringVec() const {
        return &this->SubStringVec;
    }
    inline const wchar_t* GetSubString(size_t Pos) const {
        return Pos < this->SubStringVec.size() ? this->SubStringVec[Pos].c_str() : nullptr;
    }

    inline const wchar_t* GetMatchResult() const {
        return this->MatchedStr.c_str();
    }
    inline const wchar_t* GetMatchResult(const wchar_t* SubStr, size_t Sub) {
        this->GetSubStringPos(this->GetSplitData(), SubStr, Sub);
        return this->MatchedStr.c_str();
    }
    inline const wchar_t* GetMatchResult(const wchar_t* Src, const wchar_t* SubStr, size_t Sub) {
        this->GetSubStringPos(Src, SubStr, Sub);
        return this->MatchedStr.c_str();
    }
    inline const wchar_t* GetMatchResult(const std::wstring& Src, const wchar_t* SubStr, size_t Sub) {
        this->GetSubStringPos(Src, SubStr, Sub);
        return this->MatchedStr.c_str();
    }

    [[nodiscard]] const wchar_t* GetNextKeyWord(size_t StartPos) const;
    const wchar_t* GetNextKeyWord(size_t StartPos, const wchar_t* pKeyWord) const;

    [[nodiscard]] int GetNextKeyWordPos(size_t StartPos) const;
    int GetNextKeyWordPos(size_t startPos, const wchar_t* pKeyWord) const;

    inline size_t GetKeyWordPairVecSize() const {
        return this->KeyWordPairVec.size();
    }
    inline const std::vector<KeyWordPair>* GetKeyWordVec() const {
        return &this->KeyWordPairVec;
    }
    inline const wchar_t* GetKeyWord(size_t Pos) const {
        return Pos < this->KeyWordPairVec.size() ? this->KeyWordPairVec[Pos].second.c_str() : nullptr;
    }
    inline int GetKeyWordPos(size_t Pos) const {
        return Pos < this->KeyWordPairVec.size() ? static_cast<int>(this->KeyWordPairVec[Pos].first) : -1;
    }

    inline void SetSplitReserve(size_t Size) {
        this->SplitStrVec.reserve(Size);
    }

    inline size_t GetSplitSize() const {
        return this->SplitStrVec.size();
    }
    inline const std::vector<std::wstring>* GetSplitVec() const {
        return &this->SplitStrVec;
    }
    inline const wchar_t* GetSplit(size_t Pos) const {
        return Pos < this->SplitStrVec.size() ? this->SplitStrVec[Pos].c_str() : nullptr;
    }
    inline const wchar_t* operator[](size_t Pos) const {
        return GetSplit(Pos);
    }

    bool InsertFile(const Split& newFile,
        size_t pos, bool bReplace);
    bool InsertFile(const wchar_t* pFilePath, const wchar_t* pKey, const bool bUnicode,
        const size_t pos, const bool bReplace) {
        Split newFile = *this;
        if (!newFile.LoadFile(pFilePath, pKey, bUnicode)) {
            return false;
        }

        return InsertFile(newFile, pos, bReplace);
    }
    bool InsertFile(const std::wstring& filePath, const std::wstring& key, const bool bUnicode,
        const size_t pos, const bool bReplace) {
        return InsertFile(filePath.c_str(), key.c_str(), bUnicode, pos, bReplace);
    }
};

//if you input \r\n in MMF, fusion will convert it to \\r\\n, which not match \r\n, so we convert it back here
inline std::wstring NewLineEscape(const wchar_t* Src) {
    const wregex NewLineEscape(L"(\\\\r\\\\n)");
    return regex_replace(std::wstring(Src), NewLineEscape, L"\r\n");
}
