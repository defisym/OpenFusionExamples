#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Locale-independent UTF-8 / native wide strings to UTF-16 code units.
// Invalid UTF-8 or out-of-range wide input throws std::invalid_argument.
// Wide input retains existing UTF-16 units, including unpaired surrogates.

inline std::u16string to_utf16_string(std::string_view input) {
    std::u16string result;
    for (std::size_t i = 0; i < input.size();) {
        const auto first = static_cast<unsigned char>(input[i++]);
        std::uint32_t code = 0, minimum = 0;
        unsigned trailing = 0;
        if (first < 0x80) code = first;
        else if (first >= 0xc2 && first <= 0xdf) { code = first & 31; trailing = 1; minimum = 0x80; }
        else if (first >= 0xe0 && first <= 0xef) { code = first & 15; trailing = 2; minimum = 0x800; }
        else if (first >= 0xf0 && first <= 0xf4) { code = first & 7; trailing = 3; minimum = 0x10000; }
        else throw std::invalid_argument("Invalid UTF-8 input");

        if (trailing > input.size() - i) throw std::invalid_argument("Truncated UTF-8 input");

        while (trailing--) {
            const auto byte = static_cast<unsigned char>(input[i++]);
            if ((byte & 0xc0) != 0x80) throw std::invalid_argument("Invalid UTF-8 continuation");
            code = (code << 6) | (byte & 63);
        }

        if (code < minimum || code > 0x10ffff || (code >= 0xd800 && code <= 0xdfff))
            throw std::invalid_argument("Invalid UTF-8 code point");

        if (code <= 0xffff) result.push_back(static_cast<char16_t>(code));
        else {
            result.push_back(static_cast<char16_t>(0xd800 + ((code - 0x10000) >> 10)));
            result.push_back(static_cast<char16_t>(0xdc00 + ((code - 0x10000) & 0x3ff)));
        }
    }

    return result;
}

inline std::u16string to_utf16_string(std::wstring_view text) {
    std::u16string result;
    for (auto value : text) {
        const auto code = static_cast<std::uint32_t>(value);
        if (sizeof(wchar_t) == 2 || code <= 0xffff) result.push_back(static_cast<char16_t>(code));
        else {
            if (code > 0x10ffff) throw std::invalid_argument("Invalid wide character");
            result.push_back(static_cast<char16_t>(0xd800 + ((code - 0x10000) >> 10)));
            result.push_back(static_cast<char16_t>(0xdc00 + ((code - 0x10000) & 0x3ff)));
        }
    }

    return result;
}

// UTF-16LE without a BOM, independent of the host byte order.
inline std::vector<std::uint8_t> to_utf16_bytes(std::wstring_view text) {
    const auto units = to_utf16_string(text);
    std::vector<std::uint8_t> bytes;
    bytes.reserve(units.size() * 2);
    for (const auto unit : units) {
        bytes.push_back(static_cast<std::uint8_t>(unit));
        bytes.push_back(static_cast<std::uint8_t>(unit >> 8));
    }

    return bytes;
}
