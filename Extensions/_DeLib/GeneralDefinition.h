#pragma once

#include <Shlwapi.h>

#include <string>
#include <string_view>

#include <vector>

#define Empty_Str	_T("")
#define Default_Str	_T("")

//You need "\\+" to escape +
#define RegStr_IsNum _T("(\\+|\\-)?[0-9]+(\\.[0-9]+)?")

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0)
#define AddEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_PUSHEVENT , X, 0)

#define valid(X) ((X) != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete (X); (X) = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] (X); (X) = nullptr;}

#define ResetPtr(X) (X) = nullptr;

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
inline void NewStr(LPWSTR& Tar, const LPCWSTR Str) {
	release_arr(Tar);
	const rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPWSTR& Tar, const std::wstring& Str) {
	NewStr(Tar, Str.c_str());
}

inline void NewStr(std::wstring& Tar, const std::wstring& Str) {
	Tar = Str;
}

// convert string to wstring
inline bool to_wide_string(std::wstring& output,
		const char* pSrc, size_t len,
		const UINT codePage = CP_UTF8) {
	const int outputLength = MultiByteToWideChar(codePage, 0,
		pSrc, static_cast<int>(len),
		nullptr, 0);

	if (outputLength == 0) {
		return false;
	}

	output = std::wstring(outputLength, 0);
	if (!MultiByteToWideChar(codePage, 0,
		pSrc, static_cast<int>(len),
		output.data(), outputLength)) {
		return false;
	}

	return true;
}
// convert string to wstring, indicate no error occurs
inline std::wstring to_wide_string(const std::string& input, const UINT codePage = CP_UTF8) {
	std::wstring result;
	to_wide_string(result, input.c_str(), input.length(), codePage);

	return result;
}

// convert wstring to string
inline bool to_byte_string(std::string& output,
		const wchar_t* pSrc, size_t len,
		const UINT codePage = CP_UTF8) {
	const int outputLength = WideCharToMultiByte(codePage, 0,
			pSrc, static_cast<int>(len),
			nullptr, 0,
			nullptr, nullptr);

	if (outputLength == 0) {
		return false;
	}

	output = std::string(outputLength, 0);
	if (!WideCharToMultiByte(codePage, 0,
		pSrc, static_cast<int>(len),
		output.data(), outputLength,
		nullptr, nullptr)) {
		return false;
	}

	return true;
}
// convert wstring to string, indicate no error occurs
inline std::string to_byte_string(const std::wstring& input, const UINT codePage = CP_UTF8) {
	std::string result;
	to_byte_string(result, input.c_str(), input.length(), codePage);

	return result;
}

