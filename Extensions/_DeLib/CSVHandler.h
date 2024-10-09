// ReSharper disable CppInconsistentNaming
#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "StrNum.h"

namespace CSV {
    template<typename T>
    concept STR = std::is_same_v<std::remove_cvref_t<T>, std::string>
        || std::is_same_v<std::remove_cvref_t<T>, std::wstring>;

    template<STR T>
    struct GetViewType {};

    template<>
    struct GetViewType<std::string> {
        using ViewType = std::string_view;
        using CharType = char;

        constexpr static auto CHAR_STR_END = '\0';
        constexpr static auto CHAR_DELIMITER = ',';
        constexpr static auto CHAR_STR_QUOTATION = '\"';
        constexpr static auto STR_NEWLINE = "\r\n";
        constexpr static auto STR_RET = '\r';
        constexpr static auto STR_LINEFEED = '\n';

        static size_t GetLength(const CharType* pStr) { return strlen(pStr); }
    };

    template<>
    struct GetViewType<std::wstring> {
        using ViewType = std::wstring_view;
        using CharType = wchar_t;

        constexpr static auto CHAR_STR_END = L'\0';
        constexpr static auto CHAR_DELIMITER = L',';
        constexpr static auto CHAR_STR_QUOTATION = L'\"';
        constexpr static auto STR_NEWLINE = L"\r\n";
        constexpr static auto STR_RET = L'\r';
        constexpr static auto STR_LINEFEED = L'\n';

        static size_t GetLength(const CharType* pStr) { return wcslen(pStr); }
    };

    template<STR T>
    struct CSVHandler {
        using ViewType = typename GetViewType<T>::ViewType;        
        T escape;

        //process quota escape
        inline T EscapeDoubleToSingle(const ViewType& src) {
            escape.clear();
            escape.reserve(src.length());

            for (size_t pos = 0; pos != src.length(); pos++) {
                const bool end = (pos == src.length() - 1);

                const auto cur = src[pos];
                const auto next = !end ? src[pos + 1] : GetViewType<T>::CHAR_STR_END;

                escape += src[pos];

                if ((cur == GetViewType<T>::CHAR_STR_QUOTATION) && (next == GetViewType<T>::CHAR_STR_QUOTATION)) {
                    pos++;
                }
            }

            return escape;
        }

        inline T EscapeSingleToDouble(const ViewType& src) {
            escape.clear();
            escape.reserve(src.length());

            for (const auto& pos : src) {
                if (pos == GetViewType<T>::CHAR_STR_QUOTATION) { escape += pos; }
                escape += pos;
            }

            return escape;
        }
    };

    template<STR T>
    struct CSVParser :CSVHandler<T> {
        using NewLineCb = std::function<void()>;
        using NewItemCb = std::function<void(const typename GetViewType<T>::ViewType&)>;

        struct ParseOptions {
            bool bRemoveQuota = false;
            bool bEscapeDoubleQuota = false;
            bool bTrim = false;
        };

        // parse the given CSV string
        // you need to reset the data receiver first
        inline bool ParseCSV(const typename GetViewType<T>::CharType* pStr,
            const NewLineCb& newLineCb, const NewItemCb& newItemCb,
            const ParseOptions& opt = {}) {
            // protection
            if (pStr == nullptr) { return false; }
            if (newLineCb == nullptr) { return false; }
            if (newItemCb == nullptr) { return false; }

            const auto strLen = GetViewType<T>::GetLength(pStr);

            if (strLen == 0) { return false; }

            // reset
            newLineCb();

            // start phrase
            size_t tokenStart = 0;

            bool bInQuota = false;
            bool bDoubleQuota = false;

            for (size_t pos = 0; pos != strLen; pos++) {
                const auto curChar = pStr[pos];
                const auto nextChar = pStr[pos + 1];

                const bool bEnd = nextChar == GetViewType<T>::CHAR_STR_END;

                // quota
                if (!bInQuota && (curChar == GetViewType<T>::CHAR_STR_QUOTATION)) {
                    bInQuota = true;
                    continue;
                }

                if (bInQuota && (curChar == GetViewType<T>::CHAR_STR_QUOTATION)) {
                    //double quota
                    if (nextChar == GetViewType<T>::CHAR_STR_QUOTATION) {
                        bDoubleQuota = true;
                        pos++;
                        continue;
                    }

                    bInQuota = false;
                }

                //get token
                if (bInQuota) { continue; }
                
                const auto bNewLine = curChar == GetViewType<T>::STR_RET
                    && nextChar == GetViewType<T>::STR_LINEFEED;
                const auto bDelimiter = curChar == GetViewType<T>::CHAR_DELIMITER;

                if (!bEnd && !bNewLine && !bDelimiter) { continue; }

                // len is valid here, as curChar is not content unless it's end
                const auto tokenLen = pos - tokenStart + bEnd;
                const auto bRemoveQuota = opt.bRemoveQuota && pStr[tokenStart] == GetViewType<T>::CHAR_STR_QUOTATION;

                typename GetViewType<T>::ViewType token(pStr + tokenStart + bRemoveQuota,
                    tokenLen - 2 * bRemoveQuota);

                if (opt.bTrim) { token = GetTrimmedStr(token); }

                newItemCb(opt.bEscapeDoubleQuota && bDoubleQuota
                    ? CSVHandler<T>::EscapeDoubleToSingle(token)
                    : token);
                bDoubleQuota = false;

                // update token start
                pos += bNewLine;
                tokenStart = pos + 1;

                // handle newline
                if (bNewLine && tokenStart < strLen) { newLineCb(); }
            }

            return true;
        }
    };

    template<STR T>
    struct CSVBuilder :CSVHandler<T> {
        T result;

        inline CSVBuilder* Reset() {
            result.clear();

            return this;
        }

        struct BuildOptions {
            bool bAddQuota = false;
            bool bEscapeSingleQuota = false;
            bool bTrim = false;
        };

    private:
        inline CSVBuilder* AddNewItemWithQuote(const typename GetViewType<T>::ViewType& item,
            const BuildOptions& opt = {}) {
            if (opt.bAddQuota) { result += GetViewType<T>::CHAR_STR_QUOTATION; }
            result += item;
            if (opt.bAddQuota) { result += GetViewType<T>::CHAR_STR_QUOTATION; }

            result += GetViewType<T>::CHAR_DELIMITER;

            return this;
        }
    public:
        inline CSVBuilder* AddNewItem(typename GetViewType<T>::ViewType item,
            const BuildOptions& opt = {}) {
            if (opt.bTrim) { item = GetTrimmedStr(item); }

            return AddNewItemWithQuote(opt.bEscapeSingleQuota && item.find_first_of(GetViewType<T>::CHAR_STR_QUOTATION) != T::npos
                ? CSVHandler<T>::EscapeSingleToDouble(item)
                : item,
                opt);
        }

        inline CSVBuilder* AddNewLine() {
            if (result.back() == GetViewType<T>::CHAR_DELIMITER) { result.pop_back(); }
            result += GetViewType<T>::STR_NEWLINE;

            return this;
        }
    };
}