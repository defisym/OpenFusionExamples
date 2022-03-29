// Author:
// https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
// Modified:
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

// Use template to support both wstring/string

#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
#include <iostream>

typedef unsigned char BYTE;
constexpr auto RETURNSIZE = 50;
constexpr auto BASE64_DECODEERROR = 0;

template<class T> 
concept STR = std::is_same_v<T, std::wstring> || std::is_same_v<T, std::string>;

template<STR base64Str>
std::wstring base64_chars_G(const std::wstring type) {
    std::wstring base64_charsW =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"0123456789+/";

    return base64_charsW;
}

template<STR base64Str>
std::string base64_chars_G(const std::string type) {
    std::string base64_charsA =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    return base64_charsA;
}

template<STR base64Str>
std::wstring base64_char_G(const std::wstring type, size_t retType) {
    switch (retType) {
    case 0:
        return L"=";
    case 1:
        return L"+";
    case 2:
        return L"/";
    default:
        return L"\0";
    }
}

template<STR base64Str>
std::string base64_char_G(const std::string type, size_t retType) {
    switch (retType) {
    case 0:
        return "=";
    case 1:
        return "+";
    case 2:
        return "/";
    default:
        return "\0";
    }
}

template<STR base64Str>
class Base64 {
private:
    using base64Char = std::remove_const_t<std::remove_reference_t<decltype(base64_chars_G<base64Str>(base64Str())[0])>>;
    
    const base64Char b64EndChar = base64_char_G<base64Str>(base64Str(), 0)[0];
    
    const base64Char plusChar = base64_char_G<base64Str>(base64Str(), 1)[0];
    const base64Char slashChar = base64_char_G<base64Str>(base64Str(), 2)[0];
    const base64Char strEndChar = base64_char_G<base64Str>(base64Str(), -1)[0];

    const base64Str base64_chars = base64_chars_G<base64Str>(base64Str());

    base64Str enRet;
    std::vector<BYTE> deRet;
    
    inline bool is_base64(base64Char c) {
        auto ret = (isalnum(c) || (c == plusChar) || (c == slashChar));

        if (!ret) {
            throw BASE64_DECODEERROR;
        }

        return ret;
    }

    inline void reserve(size_t size) {
        enRet.reserve(size);
        deRet.reserve(size);
    }

public:
    Base64() {
        reserve(RETURNSIZE);
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
                    deRet.emplace_back(char_array_3[i]);
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
                deRet.emplace_back(char_array_3[j]);
            }
        }

        return deRet;
    }

    inline void base64_decode_to_pointer(const base64Str& encoded_string, BYTE* buff, size_t size) {
        base64_decode(encoded_string);
        memcpy(buff, &deRet[0], min(size, deRet.size()));
    }
};

#endif