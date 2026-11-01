#pragma once

#include "EncryptionCore.h"

#include <string_view>

namespace encryption::text {

inline std::u16string utf16_from_utf8(std::string_view input) {
    std::u16string result;
    for (std::size_t i = 0; i < input.size();) {
        const auto first = static_cast<unsigned char>(input[i++]);
        std::uint32_t code = 0, minimum = 0;
        unsigned trailing = 0;
        if (first < 0x80) code = first;
        else if (first >= 0xc2 && first <= 0xdf) { code = first & 31; trailing = 1; minimum = 0x80; }
        else if (first >= 0xe0 && first <= 0xef) { code = first & 15; trailing = 2; minimum = 0x800; }
        else if (first >= 0xf0 && first <= 0xf4) { code = first & 7; trailing = 3; minimum = 0x10000; }
        else throw Error(ErrorCode::argument, "Invalid UTF-8 password");

        require(trailing <= input.size() - i, ErrorCode::argument, "Truncated UTF-8 password");

        while (trailing--) {
            const auto byte = static_cast<unsigned char>(input[i++]);
            require((byte & 0xc0) == 0x80, ErrorCode::argument, "Invalid UTF-8 continuation");
            code = (code << 6) | (byte & 63);
        }

        require(code >= minimum && code <= 0x10ffff && !(code >= 0xd800 && code <= 0xdfff),
                ErrorCode::argument, "Invalid UTF-8 code point");

        if (code <= 0xffff) result.push_back(static_cast<char16_t>(code));
        else {
            result.push_back(static_cast<char16_t>(0xd800 + ((code - 0x10000) >> 10)));
            result.push_back(static_cast<char16_t>(0xdc00 + ((code - 0x10000) & 0x3ff)));
        }
    }

    return result;
}

inline std::u16string utf16(std::wstring_view text) {
    std::u16string result;
    for (auto value : text) {
        const auto code = static_cast<std::uint32_t>(value);
        if (sizeof(wchar_t) == 2 || code <= 0xffff) result.push_back(static_cast<char16_t>(code));
        else {
            require(code <= 0x10ffff, ErrorCode::argument, "Invalid wide character");
            result.push_back(static_cast<char16_t>(0xd800 + ((code - 0x10000) >> 10)));
            result.push_back(static_cast<char16_t>(0xdc00 + ((code - 0x10000) & 0x3ff)));
        }
    }

    return result;
}

inline Bytes utf16_bytes(std::wstring_view text) {
    const auto units = utf16(text);
    Bytes bytes;
    bytes.reserve(units.size() * 2);
    for (const auto unit : units) {
        bytes.push_back(static_cast<Byte>(unit));
        bytes.push_back(static_cast<Byte>(unit >> 8));
    }

    return bytes;
}

inline std::filesystem::path path(const wchar_t* text) {
    require(text != nullptr, ErrorCode::argument, "Null file path");
#ifdef _WIN32
    return std::filesystem::path(text);
#else
    std::string utf8;
    for (const auto value : std::wstring_view(text)) {
        const auto code = static_cast<std::uint32_t>(value);
        require(code <= 0x10ffff && !(code >= 0xd800 && code <= 0xdfff),
                ErrorCode::argument, "Invalid file path character");
        if (code < 0x80) utf8.push_back(static_cast<char>(code));
        else if (code < 0x800) {
            utf8.push_back(static_cast<char>(0xc0 | (code >> 6)));
            utf8.push_back(static_cast<char>(0x80 | (code & 63)));
        } else if (code < 0x10000) {
            utf8.push_back(static_cast<char>(0xe0 | (code >> 12)));
            utf8.push_back(static_cast<char>(0x80 | ((code >> 6) & 63)));
            utf8.push_back(static_cast<char>(0x80 | (code & 63)));
        } else {
            utf8.push_back(static_cast<char>(0xf0 | (code >> 18)));
            utf8.push_back(static_cast<char>(0x80 | ((code >> 12) & 63)));
            utf8.push_back(static_cast<char>(0x80 | ((code >> 6) & 63)));
            utf8.push_back(static_cast<char>(0x80 | (code & 63)));
        }
    }
    return std::filesystem::u8path(utf8);
#endif
}

} // namespace encryption::text