// general save / load, do conversion
inline bool LoadData(std::wstring& output,
	const char* pSrc, size_t len,
	bool& bUnicode) {
	if (pSrc == nullptr) {
		return false;
	}

	// BOM
	constexpr auto UTF8_SIGNATURE = "\xEF\xBB\xBF";

	if ((len >= 3) && (memcmp(pSrc, UTF8_SIGNATURE, 3) == 0)) {
		bUnicode = true;

		pSrc += 3;
		len -= 3;
	}

	if (len == 0) {
		return false;
	}

	const UINT codePage = bUnicode ? CP_UTF8 : CP_ACP;
	return to_wide_string(output, pSrc, len, codePage);
}
// save data and handle unicode    
inline bool SaveData(std::string& output,
	const wchar_t* pSrc, const size_t len,
	bool bUnicode = true) {
	const UINT codePage = bUnicode ? CP_UTF8 : CP_ACP;
	return to_byte_string(output, pSrc, len, codePage);
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

inline bool StringViewIEqu(const std::wstring_view& l, const std::wstring_view& r) {
	const auto length = r.length();

	if (length != l.size()) {
		return false;
	}

	for (size_t i = 0; i < length; i++) {
		if (ChrCmpIW(l[i], r[i]) != 0) {
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

// true if pR is pL's append
inline bool StringAppend(const LPCWSTR pL, const LPCWSTR pR) {
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

inline bool StringIAppend(const LPCWSTR pL, const LPCWSTR pR) {
	const auto lLength = wcslen(pL);
	const auto rLength = wcslen(pR);

	if (lLength > rLength) { return false; }

	for (size_t i = 0; i < lLength; i++) {
		if (ChrCmpIW(pL[i], pR[i]) != 0) {
			return false;
		}
	}

	return true;
}

inline std::wstring StringViewToString(const std::wstring_view& str) {
	return { str.data(), str.size() };
}

// MSGBOX
#include "StrNum.h"

inline void MSGBOX(const std::wstring& content, const std::wstring& title = L"ALERT") {
	MessageBox(nullptr, content.c_str(), title.c_str(), MB_OK);
}

// basic split
#include <functional>

template<typename T>
concept CHR = std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, char>
|| std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, wchar_t>;

template<CHR T>
inline bool StrEqu(const T* l, const T* r) {
	 if constexpr(std::is_same_v<T, char>) {
		 return strcmp(l, r) == 0;
	 }else {
		 return wcscmp(l, r) == 0;
	 }
}

template<CHR T>
inline bool StrIEqu(const T* l, const T* r) {
	if constexpr (std::is_same_v<T, char>) {
		return _stricmp(l, r) == 0;
	}
	else {
		return _wcsicmp(l, r) == 0;
	}
}

template<CHR T>
inline bool StrEmpty(const T* pStr) {
	if constexpr (std::is_same_v<T, char>) {
		return StrEqu(pStr, "");
	}
	else {
		return StrEqu(pStr, L"");
	}
}

template<CHR T>
inline void SplitStringCore(const std::basic_string<T>& input,
	const T delimiter,
	const std::function<void(const std::basic_string_view<T>&)>& callBack) {
	//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	size_t start = input.find_first_not_of(delimiter);
	size_t end = start;

	while (start != std::basic_string<T>::npos) {
		end = input.find(delimiter, start);
		callBack({ input.data() + start,
			end == std::basic_string<T>::npos
			? input.length() - start
			: end - start });
		start = input.find_first_not_of(delimiter, end);
	}
}

template<typename RET, CHR T>
inline std::vector<RET> SplitString(const std::basic_string<T>& input,
	const T delimiter,
	std::function<RET(const std::basic_string_view<T>&)> callBack) {
	std::vector<RET> resultList;

	SplitStringCore<T>(input, delimiter,
		[&] (const std::basic_string_view<T>& item) {
			resultList.emplace_back(std::move(callBack(item)));
		});

	return resultList;
}

template<CHR T>
inline std::vector<std::basic_string<T>> SplitString(const std::basic_string<T>& input, const T delimiter) {
	return SplitString<std::basic_string<T>, T>(input, delimiter,
		[] (const std::basic_string_view<T>& item) {
			std::basic_string<T> obj{ item.data(),item.length() };

			return obj;
		});
}

template<typename T>
inline T Range(T v, T minv, T maxv) {
	return (std::max)(minv, (std::min)(maxv, v));
}

template<typename T>
inline void UpdateRange(T& v, T minv, T maxv) {
	v = (std::max)(minv, (std::min)(maxv, v));
}

// https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
template<typename T>
inline bool NearlyEqualCore(const T a, const T b,
						// those defaults are arbitrary and could be removed
						const T epsilon,
						const T abs_th) {
	//assert(std::numeric_limits<float>::epsilon() <= epsilon);
	//assert(epsilon < 1.f);

	if (a == b) return true;  // NOLINT(clang-diagnostic-float-equal)

	const auto diff = std::abs(a - b);
	// or even faster:
	//const auto norm = (std::min)(std::abs(a + b),
	const auto norm = (std::min)(std::abs(a) + std::abs(b),
		(std::numeric_limits<T>::max)());
	// keeping this commented out until I update figures below
	return diff < (std::max)(abs_th, epsilon * norm);
}

inline bool NearlyEqualFLT(const float a, const float b,
						// those defaults are arbitrary and could be removed
						const float epsilon = 128 * FLT_EPSILON,
						const float abs_th = FLT_MIN) {
	return NearlyEqualCore<float>(a, b, epsilon, abs_th);
}

inline bool NearlyEqualDBL(const double a, const double b,
						// those defaults are arbitrary and could be removed
						const double epsilon = 128 * DBL_EPSILON,
						const double abs_th = DBL_MIN) {
	return NearlyEqualCore<double>(a, b, epsilon, abs_th);
}

inline bool NearlyEqualLDBL(const long double a, const long double b,
						// those defaults are arbitrary and could be removed
						const long double epsilon = 128 * LDBL_EPSILON,
						const long double abs_th = LDBL_MIN) {
	return NearlyEqualCore<long double>(a, b, epsilon, abs_th);
}