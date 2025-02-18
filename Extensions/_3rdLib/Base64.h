// Author:
// https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
// Modified:
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

// Use template to support both wstring/string

// Usage
// 1.
// auto data = base64_decode(encoded_string);
// 2.
// size_t bufsz = BIGENOUGH;
// unsigned char* buffer = new unsigned char[BIGENOUGH];
// base64_decode(encoded_string, buffer, bufsz);
// 3.
// base64_decode(encoded_string);
// size_t bufsz = base64_decode_size();
// unsigned char* buffer = new unsigned char[bufsz];
// base64_decode(buffer, bufsz);

#pragma once

#include <vector>
#include <string>
#include <functional>

#include "StringTraits.h"

namespace Base64Constants {
    constexpr auto B64EndChar = 0;
    constexpr auto PlusChar = 1;
    constexpr auto SlashChar = 2;
    constexpr auto StrEndChar = -1;
}

template<StringConcept StringType = std::wstring>
struct Base64Helper {};

template<>
struct Base64Helper<std::string> {
    static std::string base64_chars() {
        static const std::string base64_charsA =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        return base64_charsA;
    }

    static constexpr char get_base64_char(const size_t retType) {
        switch (retType) {
        case Base64Constants::B64EndChar:
            return '=';
        case Base64Constants::PlusChar:
            return '+';
        case Base64Constants::SlashChar:
            return '/';
        default:
            return '\0';
        }
    }
};

template<>
struct Base64Helper<std::wstring> {
    static std::wstring base64_chars() {
        static const std::wstring base64_charsW =
            L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            L"abcdefghijklmnopqrstuvwxyz"
            L"0123456789+/";

        return base64_charsW;
    }

    static constexpr wchar_t get_base64_char(const size_t retType) {
        switch (retType) {
        case Base64Constants::B64EndChar:
            return L'=';
        case Base64Constants::PlusChar:
            return L'+';
        case Base64Constants::SlashChar:
            return L'/';
        default:
            return L'\0';
        }
    }
};

template<StringConcept Base64Str>
class Base64 {
    using Helper = Base64Helper<Base64Str>;
    Base64Str base64_chars = Helper::base64_chars();
    
    using Base64Char = typename CharType<Base64Str>::Type;
    Base64Char b64EndChar = Helper::get_base64_char(Base64Constants::B64EndChar);
    Base64Char plusChar = Helper::get_base64_char(Base64Constants::PlusChar);
    Base64Char slashChar = Helper::get_base64_char(Base64Constants::SlashChar);
    Base64Char strEndChar = Helper::get_base64_char(Base64Constants::StrEndChar);

    Base64Str enRet;
    std::vector<unsigned char> deRet;
    
    inline bool is_base64(Base64Char c) {
        auto ret = (isalnum(c) || (c == plusChar) || (c == slashChar));

        if (!ret) {
            throw std::exception("BASE64_DECODE_ERROR");
        }

        return ret;
    }

    inline void reserve(size_t size) {
        enRet.reserve(size);
        deRet.reserve(size);
    }

public:
    Base64() {
        static constexpr auto RETURN_SIZE = 50;
        reserve(RETURN_SIZE);
    }

    inline Base64Str base64_encode(unsigned char const* buf, unsigned int bufLen) {
        int i = 0;

        unsigned char char_array_4[4];
        unsigned char char_array_3[3];        

        enRet.clear();

        while (bufLen--) {
            char_array_3[i++] = *(buf++);

            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) {
                    enRet += base64_chars[char_array_4[i]];
                }

                i = 0;
            }
        }

        if (i) {
            for (int j = i; j < 3; j++) {
                char_array_3[j] = static_cast<unsigned char>(strEndChar);
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = static_cast<unsigned char>(((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
            char_array_4[2] = static_cast<unsigned char>(((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (int j = 0; j < i + 1; j++) {
                enRet += base64_chars[char_array_4[j]];
            }

            while (i++ < 3) {
                enRet += b64EndChar;
            }
        }

        return enRet;
    }
    inline std::vector<unsigned char> base64_decode(const Base64Str& encoded_string) {
        int i = 0;

        Base64Char char_array_4[4];
        Base64Char char_array_3[3];

        deRet.clear();

        int in_len = encoded_string.size();
        int in_ = 0;

        while (in_len-- && (encoded_string[in_] != b64EndChar) && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_];
            in_++;

            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = static_cast<Base64Char>(base64_chars.find(char_array_4[i]));
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++) {
                    deRet.emplace_back(static_cast<unsigned char>(char_array_3[i]));
                }

                i = 0;
            }
        }

        if (i) {
            for (int j = i; j < 4; j++) {
                char_array_4[j] = 0;
            }

            for (int j = 0; j < 4; j++) {
                char_array_4[j] = static_cast<Base64Char>(base64_chars.find(char_array_4[j]));
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (int j = 0; j < i - 1; j++) {
                deRet.emplace_back(static_cast<unsigned char>(char_array_3[j]));
            }
        }

        return deRet;
    }

    [[deprecated]] inline size_t base64_decode_size() const {
        return deRet.size();
    }
    [[deprecated]] inline void base64_decode_to_pointer(unsigned char* buff, const size_t size) const {
        memcpy(buff, deRet.data(), (std::min)(size, deRet.size()));
    }

    inline bool base64_decode_to_pointer(const Base64Str& encoded_string, unsigned char* buff, size_t size) {
        return base64_decode_callback(encoded_string, [&] (const unsigned char* base64_buff, const size_t base64_size) {
            memcpy(buff, base64_buff, (std::min)(size, base64_size));
        });
    }

    // decode internally, use callback to process raw data
    // don't need to make a temp buffer
    inline bool base64_decode_callback(const Base64Str& encoded_string,
        const std::function<void(const unsigned char* buff, const size_t size)>& callback) {
        try {
            this->base64_decode(encoded_string);
        } catch ([[maybe_unused]] std::exception& e) {
            return false;
        }

        callback(deRet.data(), deRet.size());

        return true;
    }
};