#pragma once

#include "Encryption.h"

#include <vector>

#define UTF8_SIGNATURE     "\xEF\xBB\xBF"

#define NEWLINE     L"\r\n"
#define TAB         L"\t"
#define SPACE       L" "

#define ALL         L".*"

#define BOOST

#ifndef BOOST

#include <regex>

using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;

using std::regex_match;
using std::regex_search;
using std::regex_replace;

using RegexFlag = std::regex_constants::syntax_option_type;
using std::regex_constants::icase;
using std::regex_constants::ECMAScript;
using std::regex_constants::optimize;

#else

#include <boost/regex.hpp>

using boost::wregex;
using boost::wsregex_token_iterator;
using boost::wsmatch;

using boost::regex_match;
using boost::regex_search;
using boost::regex_replace;

using RegexFlag = boost::regex_constants::syntax_option_type;
using boost::regex_constants::icase;
using boost::regex_constants::ECMAScript;
using boost::regex_constants::optimize;

#endif

constexpr auto RESERVE_DEFAULT = 50;
// usually the vec size is SplitSrcStrLength / RESERVE_MAGNUM
constexpr auto RESERVE_MAGNUM = 16;

constexpr auto DEFAULE_REGEXFLAG = ECMAScript | optimize;

class Split :
    public Encryption
{
private:
    //Unicode
    bool Unicode = true;

    //Converted str
    wchar_t* SplitSrcStr = nullptr;
    size_t SplitSrcStrLength = 0;

    std::wstring SplitDataStr;

    std::wstring MatchedStr;
    std::wstring ReplacedStr;

    std::vector<std::wstring> SubStringVec;

    //regex flags
    RegexFlag DefaultFlag = DEFAULE_REGEXFLAG;
    RegexFlag Flag = DefaultFlag;

    //Main regex, split lines
    wregex LineReg;
    std::wstring LineRegStr;

    //EmptyLine regex, remove empty lines
    wregex EmptyLineReg;
    bool RemoveEmptyLine = false;

    //Comment regex, remove comments
    wregex CommentReg;
    bool RemoveCommnet = false;

    //Indent regex, escape SPACE and TAB at the beginning and ending of string
    wregex IndentReg;
    bool RemoveIndent = false;

    //Result
    std::vector<std::wstring> SplitStrVec;

    //Keyword that you can quick get it's pos in vec
    wregex KeyWordReg;
    bool KeyWord = false;

    //Keyword list
    std::vector<std::pair<size_t, std::wstring>> KeyWordPairVec;

    inline void Reserve(size_t size = RESERVE_DEFAULT) {
		SplitStrVec.reserve(size);
        SubStringVec.reserve(size);
        KeyWordPairVec.reserve(size);
    }

    //Convert
    inline size_t GetSize(const char* Src, size_t Len, UINT CodePage = CP_UTF8) {
        int Size = MultiByteToWideChar(CodePage, 0, Src, (int)Len, 0, 0);

        return (size_t)((Size > 0) ? Size : -1);
    }
    inline bool Convert(const char* Src, size_t SrcLen, const wchar_t* Des, size_t DesLen, UINT CodePage = CP_UTF8) {
        int Size = MultiByteToWideChar(CodePage, 0, Src, (int)SrcLen, (wchar_t*)Des, (int)DesLen);

        return (Size > 0);
    }

    inline void NewSplitSrc(size_t Len) {
        this->SplitSrcStrLength = Len + 1;
        this->SplitSrcStr = new wchar_t[this->SplitSrcStrLength];
        memset(this->SplitSrcStr, 0, sizeof(wchar_t) * (this->SplitSrcStrLength));
    }
    inline void ReleaseSplitSrcStr() {
        if (this->SplitSrcStr != nullptr) {
            delete[] this->SplitSrcStr;
            this->SplitSrcStr = nullptr;
        }
    }

public:
    Split();
    ~Split();

    void ResetSplit();
    
    inline void SetUnicode(bool Unicode) {
        this->Unicode = Unicode;
    }

    //load file and decrypt it
    void LoadFile(const wchar_t* FilePath, const wchar_t* Key, bool Unicode = true);
    void LoadFile(const std::wstring& FilePath, const std::wstring& Key, bool Unicode = true);

    //load data loaded by parent class
    void LoadData();
    //load data from byte str (convert to wchar)
    void LoadData(const char* Src);
    void LoadData(const char* Src, size_t Len);
    //load data from wstring
    void LoadData(const std::wstring& Src);
    //load data from wchar
    void LoadData(const wchar_t* Src);
    void LoadData(const wchar_t* Src, size_t Len);

    inline void InitRegexFlag() {
        this->DefaultFlag = DEFAULE_REGEXFLAG;
        this->Flag = DefaultFlag;
    }
    void ReSetRegexFlag();

    //E.g. SetRegexFlag(ECMAScript | icase);
    void SetRegexFlag(RegexFlag Flag);

    void SetCaseInsensitive(bool Enable);

    inline RegexFlag GetRegexFlag() {
        return this->Flag;
    }

    void InitSplit(const wchar_t* Split);
    void InitEmptyLine(const wchar_t* EnptyLine);
    void InitComment(const wchar_t* Comment);
    void InitIndent(const wchar_t* Indent);
    void InitKeyWord(const wchar_t* KeyWord);

    void InitRegex(const wchar_t* Split, const wchar_t* EnptyLine, const wchar_t* Comment, const wchar_t* Indent, const wchar_t* KeyWord);

    void SplitData();

    inline const wchar_t* GetSplitData() {
        if (this->SplitDataStr.empty()) {
            return this->SplitSrcStr;
        }
        else{
            return this->SplitDataStr.c_str();
        }
    }

    //Replace string
    inline const wchar_t* ReplaceStr(const wchar_t* SubStr, const wchar_t* Replace) {
        wregex SubString(SubStr, this->Flag);
        this->ReplacedStr = regex_replace(this->SplitDataStr, SubString, Replace);
        return this->ReplacedStr.c_str();
    }
	
    inline const wchar_t* ReplaceStr(const wchar_t* Src, const wchar_t* SubStr, const wchar_t* Replace) {
        return ReplaceStr(std::wstring(Src), SubStr, Replace);        
    }
    inline const wchar_t* ReplaceStr(const std::wstring& Src, const wchar_t* SubStr, const wchar_t* Replace) {
        wregex SubString(SubStr, this->Flag);
        this->ReplacedStr = regex_replace(Src, SubString, Replace);
        return this->ReplacedStr.c_str();
    }

    //Get the pos of string that occurs sub times, default = 0
    //Returns -1 if no match or not enough occurs
    //Set sub to -1 to get the last occurence's pos
    int GetSubStringPos(const wchar_t* SubStr, size_t Sub = 0);
    int GetSubStringPos(const wchar_t* Src, const wchar_t* SubStr, size_t Sub = 0);
    int GetSubStringPos(const std::wstring& Src, const wchar_t* SubStr, size_t Sub = 0, bool SaveAll = false);

    void GetAllSubString(const wchar_t* SubStr);
    void GetAllSubString(const wchar_t* Src, const wchar_t* SubStr);
    void GetAllSubString(const std::wstring& Src, const wchar_t* SubStr);

    //Type == true search
    //Type == false match
    inline bool StringMatchRegex(const wchar_t* SubStr, bool Type = false) {
        return this->StringMatchRegex(this->SplitDataStr, SubStr,Type);
    }
    inline bool StringMatchRegex(const wchar_t* Src, const wchar_t* SubStr, bool Type = false) {
        return this->StringMatchRegex(std::wstring(Src), SubStr, Type);
    }
    inline bool StringMatchRegex(const std::wstring& Src, const wchar_t* SubStr, bool Type = false) {
        return Type ? regex_search(Src, wregex(SubStr, this->Flag)) : regex_match(Src, wregex(SubStr, this->Flag));
    }

    inline size_t GetSubStringSize() {
        return this->SubStringVec.size();
    }
    inline const std::vector<std::wstring>* GetSubStringVec() {
        return &this->SubStringVec;
    }
    inline const wchar_t* GetSubString(size_t Pos) {
        return (Pos < this->SubStringVec.size()) && (Pos >= 0) ? this->SubStringVec[Pos].c_str() : nullptr;
    }

    inline const wchar_t* GetMatchResult() {
        return this->MatchedStr.c_str();
    }
    inline const wchar_t* GetMatchResult(const wchar_t* SubStr, size_t Sub) {
        this->GetSubStringPos(this->SplitDataStr, SubStr, Sub);
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

    const wchar_t* GetNextKeyWord(size_t StartPos);
    const wchar_t* GetNextKeyWord(size_t StartPos, const wchar_t* KeyWord);

    int GetNextKeyWordPos(size_t StartPos);
    int GetNextKeyWordPos(size_t StartPos, const wchar_t* KeyWord);

    inline size_t GetKeyWordPairVecSize() {
        return this->KeyWordPairVec.size();
    }
    inline const wchar_t* GetKeyWord(size_t Pos) {
        return (Pos < this->KeyWordPairVec.size()) && (Pos >= 0) ? this->KeyWordPairVec[Pos].second.c_str() : nullptr;
    }
    inline int GetKeyWordPos(size_t Pos) {
        return (Pos < this->KeyWordPairVec.size()) && (Pos >= 0) ? (int)this->KeyWordPairVec[Pos].first : -1;
    }

    inline void SetSplitReserve(size_t Size) {
        this->SplitStrVec.reserve(Size);
    }

    inline size_t GetSplitSize() {
        return this->SplitStrVec.size();
    }
    inline const std::vector<std::wstring>* GetSplit() {
        return &this->SplitStrVec;
    }
    inline const wchar_t* GetSplitVec(size_t Pos) {
        return (Pos < this->SplitStrVec.size()) && (Pos >= 0) ? this->SplitStrVec[Pos].c_str() : nullptr;
    }
    // the same as GetSplit(size_t Pos)
    inline const wchar_t* operator[](size_t Pos) {
        return (Pos < this->SplitStrVec.size()) && (Pos >= 0) ? this->SplitStrVec[Pos].c_str() : nullptr;
    };
};

//if you input \r\n in MMF, fusion will convert it to \\r\\n, which not match \r\n, so we convert it back here
inline const std::wstring NewLineEscape(const wchar_t* Src) {
    wregex NewLineEscape(L"(\\\\r\\\\n)");
    return regex_replace(std::wstring(Src), NewLineEscape, L"\r\n").c_str();

}