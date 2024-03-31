// ReSharper disable CppInconsistentNaming
#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "StrNum.h"

constexpr auto CHAR_STR_END = L'\0';
constexpr auto CHAR_DELIMITER = L',';
constexpr auto CHAR_STR_QUOTATION = L'\"';
constexpr auto STR_NEWLINE = L"\r\n";

struct CSVHandler {
	//process quota escape
	static inline std::wstring EscapeDoubleToSingle(const std::wstring_view& src) {
		std::wstring escape;
		escape.reserve(src.length());

		for (size_t pos = 0; pos != src.length(); pos++) {
			const bool end = (pos == src.length() - 1);

			const auto cur = src[pos];
			const auto next = !end ? src[pos + 1] : CHAR_STR_END;

			escape += src[pos];

			if ((cur == CHAR_STR_QUOTATION) && (next == CHAR_STR_QUOTATION)) {
				pos++;
			}
		}

		return escape;
	}

	static inline std::wstring EscapeSingleToDouble(const std::wstring_view& src) {
		std::wstring escape;
		escape.reserve(src.length());

		for (const auto& pos : src) {
			if (pos == CHAR_STR_QUOTATION) { escape += pos; }
			escape += pos;
		}

		return escape;
	}
};

struct CSVParser :CSVHandler {
	using NewLineCb = std::function<void()>;
	using NewItemCb = std::function<void(const std::wstring_view&)>;

	struct ParseOptions {
		bool bRemoveQuota = false;
		bool bEscapeDoubleQuota = false;
		bool bTrim = false;
	};

	// parse the given CSV string
	// you need to reset the data receiver first
	static inline bool ParseCSV(const wchar_t* pStr,
		const NewLineCb& newLineCb, const NewItemCb& newItemCb,
		const ParseOptions& opt = {}) {
		// protection
		if (pStr == nullptr) { return false; }
		if (newLineCb == nullptr) { return false; }
		if (newItemCb == nullptr) { return false; }

		const auto strLen = wcslen(pStr);

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

			const bool bEnd = nextChar == CHAR_STR_END;

			// quota
			if (!bInQuota && (curChar == CHAR_STR_QUOTATION)) {
				bInQuota = true;
				continue;
			}

			if (bInQuota && (curChar == CHAR_STR_QUOTATION)) {
				//double quota
				if (nextChar == CHAR_STR_QUOTATION) {
					bDoubleQuota = true;
					pos++;
					continue;
				}

				bInQuota = false;
			}

			//get token
			if (bInQuota) { continue; }

			const auto bNewLine = curChar == L'\r' && nextChar == L'\n';
			const auto bDelimiter = curChar == CHAR_DELIMITER;

			if (!bEnd && !bNewLine && !bDelimiter) { continue; }

			// len is valid here, as curChar is not content unless it's end
			const auto tokenLen = pos - tokenStart + bEnd;
			const auto bRemoveQuota = opt.bRemoveQuota && pStr[tokenStart] == CHAR_STR_QUOTATION;

			std::wstring_view token(pStr + tokenStart + bRemoveQuota,
				tokenLen - 2 * bRemoveQuota);

			if (opt.bTrim) { token = GetTrimmedStr(token); }

			newItemCb(opt.bEscapeDoubleQuota && bDoubleQuota
				? EscapeDoubleToSingle(token)
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

struct CSVBuilder :CSVHandler {
	std::wstring result;

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
	inline CSVBuilder* AddNewItemWithQuote(const std::wstring_view& item,
		const BuildOptions& opt = {}) {
		if (opt.bAddQuota) { result += CHAR_STR_QUOTATION; }
		result += item;
		if (opt.bAddQuota) { result += CHAR_STR_QUOTATION; }

		result += CHAR_DELIMITER;

		return this;
	}
public:
	inline CSVBuilder* AddNewItem(std::wstring_view item,
		const BuildOptions& opt = {}) {
		if (opt.bTrim) { item = GetTrimmedStr(item); }

		return AddNewItemWithQuote(opt.bEscapeSingleQuota && item.find_first_of(CHAR_STR_QUOTATION) != std::wstring::npos
			? EscapeSingleToDouble(item)
			: item,
			opt);
	}

	inline CSVBuilder* AddNewLine() {
		if (result.back() == CHAR_DELIMITER) { result.pop_back(); }
		result += STR_NEWLINE;

		return this;
	}
};