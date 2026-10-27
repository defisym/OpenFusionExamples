#pragma once

// ------------------------------------------
// include
// ------------------------------------------

// STL
#include <string>
#include <string_view>

#include <vector>
#include <functional>
#include <algorithm>
#include <cfloat>
#ifdef _WIN32
#include <limits>
#include <cwctype>
#else
#include <filesystem>
#endif

// Custom
#include "StringTraits.h"
#include "ArithmeticTraits.h"

// ------------------------------------------
// Macro Helper
// ------------------------------------------
#if defined(MMFEXT) || defined(PROEXT)
#define Empty_Str	_T("")
#define Default_Str	_T("")

//You need "\\+" to escape +
#define RegStr_IsNum _T("(\\+|\\-)?[0-9]+(\\.[0-9]+)?")

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0)
#define AddEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_PUSHEVENT , X, 0)

#define RAD(_DEG) ((PI*(_DEG))/180)
#define DEG(_RAD) ((180*(_RAD))/PI)

#define MV rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv

#define SetExtUserData(pData) MV->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib, pData)
#define GetExtUserData() MV->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)

#define TRUNCATE_SHORT(v) ((short)(v) & 0x7FFF)
#endif

#define CONNECT_STR(l, r) l##r
#define STRINGIFY(s) #s
#ifdef _WIN32
#define CUR_FOLDER CONNECT_STR(__FILE__, "\\..\\")
#else
namespace DeLibDetail {
    class PathString {
        std::string value;
    public:
        explicit PathString(const std::filesystem::path& path) : value(path.string()) {}

        // Pointer access requires an owner whose lifetime the caller can retain.
        operator char*() & noexcept { return value.data(); }
        operator const char*() const & noexcept { return value.c_str(); }
        operator char*() && = delete;
        operator const char*() const && = delete;
        size_t size() const noexcept { return value.size(); }
    };
}
#define CUR_FOLDER DeLibDetail::PathString(std::filesystem::path(__FILE__).parent_path() / "")
#endif

// Note: if you are using files inside a shard network folder
// RELATIVE_PATH macro will return a path like \\Mac\Home\Dev
// which is not supported by the linker.
// in this case, please use symbolic link (mklink /D) to create a local folder
// which points to the network folder, and use RELATIVE_PATH with that local folder
// for example, mklink /D C:\Dev Z:\Mac\Home\Dev
// depending on your network drive settings.
// direct link to path like mklink /D C:\Dev \\Mac\Home\Dev is not supported by Windows
// and path like that cannot open in Explorer either.
#ifdef _WIN32
#define RELATIVE_PATH(path) CONNECT_STR(CUR_FOLDER, path)
#else
#define RELATIVE_PATH(relative) DeLibDetail::PathString( \
    (std::filesystem::path(__FILE__).parent_path() / (relative)).lexically_normal())
#endif

// ------------------------------------------
// Constants
// ------------------------------------------
constexpr auto CLEAR_MEMRANGE = 128;
constexpr auto CLEAR_NUMTHRESHOLD = 50;

// limit: 3.5 GB
constexpr auto MAX_MEMORYLIMIT = 3 * 1024 + 256;
constexpr auto DEFAULT_MEMORYLIMIT = 3 * 1024;

// ------------------------------------------
// Hasher
// ------------------------------------------
//   Calculate hash of given object
//     inline static size_t Hasher(const Object& o) {
//         size_t seed = ElementNum;
//         
//         seed ^= o.ele_1 + HASHER_MOVE(seed);
//         seed ^= o.ele_2 + HASHER_MOVE(seed);
//         ...
//         seed ^= o.ele_ElementNum + HASHER_MOVE(seed);
//         
//         return seed;
//     }
static constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
static constexpr auto HASHER_MOVE(size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); }

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
// CodePage conversion
// ------------------------------------------

// convert string to wstring
inline bool to_wide_string(std::wstring& output,
        const char* pSrc, size_t len,
        const unsigned int codePage);
// convert string to wstring, indicate no error occurs
inline std::wstring to_wide_string(const std::string& input,
    const unsigned int codePage);

// convert wstring to string
inline bool to_byte_string(std::string& output,
        const wchar_t* pSrc, size_t len,
        const unsigned int codePage);
// convert wstring to string, indicate no error occurs
inline std::string to_byte_string(const std::wstring& input,
    const unsigned int codePage);

// get unicode codepage
inline unsigned int GetUnicodeCodePage(bool bUnicode = true);

