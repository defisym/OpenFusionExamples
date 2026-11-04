#pragma once

#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

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
