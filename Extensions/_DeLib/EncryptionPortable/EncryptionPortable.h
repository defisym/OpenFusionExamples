#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

namespace encryption_portable {

constexpr std::size_t kAesKeySize = 16;
constexpr std::size_t kIvSize = 16;
constexpr std::size_t kHashSize = 32;
constexpr std::size_t kKeyIvWordCount = 16;

using Key = std::array<std::uint8_t, kAesKeySize>;
using Iv = std::array<std::uint8_t, kIvSize>;
using Sha256 = std::array<std::uint8_t, kHashSize>;

// Converts wchar_t input to UTF-16 code units, then reproduces GenerateKey's
// 16-unit key/IV layout. Empty input selects the legacy default key and IV.
// The short-input expansion uses fixed 32-bit arithmetic for cross-platform
// determinism.
bool derive_key_and_iv(std::wstring_view input, Key& key, Iv& iv,
                       std::string* error = nullptr);

// Decrypts AES-128-CBC with PKCS#7 padding incrementally. Memory use is
// bounded by the internal I/O buffer. On failure, output may contain partial
// plaintext; callers writing files should use a temporary file and rename it
// only after this function succeeds.
bool decrypt_cbc_stream(std::istream& ciphertext,
                        std::ostream& plaintext,
                        const Key& key,
                        const Iv& iv,
                        std::string* error = nullptr);

// Computes SHA-256 without reading the whole input into memory.
bool sha256_stream(std::istream& input,
                   Sha256& digest,
                   std::string* error = nullptr);

}  // namespace encryption_portable
