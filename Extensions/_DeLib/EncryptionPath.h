#pragma once

#include "EncryptionCore.h"

#include <string_view>

namespace encryption {

// Wide paths for encrypted-file I/O; not part of the general string utilities.
inline std::filesystem::path file_path(const wchar_t* text) {
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

} // namespace encryption
