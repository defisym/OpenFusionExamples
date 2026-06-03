#pragma once

#include <map>
#include <string>
#include <format>

//Boost
#define USE_BOOST_REGEX
#include "RegexHelper.h"

struct WordBreakHandler {
    wchar_t regstr[2] = { L'\0', L'\0' };

    wregex* pWordBreak = nullptr;
    wregex* pEmpty = nullptr;
    wregex* pEmptyStr = nullptr;

    using RegexCache = std::map<wchar_t, bool>;

    RegexCache WordBreakCache;
    RegexCache EmptyCache;

    // reference
    // previously a complex regex was used
    //  https://www.jianshu.com/p/fcbc5cd06f39
    //  filter -> match == no CJK, may miss some part, so add [a-zA-Z]
    //  L"[a-zA-Z\\u2E80-\\uA4CF\\uF900-\\uFAFF\\uFE10-\\uFE1F\\uFE30-\\uFE4F\\uFF00-\\uFFEF]"
    inline std::wstring BuildWordBreakRegex() {
        // not word
        std::wstring notWordRegStr = {};

        notWordRegStr += L"\\x{D800}-\\x{DFFF}";        // surrogate

        notWordRegStr += L"\\x{4E00}-\\x{9FFF}";        // CJK Unified Ideographs
        notWordRegStr += L"\\x{3040}-\\x{30FF}";        // Hiragana + Katakana
        //notWordRegStr += L"\\x{AC00}-\\x{D7AF}";        // Hangul

        // word
        std::wstring wordRegStr = {};

        wordRegStr += L"0-9";                           // number
        wordRegStr += L"a-zA-Z";                        // Basic Latin
        wordRegStr += L"\\x{00C0}-\\x{024F}";           // Latin-1 / Extended
        wordRegStr += L"\\x{0370}-\\x{03FF}";           // Greek
        wordRegStr += L"\\x{0400}-\\x{04FF}";           // Cyrillic
        wordRegStr += L"\\x{AC00}-\\x{D7AF}";           // Hangul

        return std::format(L"(?![{}])[{}]", notWordRegStr, wordRegStr);
    }

#ifdef  _DEBUG
    // check a set of characters match regex
    //wchar_t wChars[] = { L'a',L'b', L'c', L'd' };
    //auto bMatch = CheckRegex(wChars, wcslen(wChars), pRegex);
    inline bool CheckRegex(const wchar_t* wChars, const size_t sz,
        wregex* pRegex) {
        auto bRet = true;
        wchar_t regstr[2] = { L'\0', L'\0' };

        for (size_t i = 0; i < sz; i++) {
            regstr[0] = wChars[i];
            bRet &= regex_match(regstr, *pRegex);
        }

        return bRet;
    }
#endif //  _DEBUG

    WordBreakHandler() {
        constexpr auto defaultRegexFlag = static_cast<RegexFlag>(ECMAScript | optimize);

        this->pWordBreak = new wregex(BuildWordBreakRegex(), defaultRegexFlag);
        this->pEmpty = new wregex(L"[\\s]", defaultRegexFlag);
        this->pEmptyStr = new wregex(L"[\\s]*", defaultRegexFlag);
    }
    ~WordBreakHandler() {
        delete this->pWordBreak;
        this->pWordBreak = nullptr;

        delete this->pEmpty;
        this->pEmpty = nullptr;

        delete this->pEmptyStr;
        this->pEmptyStr = nullptr;
    }

    inline bool RegexCore(const wregex* pRegex, RegexCache* pCache, const wchar_t wChar) {
        regstr[0] = wChar;

        const auto it = pCache->find(wChar);

        if (it == pCache->end()) {
            const auto bRet = regex_match(regstr, *pRegex);
            (*pCache)[wChar] = bRet;

            return bRet;
        }
        else {
            return it->second;
        }

        //return regex_match(regstr, *pRegex);
    }

    inline bool WordBreak(const wchar_t wChar) {
        return RegexCore(this->pWordBreak, &this->WordBreakCache, wChar);
    }

    inline bool NotEmpty(const wchar_t wChar) {
        return !RegexCore(this->pEmpty, &this->EmptyCache, wChar);
    }

    inline bool NotEmpty(const wchar_t* pChar) const {
        return !regex_match(pChar, *this->pEmptyStr);
    }
};