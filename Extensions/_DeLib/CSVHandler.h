// ReSharper disable CppInconsistentNaming
#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "StrNum.h"

namespace CSV {
    template<StringConcept StringType>
    struct SpecialChar {};

    template<>
    struct SpecialChar<std::string> {    
        constexpr static auto CHAR_STR_END = '\0';
        constexpr static auto CHAR_DELIMITER = ',';
        constexpr static auto CHAR_STR_QUOTATION = '\"';
        constexpr static auto STR_NEWLINE = "\r\n";
        constexpr static auto STR_RET = '\r';
        constexpr static auto STR_LINEFEED = '\n';
    };

    template<>
    struct SpecialChar<std::wstring> {
        constexpr static auto CHAR_STR_END = L'\0';
        constexpr static auto CHAR_DELIMITER = L',';
        constexpr static auto CHAR_STR_QUOTATION = L'\"';
        constexpr static auto STR_NEWLINE = L"\r\n";
        constexpr static auto STR_RET = L'\r';
        constexpr static auto STR_LINEFEED = L'\n';
    };

    template<StringConcept StringType>
    struct CSVHandler {
        using ViewType = typename ViewType<StringType>::Type;
        StringType escape;

        //process quota escape
        inline StringType EscapeDoubleToSingle(const ViewType& src) {
            escape.clear();
            escape.reserve(src.length());

            for (size_t pos = 0; pos != src.length(); pos++) {
                const bool end = (pos == src.length() - 1);

                const auto cur = src[pos];
                const auto next = !end ? src[pos + 1] : SpecialChar<StringType>::CHAR_STR_END;

                escape += src[pos];

                if ((cur == SpecialChar<StringType>::CHAR_STR_QUOTATION) && (next == SpecialChar<StringType>::CHAR_STR_QUOTATION)) {
                    pos++;
                }
            }

            return escape;
        }

        inline StringType EscapeSingleToDouble(const ViewType& src) {
            escape.clear();
            escape.reserve(src.length());

            for (const auto& pos : src) {
                if (pos == SpecialChar<StringType>::CHAR_STR_QUOTATION) { escape += pos; }
                escape += pos;
            }

            return escape;
        }
    };

    template<StringConcept StringType>
    struct CSVParser :CSVHandler<StringType> {
        using NewLineCb = std::function<void()>;
        using NewItemCb = std::function<void(const typename ViewType<StringType>::Type&)>;

        struct ParseOptions {
            bool bRemoveQuota = false;
            bool bEscapeDoubleQuota = false;
            bool bTrim = false;
        };

        // parse the given CSV string
        // you need to reset the data receiver first
        inline bool ParseCSV(const typename CharType<StringType>::Type* pStr,
            const NewLineCb& newLineCb, const NewItemCb& newItemCb,
            const ParseOptions& opt = {}) {
            // protection
            if (pStr == nullptr) { return false; }
            if (newLineCb == nullptr) { return false; }
            if (newItemCb == nullptr) { return false; }

            const auto strLen = CharType<StringType>::Length(pStr);

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

                const bool bEnd = nextChar == SpecialChar<StringType>::CHAR_STR_END;

                // quota
                if (!bInQuota && (curChar == SpecialChar<StringType>::CHAR_STR_QUOTATION)) {
                    bInQuota = true;
                    continue;
                }

                if (bInQuota && (curChar == SpecialChar<StringType>::CHAR_STR_QUOTATION)) {
                    //double quota
                    if (nextChar == SpecialChar<StringType>::CHAR_STR_QUOTATION) {
                        bDoubleQuota = true;
                        pos++;
                        continue;
                    }

                    bInQuota = false;
                }

                //get token
                if (bInQuota) { continue; }
                
                const auto bNewLine = curChar == SpecialChar<StringType>::STR_RET
                    && nextChar == SpecialChar<StringType>::STR_LINEFEED;
                const auto bDelimiter = curChar == SpecialChar<StringType>::CHAR_DELIMITER;

                if (!bEnd && !bNewLine && !bDelimiter) { continue; }

                // len is valid here, as curChar is not content unless it's end
                const auto tokenLen = pos - tokenStart + bEnd;
                const auto bRemoveQuota = opt.bRemoveQuota && pStr[tokenStart] == SpecialChar<StringType>::CHAR_STR_QUOTATION;

                typename SpecialChar<StringType>::ViewType token(pStr + tokenStart + bRemoveQuota,
                    tokenLen - 2 * bRemoveQuota);

                if (opt.bTrim) { token = GetTrimmedStr(token); }

                newItemCb(opt.bEscapeDoubleQuota && bDoubleQuota
                    ? CSVHandler<StringType>::EscapeDoubleToSingle(token)
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

    template<StringConcept StringType>
    struct CSVBuilder :CSVHandler<StringType> {
        StringType result;

        inline CSVBuilder* Reset() {
            result.clear();

            return this;
        }

        inline CSVBuilder* ReAlloc(size_t sz) {
            result.reserve(sz);

            return this;
        }

        struct BuildOptions {
            bool bAddQuota = false;
            bool bEscapeSingleQuota = false;
            bool bTrim = false;
        };

    private:
        inline CSVBuilder* AddNewItemWithQuote(const typename ViewType<StringType>::Type& item,
            const BuildOptions& opt = {}) {
            if (opt.bAddQuota) { result += SpecialChar<StringType>::CHAR_STR_QUOTATION; }
            result += item;
            if (opt.bAddQuota) { result += SpecialChar<StringType>::CHAR_STR_QUOTATION; }

            result += SpecialChar<StringType>::CHAR_DELIMITER;

            return this;
        }
    public:
        inline CSVBuilder* AddNewItem(typename ViewType<StringType>::Type item,
            const BuildOptions& opt = {}) {
            if (opt.bTrim) { item = GetTrimmedStr(item); }

            return AddNewItemWithQuote(opt.bEscapeSingleQuota && item.find_first_of(SpecialChar<StringType>::CHAR_STR_QUOTATION) != StringType::npos
                ? CSVHandler<StringType>::EscapeSingleToDouble(item)
                : item,
                opt);
        }

        inline CSVBuilder* AddNewLine() {
            if (result.back() == SpecialChar<StringType>::CHAR_DELIMITER) { result.pop_back(); }
            result += SpecialChar<StringType>::STR_NEWLINE;

            return this;
        }
    };
}