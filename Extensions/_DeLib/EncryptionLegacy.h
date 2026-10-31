#pragma once
#include "EncryptionCore.h"
#include <string_view>

namespace encryption::legacy {
// Compatibility with Windows asset files, never a modern password KDF.
enum class WordSize { win32, win64 };
inline constexpr WordSize native_word_size =
#ifdef _WIN64
    WordSize::win64;
#else
    WordSize::win32;
#endif
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
template<class Word>
inline KeyIv derive_impl(std::u16string_view text) {
    std::array<std::uint16_t, 16> words{};
    if (text.empty()) {
        KeyIv result{};
        for (std::size_t i = 0; i < 16; ++i) result.key[i] = result.iv[i] = static_cast<Byte>(i);
        return result;
    }
    const auto count = (std::min)(text.size(), words.size());
    std::copy_n(text.begin(), count, words.begin());
    if (text.size() < 16) {
        Word seed = static_cast<Word>(text.size());
        const auto move = [](Word value) -> Word {
            return Word(Word{0x9e3779b9u} + (value << 6) + (value >> 2));
        };
        for (std::size_t i = text.size(); i != 0; --i)
            seed ^= Word(Word(text[i - 1]) + move(seed));
        for (std::size_t i = text.size(); i < 16; ++i) {
            words[i] = static_cast<std::uint16_t>(((seed >> 8) & 255) ^ (seed & 255));
            seed ^= Word(Word(words[i]) + move(seed));
        }
    } else {
        for (std::size_t i = 16; i < text.size(); ++i)
            for (auto& word : words) word ^= static_cast<std::uint16_t>(text[i]);
    }
    KeyIv result{};
    for (std::size_t i = 0; i < 8; ++i) {
        result.key[i * 2] = static_cast<Byte>(words[i]);
        result.key[i * 2 + 1] = static_cast<Byte>(words[i] >> 8);
        result.iv[i * 2] = static_cast<Byte>(words[i + 8]);
        result.iv[i * 2 + 1] = static_cast<Byte>(words[i + 8] >> 8);
    }
    return result;
}
inline KeyIv derive(std::u16string_view text, WordSize size = WordSize::win32) {
    return size == WordSize::win32 ? derive_impl<std::uint32_t>(text) : derive_impl<std::uint64_t>(text);
}
} // namespace encryption::legacy