// general save / load, do conversion
inline bool LoadData(std::wstring& output,
	const char* pSrc, size_t len,
	bool& bUnicode) {
    if (pSrc == nullptr) { return false; }

	// BOM
	constexpr auto UTF8_SIGNATURE = "\xEF\xBB\xBF";

	if ((len >= 3) && (memcmp(pSrc, UTF8_SIGNATURE, 3) == 0)) {
		bUnicode = true;

		pSrc += 3;
		len -= 3;
	}

    if (len == 0) { return false; }

	return to_wide_string(output, pSrc, len, GetUnicodeCodePage(bUnicode));
}
// save data and handle unicode    
inline bool SaveData(std::string& output,
	const wchar_t* pSrc, const size_t len,
	bool bUnicode = true) {
	return to_byte_string(output, pSrc, len, GetUnicodeCodePage(bUnicode));
}

// ------------------------------------------
// CodePage conversion implementation
// ------------------------------------------

#ifdef _WIN32

// Windows
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

inline bool to_wide_string(std::wstring& output,
    const char* pSrc, size_t len,
    const unsigned int codePage = CP_UTF8) {
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
inline std::wstring to_wide_string(const std::string& input,
    const unsigned int codePage = CP_UTF8) {
    std::wstring result;
    to_wide_string(result, input.c_str(), input.length(), codePage);

    return result;
}

inline bool to_byte_string(std::string& output,
    const wchar_t* pSrc, size_t len,
    const unsigned int codePage = CP_UTF8) {
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
inline std::string to_byte_string(const std::wstring& input,
    const unsigned int codePage = CP_UTF8) {
    std::string result;
    to_byte_string(result, input.c_str(), input.length(), codePage);

    return result;
}

inline unsigned int GetUnicodeCodePage(bool bUnicode) {
   return bUnicode ? CP_UTF8 : CP_ACP;
}

#else
// POSIX / GNU libiconv. Android requires an explicitly supplied iconv implementation.
#include <cerrno>
#include <iconv.h>

inline constexpr unsigned int CP_UTF8 = 65001;
inline constexpr unsigned int CP_ACP = 0;

// Compatibility policy, not detection of an OS-wide Windows ANSI code page.
#ifndef DELIB_ACP_ENCODING
#define DELIB_ACP_ENCODING "UTF-8"
#endif

namespace DeLibDetail {
    inline std::string EncodingName(const unsigned int codePage) {
        switch (codePage) {
        case CP_ACP:
            return DELIB_ACP_ENCODING;
        case CP_UTF8: return "UTF-8";
        case 65000: return "UTF-7";
        case 1200: return "UTF-16LE";
        case 1201: return "UTF-16BE";
        case 12000: return "UTF-32LE";
        case 12001: return "UTF-32BE";
        case 20127: return "ASCII";
        case 54936: return "GB18030";
        default:
            if (codePage >= 28591 && codePage <= 28606) {
                return "ISO-8859-" + std::to_string(codePage - 28590);
            }
            return "CP" + std::to_string(codePage);
        }
    }

    // iconv implementations expose either char** or const char** for the input cursor.
    template<typename Function>
    inline size_t RunIconv(Function function, iconv_t converter, char** input, size_t* inputLeft,
            char** output, size_t* outputLeft) {
        if constexpr (std::is_invocable_v<Function, iconv_t, char**, size_t*, char**, size_t*>) {
            return function(converter, input, inputLeft, output, outputLeft);
        } else {
            const char* source = input == nullptr ? nullptr : *input;
            const auto result = function(converter, input == nullptr ? nullptr : &source,
                    inputLeft, output, outputLeft);
            if (input != nullptr) { *input = const_cast<char*>(source); }
            return result;
        }
    }

    inline bool ConvertBytes(std::string& output, const char* input, const size_t length,
            const std::string& sourceEncoding, const std::string& targetEncoding) {
        if (input == nullptr || length == 0 || sourceEncoding.empty() || targetEncoding.empty()) {
            return false;
        }
        const auto converter = iconv_open(targetEncoding.c_str(), sourceEncoding.c_str());
        if (converter == (iconv_t)-1) { return false; }
        struct CloseIconv {
            iconv_t converter;
            ~CloseIconv() { iconv_close(converter); }
        } closer{converter};
        std::string converted;
        char* source = const_cast<char*>(input);
        size_t inputLeft = length;
        auto step = [&](const bool flush) {
            char buffer[256];
            char* target = buffer;
            size_t outputLeft = sizeof(buffer);
            const auto result = RunIconv(&iconv, converter, flush ? nullptr : &source,
                    flush ? nullptr : &inputLeft, &target, &outputLeft);
            const auto error = errno;
            converted.append(buffer, sizeof(buffer) - outputLeft);
            if (result == static_cast<size_t>(-1)) { return error == E2BIG ? 1 : -1; }
            // Never silently accept irreversible substitutions on a non-Windows backend.
            return result == 0 ? 0 : -1;
        };
        int state;
        do { state = step(false); } while (state == 1);
        if (state < 0 || inputLeft != 0) { return false; }
        do { state = step(true); } while (state == 1);
        if (state < 0) { return false; }
        output.swap(converted);
        return true;
    }
}

inline bool to_wide_string(std::wstring& output, const char* pSrc, size_t len,
        const unsigned int codePage = CP_UTF8) {
    std::string converted;
    if (!DeLibDetail::ConvertBytes(converted, pSrc, len, DeLibDetail::EncodingName(codePage), "WCHAR_T")
            || converted.size() % sizeof(wchar_t) != 0) { return false; }
    std::wstring result(converted.size() / sizeof(wchar_t), 0);
    std::memcpy(result.data(), converted.data(), converted.size());
    output.swap(result);
    return true;
}

inline std::wstring to_wide_string(const std::string& input, const unsigned int codePage = CP_UTF8) {
    std::wstring result;
    to_wide_string(result, input.data(), input.size(), codePage);
    return result;
}

inline bool to_byte_string(std::string& output, const wchar_t* pSrc, size_t len,
        const unsigned int codePage = CP_UTF8) {
    if (len > (std::numeric_limits<size_t>::max)() / sizeof(wchar_t)) { return false; }
    return DeLibDetail::ConvertBytes(output, reinterpret_cast<const char*>(pSrc), len * sizeof(wchar_t),
            "WCHAR_T", DeLibDetail::EncodingName(codePage));
}

inline std::string to_byte_string(const std::wstring& input, const unsigned int codePage = CP_UTF8) {
    std::string result;
    to_byte_string(result, input.data(), input.size(), codePage);
    return result;
}

inline unsigned int GetUnicodeCodePage(bool bUnicode) { return bUnicode ? CP_UTF8 : CP_ACP; }

#endif

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

// ------------------------------------------
// basic split
// ------------------------------------------

// split string by delimiter
template<CharConcept T>
inline void SplitStringCore(const std::basic_string_view<T>& input,
	const T delimiter,
	const std::function<void(const std::basic_string_view<T>&)>& callBack) {
	//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	size_t start = input.find_first_not_of(delimiter);
	size_t end = start;

	while (start != std::basic_string_view<T>::npos) {
		end = input.find(delimiter, start);
		callBack({ input.data() + start,
			end == std::basic_string_view<T>::npos
			? input.length() - start
			: end - start });
		start = input.find_first_not_of(delimiter, end);
	}
}

// split string by delimiter, return but will terminate if callback returns false
template<CharConcept T>
inline void SplitStringUntilCore(const std::basic_string_view<T>& input,
    const T delimiter,
    const std::function<bool(const std::basic_string_view<T>&)>& callBack) {
    //https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
    size_t start = input.find_first_not_of(delimiter);
    size_t end = start;

    while (start != std::basic_string_view<T>::npos) {
        end = input.find(delimiter, start);
        if (!callBack({ input.data() + start,
            end == std::basic_string_view<T>::npos
            ? input.length() - start
            : end - start })) {
            break;
        }

        start = input.find_first_not_of(delimiter, end);
    }
}

template<typename RET, CharConcept T>
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

template<CharConcept T>
inline std::vector<std::basic_string<T>> SplitString(const std::basic_string<T>& input, const T delimiter) {
	return SplitString<std::basic_string<T>, T>(input, delimiter,
		[] (const std::basic_string_view<T>& item) {
			std::basic_string<T> obj{ item.data(),item.length() };

			return obj;
		});
}

// ------------------------------------------
// Range
// ------------------------------------------

template<ArithmeticConcept T>
inline T Range(T v, T minv, T maxv) {
	return (std::max)(minv, (std::min)(maxv, v));
}

template<ArithmeticConcept T>
inline void UpdateRange(T& v, T minv, T maxv) {
	v = (std::max)(minv, (std::min)(maxv, v));
}

// ------------------------------------------
// float compare
// ------------------------------------------

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

// ------------------------------------------
// Insert sort
// ------------------------------------------

template<typename T>
typename std::vector<T>::iterator
InsertSortedUpperBound(std::vector<T>& vec, T const& item) {
	return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}
template<typename T, typename Pred>
typename std::vector<T>::iterator
InsertSortedUpperBound(std::vector<T>& vec, T const& item, Pred pred) {
	return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}
template<typename T>
typename std::vector<T>::iterator
InsertSortedLowerBound(std::vector<T>& vec, T const& item) {
	return vec.insert(std::lower_bound(vec.begin(), vec.end(), item), item);
}
template<typename T, typename Pred>
typename std::vector<T>::iterator
InsertSortedLowerBound(std::vector<T>& vec, T const& item, Pred pred) {
	return vec.insert(std::lower_bound(vec.begin(), vec.end(), item, pred), item);
}

