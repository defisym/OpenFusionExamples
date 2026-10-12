#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace encryption_portable {

// New-format parameters. Generate a fresh salt for every file and store it
// alongside the ciphertext (for example in a versioned file header).
constexpr std::size_t kSaltSize = 16;
constexpr std::size_t kKeySize = 32;
constexpr std::size_t kIvSize = 16;
constexpr std::size_t kHashSize = 32;
constexpr std::uint32_t kDefaultPbkdf2Iterations = 200000;

using Salt = std::array<std::uint8_t, kSaltSize>;
using Key = std::array<std::uint8_t, kKeySize>;
using Iv = std::array<std::uint8_t, kIvSize>;
using Sha256 = std::array<std::uint8_t, kHashSize>;

// Creates cryptographically random salt bytes using the selected OpenSSL
// provider/platform RNG.
bool make_salt(Salt& salt, std::string* error = nullptr);

// Derives AES-256 key and CBC IV from UTF-8 password bytes and per-file salt.
// The IV is derived for compatibility with the existing class's key+IV API;
// use a unique random salt for every encryption operation.
bool derive_key_and_iv(const std::string& password_utf8,
                       const Salt& salt,
                       Key& key,
                       Iv& iv,
                       std::uint32_t iterations = kDefaultPbkdf2Iterations,
                       std::string* error = nullptr);

// Decrypts AES-256-CBC with PKCS#7 padding incrementally. Memory use is
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
