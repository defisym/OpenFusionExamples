#pragma once

#include <Shlwapi.h>

#include <string>
#include <string_view>

#include <vector>

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrIEqu(X,Y) (_wcsicmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

//You need "\\+" to escape +
#define RegStr_IsNum _T("(\\+|\\-)?[0-9]+(\\.[0-9]+)?")

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);
#define AddEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_PUSHEVENT , X, 0);

#define valid(X) ((X) != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete (X); (X) = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] (X); (X) = nullptr;}

#define ResertPtr(X) X=nullptr;

#define RAD(_DEG) ((PI*(_DEG))/180)
#define DEG(_RAD) ((180*(_RAD))/PI)

#define MV rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv

#define SetExtUserData(pData) MV->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib, pData)
#define GetExtUserData() MV->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)

#define TURNCATE_SHORT(v) ((short)(v) & 0x7FFF)

constexpr auto CLEAR_MEMRANGE = 128;
constexpr auto CLEAR_NUMTHRESHOLD = 50;

// limit: 3.5 GB
constexpr auto MAX_MEMORYLIMIT = 3 * 1024 + 256;
constexpr auto DEFAULT_MEMORYLIMIT = 3 * 1024;

// hasher
static constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
static constexpr auto HASHER_MOVE(size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); }

// Usage

// inline static size_t Hasher(const Object& o) {
	// size_t seed = ElementNum;

	// seed ^= o.ele_1 + HASHER_MOVE(seed);
	// seed ^= o.ele_2 + HASHER_MOVE(seed);
	// ...
	// seed ^= o.ele_ElementNum + HASHER_MOVE(seed);

	// return seed;
// }

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(LPWSTR & Tar, const LPCWSTR Str) {
	release_arr(Tar);
	const rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPWSTR & Tar, const std::wstring & Str) {
	NewStr(Tar, Str.c_str());
}

inline void NewStr(std::wstring& Tar, const std::wstring& Str) {
	Tar = Str;
}

// convert string to wstring
inline std::wstring to_wide_string(const std::string& input, const UINT codePage = CP_UTF8) {
	const auto size_needed = MultiByteToWideChar(codePage, 0, &input.at(0), (int)input.size(), nullptr, 0);

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(codePage, 0, &input.at(0), (int)input.size(), &result.at(0), size_needed);

	return result;
}

// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input, const UINT codePage = CP_UTF8) {
	const auto size_needed = WideCharToMultiByte(codePage, 0, &input.at(0), (int)input.size(), nullptr, 0, nullptr, nullptr);

	std::string result(size_needed, 0);
	WideCharToMultiByte(codePage, 0, &input.at(0), (int)input.size(), &result.at(0), size_needed, nullptr, nullptr);

	return result;
}

inline std::wstring_view GetTrimmedStr(LPWSTR pStart, size_t length) {
	while (pStart[0] == L' ') {
		pStart++;
		length--;
	}

	while ((pStart + length - 1)[0] == L' ') {
		length--;
	}

	return { pStart, length };
}

inline std::wstring_view GetTrimmedStr(std::wstring_view& str) {
	return GetTrimmedStr(const_cast<wchar_t*>(str.data()), str.size());
}

inline void TrimStr(std::wstring& str,
	const std::vector<wchar_t>& frontChars = { L' ' },
	const std::vector<wchar_t>& backChars = { L' ' }) {
	constexpr auto front = true;
	constexpr auto back = false;

	auto needTrim = [&] (bool dir) {
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

inline bool StringViewEqu(const std::wstring_view& str, const LPCWSTR pStr) {
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

inline bool StringViewIEqu(const std::wstring_view& str, const LPCWSTR pStr) {
	const auto length = wcslen(pStr);

	if (length != str.size()) {
		return false;
	}

	for (size_t i = 0; i < length; i++) {
		if (ChrCmpIW(str[i], pStr[i]) != 0) {
			return false;
		}
	}

	return true;
}

// MSGBOX
#include "StrNum.h"

inline void MSGBOX(const std::wstring& content, const std::wstring& title = L"ALERT") {
	MessageBox(nullptr, content.c_str(), title.c_str(), MB_OK);
}

// basic split
#include <functional>

inline void SplitStringCore(const std::wstring& input, const wchar_t delimiter
	, const std::function<void(const std::wstring&)>& callBack) {
	//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	size_t start = input.find_first_not_of(delimiter);
	size_t end = start;

	while (start != std::wstring::npos) {
		end = input.find(delimiter, start);
		callBack(input.substr(start, end - start));
		start = input.find_first_not_of(delimiter, end);
	}
}

template<typename T>
inline std::vector<T> SplitString(const std::wstring& input, const wchar_t delimiter
	, std::function<T(const std::wstring&)> callBack) {
	std::vector<T> resultList;

	SplitStringCore(input, delimiter, [&](const std::wstring& item) {
		resultList.emplace_back(callBack(item));
		});

	return resultList;
}

inline std::vector<std::wstring> SplitString(const std::wstring& input, const wchar_t delimiter) {
	return SplitString<std::wstring>(input, delimiter, [](const std::wstring& item) {return item; });
}

template<typename T>
inline T Range(T v, T minv, T maxv) {
	return (std::max)(minv, (std::min)(maxv, v));
}

template<typename T>
inline void UpdateRange(T& v, T minv, T maxv) {
	v = (std::max)(minv, (std::min)(maxv, v));
}
