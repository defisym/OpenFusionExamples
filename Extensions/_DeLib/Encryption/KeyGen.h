#pragma once

#include "Core.h"
#include "../UnicodeConversion.h"

#include <string_view>

namespace encryption::keygen {

// Deterministic password-to-KeyIv mapping. All UTF-16 units participate; input
// length is unrestricted. This preserves existing assets, not password hardening.
enum class SeedWidth { bits32, bits64 };

inline constexpr SeedWidth native_seed_width =
#ifdef _WIN64
    SeedWidth::bits64;
#else
    SeedWidth::bits32;
#endif

template<class Word>
inline KeyIv generate_key_iv(std::u16string_view text) {
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

inline KeyIv from_utf16(std::u16string_view text, SeedWidth width = SeedWidth::bits32) {
    return width == SeedWidth::bits32 ? generate_key_iv<std::uint32_t>(text) : generate_key_iv<std::uint64_t>(text);
}

inline KeyIv from_wide(std::wstring_view input, SeedWidth width = SeedWidth::bits32) {
    try { return from_utf16(to_utf16_string(input), width); }
    catch (const std::invalid_argument& error) { throw Error(ErrorCode::argument, error.what()); }
}

inline KeyIv from_utf8(std::string_view input, SeedWidth width = SeedWidth::bits32) {
    try { return from_utf16(to_utf16_string(input), width); }
    catch (const std::invalid_argument& error) { throw Error(ErrorCode::argument, error.what()); }
}

} // namespace encryption::keygen
