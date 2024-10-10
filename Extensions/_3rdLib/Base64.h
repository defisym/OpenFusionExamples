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
// BYTE* buffer = new BYTE[BIGENOUGH];
// base64_decode(encoded_string, buffer, bufsz);
// 3.
// base64_decode(encoded_string);
// size_t bufsz = base64_decode_size();
// BYTE* buffer = new BYTE[bufsz];
// base64_decode(buffer, bufsz);

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <functional>

#include "StringTraits.h"

typedef unsigned char BYTE;
constexpr auto RETURN_SIZE = 50;
constexpr auto BASE64_DECODE_ERROR = 0;

template<StringConcept base64Str>
std::wstring base64_chars_G(const std::wstring type) {
    std::wstring base64_charsW =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"0123456789+/";

    return base64_charsW;
}

template<StringConcept base64Str>
std::string base64_chars_G(const std::string type) {
    std::string base64_charsA =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    return base64_charsA;
}

template<StringConcept base64Str>
wchar_t base64_char_G(const std::wstring type, size_t retType) {
    switch (retType) {
    case 0:
        return L'=';
    case 1:
        return L'+';
    case 2:
        return L'/';
    default:
        return L'\0';
    }
}

template<StringConcept base64Str>
char base64_char_G(const std::string type, size_t retType) {
    switch (retType) {
    case 0:
        return '=';
    case 1:
        return '+';
    case 2:
        return '/';
    default:
        return '\0';
    }
}

template<StringConcept base64Str>
class Base64 {
private:
    const base64Str type;
    using base64Char = std::remove_const_t<std::remove_reference_t<decltype(type[0])>>;
    
    const base64Char b64EndChar = base64_char_G<base64Str>(type, 0);
    
    const base64Char plusChar = base64_char_G<base64Str>(type, 1);
    const base64Char slashChar = base64_char_G<base64Str>(type, 2);
    const base64Char strEndChar = base64_char_G<base64Str>(type, -1);

    const base64Str base64_chars = base64_chars_G<base64Str>(type);

    base64Str enRet;
    std::vector<BYTE> deRet;
    
    inline bool is_base64(base64Char c) {
        auto ret = (isalnum(c) || (c == plusChar) || (c == slashChar));

        if (!ret) {
            throw BASE64_DECODE_ERROR;
        }

        return ret;
    }

    inline void reserve(size_t size) {
        enRet.reserve(size);
        deRet.reserve(size);
    }

public:
    Base64() {
        reserve(RETURN_SIZE);
    }
    ~Base64() {

    }

    inline base64Str base64_encode(BYTE const* buf, unsigned int bufLen) {
        int i = 0;
        int j = 0;

        BYTE char_array_4[4];
        BYTE char_array_3[3];        

        enRet.clear();

        while (bufLen--) {
            char_array_3[i++] = *(buf++);

            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++) {
                    enRet += base64_chars[char_array_4[i]];
                }

                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = (BYTE)strEndChar;
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                enRet += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                enRet += b64EndChar;
        }

        return enRet;
    }
    inline std::vector<BYTE> base64_decode(const base64Str& encoded_string) {
        int i = 0;
        int j = 0;

        base64Char char_array_4[4];
        base64Char char_array_3[3];

        deRet.clear();

        int in_len = encoded_string.size();
        int in_ = 0;

        while (in_len-- && (encoded_string[in_] != b64EndChar) && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_];
            in_++;

            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = (base64Char)base64_chars.find(char_array_4[i]);
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++) {
                    deRet.emplace_back((BYTE)char_array_3[i]);
                }

                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++) {
                char_array_4[j] = 0;
            }

            for (j = 0; j < 4; j++) {
                char_array_4[j] = (base64Char)base64_chars.find(char_array_4[j]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) {
                deRet.emplace_back((BYTE)char_array_3[j]);
            }
        }

        return deRet;
    }

    [[deprecated]] inline size_t base64_decode_size() {
        return deRet.size();
    }
    [[deprecated]] inline void base64_decode_to_pointer(BYTE* buff, size_t size) {
        memcpy(buff, &deRet[0], (std::min)(size, deRet.size()));
    }

    inline bool base64_decode_to_pointer(const base64Str& encoded_string, BYTE* buff, size_t size) {
        return base64_decode_callback(encoded_string, [&] (const BYTE* base64_buff, const size_t base64_size) {
            memcpy(buff, base64_buff, (std::min)(size, base64_size));
        });
    }

    // decode internally, use callback to process raw data
    // don't need to make a temp buffer
    inline bool base64_decode_callback(const base64Str& encoded_string,
        const std::function<void(const BYTE* buff, const size_t size)>& callback) {
        try {
            this->base64_decode(encoded_string);
        } catch (decltype(BASE64_DECODE_ERROR)) {
            return false;
        }

        callback(deRet.data(), deRet.size());

        return true;
    }
};