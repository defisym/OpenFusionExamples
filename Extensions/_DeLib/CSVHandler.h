#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "StrNum.h"

constexpr auto CHARSTREND = L'\0';
constexpr auto CHARDELIMITER = L',';
constexpr auto CHARSTRQUOTATIONMARK = L'\"';

struct CSVHandler {
	using NewLineCb = std::function<void()>;
	using NewItemCb = std::function<void(const std::wstring_view&)>;

	//process quota escape
	static inline std::wstring EscapeDoubleToSingle(const std::wstring_view& src) {
		std::wstring escape;
		escape.reserve(src.length());

		for (size_t pos = 0; pos != src.length(); pos++) {
			const bool end = (pos == src.length() - 1);

			const auto cur = src[pos];
			const auto next = !end ? src[pos + 1] : CHARSTREND;

			escape += src[pos];

			if ((cur == CHARSTRQUOTATIONMARK) && (next == CHARSTRQUOTATIONMARK)) {
				pos++;
			}
		}

		return escape;
	}

	static inline std::wstring EscapeSingleToDouble(const std::wstring_view& src) {
		std::wstring escape;
		escape.reserve(src.length());

		for (const auto& pos : src) {
			escape += pos;
			if (pos == CHARSTRQUOTATIONMARK) { escape += pos; }
		}

		return escape;
	}

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
		size_t tokenstart = 0, tokenend = 0;

		bool bInQuota = false;

		for (size_t pos = 0; pos != strLen; pos++) {
			const auto curChar = pStr[pos];
			const auto nextChar = pStr[pos + 1];

			const bool bEnd = nextChar == CHARSTREND;

			// quota
			if (!bInQuota && (curChar == CHARSTRQUOTATIONMARK)) {
				bInQuota = true;
				continue;
			}

			if (bInQuota && (curChar == CHARSTRQUOTATIONMARK)) {
				//double quota
				if (nextChar == CHARSTRQUOTATIONMARK) {
					pos++;
					continue;
				}

				bInQuota = false;
			}

			//get token
			if (bInQuota) { continue; }

			const auto bNewLine = curChar == L'\r' && nextChar == L'\n';
			const auto bDelimiter = curChar == CHARDELIMITER;

			if (!bEnd && !bNewLine && !bDelimiter) { continue; }

			// len is valid here, as curChar is not content unless it's end
			const auto tokenLen = pos - tokenstart + bEnd;
			const auto bRemoveQuota = opt.bRemoveQuota && pStr[tokenstart] == CHARSTRQUOTATIONMARK;

			std::wstring_view token(pStr + tokenstart + bRemoveQuota,
				tokenLen - 2 * bRemoveQuota);

			if (opt.bTrim) { token = GetTrimmedStr(token); }

			newItemCb(opt.bEscapeDoubleQuota
				? EscapeDoubleToSingle(token)
				: token);

			// update token start
			pos += bNewLine;
			tokenstart = pos + 1;

			// handle newline
			if (bNewLine && tokenstart < strLen) { newLineCb(); }
		}

		return true;
	}
};