#pragma once

#include "StringTraits.h"

#include <cctype>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <string_view>
#include <vector>

// ------------------------------------------
// Copy string
// ------------------------------------------

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(wchar_t*& Tar, const wchar_t* Str) {
    const size_t total_length = std::wcslen(Str) + 1;
    auto replacement = new wchar_t[total_length];
    std::wmemcpy(replacement, Str, total_length);
    delete[] Tar;
    Tar = replacement;
}

inline void NewStr(wchar_t*& Tar, const std::wstring& Str) {
	NewStr(Tar, Str.c_str());
}

inline void NewStr(std::wstring& Tar, const std::wstring& Str) {
	Tar = Str;
}

// ------------------------------------------
// Trim
//	char to trim: space, \t \n \r \f \v
// ------------------------------------------

inline std::wstring_view GetTrimmedStr(wchar_t* pStart, size_t length) {
	while (length != 0 && pStart[0] == L' ') {
		pStart++;
		length--;
	}

	while (length != 0 && pStart[length - 1] == L' ') {
		length--;
	}

	return { pStart, length };
}

inline std::wstring_view GetTrimmedStr(std::wstring_view& str) {
	return GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size());
}

inline std::string_view GetTrimmedStr(char* pStart, size_t length) {
    while (length != 0 && pStart[0] == ' ') {
        pStart++;
        length--;
    }

    while (length != 0 && pStart[length - 1] == ' ') {
        length--;
    }

    return { pStart, length };
}

inline std::string_view GetTrimmedStr(std::string_view& str) {
    return GetTrimmedStr(const_cast<char*>(str.data()), str.size());
}

inline void TrimStr(std::wstring& str,
	const std::vector<wchar_t>& frontChars = { L' ' },
	const std::vector<wchar_t>& backChars = { L' ' }) {
	constexpr auto front = true;
	constexpr auto back = false;

	auto needTrim = [&] (bool dir) {
        if (str.empty()) { return false; }
		const std::vector<wchar_t>& chars = dir ? frontChars : backChars;

		for (const auto& it : chars) {
			const auto c = dir ? str.front() : str.back();

			if (c == it) {
				return true;
			}
		}

		return false;
		};

	while (needTrim(front)) {
		str.erase(str.begin());
	}
	while (needTrim(back)) {
		str.pop_back();
	}
};

// ------------------------------------------
// Case
// ------------------------------------------

namespace DeLibDetail {
    template<CharConcept T>
    inline T LowerChar(const T value) {
        if constexpr (std::is_same_v<T, wchar_t>) {
            return static_cast<T>(std::towlower(static_cast<wint_t>(value)));
        } else {
            return static_cast<T>(std::tolower(static_cast<unsigned char>(value)));
        }
    }
    template<CharConcept T>
    inline T UpperChar(const T value) {
        if constexpr (std::is_same_v<T, wchar_t>) {
            return static_cast<T>(std::towupper(static_cast<wint_t>(value)));
        } else {
            return static_cast<T>(std::toupper(static_cast<unsigned char>(value)));
        }
    }
}

template<CharConcept T>
void ToLower(T* pStr, const size_t sz) {
    for (size_t index = 0; index < sz; ++index) { pStr[index] = DeLibDetail::LowerChar(pStr[index]); }
}

template<StringConcept T>
void ToLower(T& str) { ToLower(str.data(), str.size()); }

template<StringConcept T>
[[nodiscard]] T ToLower(const T& str) {
    T ret = str;
    ToLower(ret);
    return ret;
}

template<CharConcept T>
void ToUpper(T* pStr, const size_t sz) {
    for (size_t index = 0; index < sz; ++index) { pStr[index] = DeLibDetail::UpperChar(pStr[index]); }
}

template<StringConcept T>
void ToUpper(T& str) { ToUpper(str.data(), str.size()); }

template<StringConcept T>
[[nodiscard]] T ToUpper(const T& str) {
    T ret = str;
    ToUpper(ret);
    return ret;
}

template<CharConcept T>
inline bool CharIEqu(const T l, const T r) {
    return (l == r) || (DeLibDetail::LowerChar(l) == DeLibDetail::LowerChar(r));
}

template<CharConcept T>
inline bool StrEqu(const T* l, const T* r) {
    if constexpr (std::is_same_v<T, char>) {
        return strcmp(l, r) == 0;
    }
    else {
        return wcscmp(l, r) == 0;
    }
}

template<CharConcept T>
inline bool StrIEqu(const T* l, const T* r) {
#ifdef _WIN32
    if constexpr (std::is_same_v<T, char>) {
        return _stricmp(l, r) == 0;
    }
    else {
        return _wcsicmp(l, r) == 0;
    }
#else
    while (*l != 0 && *r != 0) {
        if (!CharIEqu(*l, *r)) { return false; }
        ++l;
        ++r;
    }
    return *l == *r;
#endif
}

template<CharConcept T>
inline bool StrEmpty(const T* pStr) {
    if constexpr (std::is_same_v<T, char>) {
        return StrEqu(pStr, "");
    }
    else {
        return StrEqu(pStr, L"");
    }
}

inline bool StringViewEqu(const std::wstring_view& str, const wchar_t* pStr) {
	const auto length = wcslen(pStr);

	if (length != str.size()) {
		return false;
	}

	for (size_t i = 0; i < length; i++) {
		if (str[i] != pStr[i]) {
			return false;
		}
	}

	return true;
}

inline bool StringViewIEqu(const std::wstring_view& l, const std::wstring_view& r) {
	const auto length = r.length();

	if (length != l.size()) {
		return false;
	}

	for (size_t i = 0; i < length; i++) {
		if (!CharIEqu(l[i], r[i])) {
			return false;
		}
	}

	return true;
}

inline bool StringViewIEqu(const std::wstring_view& str, const wchar_t* pStr) {
	const auto length = wcslen(pStr);

	if (length != str.size()) {
		return false;
	}

	for (size_t i = 0; i < length; i++) {
		if (!CharIEqu(str[i], pStr[i])) {
			return false;
		}
	}

	return true;
}

// true if pR is pL's append
inline bool StringAppend(const wchar_t* pL, const wchar_t* pR) {
	const auto lLength = wcslen(pL);
	const auto rLength = wcslen(pR);

	if (lLength > rLength) { return false; }

	for (size_t i = 0; i < lLength; i++) {
		if (pL[i] != pR[i]) {
			return false;
		}
	}

	return true;
}

inline bool StringIAppend(const wchar_t* pL, const wchar_t* pR) {
	const auto lLength = wcslen(pL);
	const auto rLength = wcslen(pR);

	if (lLength > rLength) { return false; }

	for (size_t i = 0; i < lLength; i++) {
		if (!CharIEqu(pL[i], pR[i])) {
			return false;
		}
	}

	return true;
}

inline std::wstring StringViewToString(const std::wstring_view& str) {
	return { str.data(), str.size() };
}
