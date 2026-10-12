#include "EncryptionPortable.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

#include <openssl/evp.h>

namespace encryption_portable {
namespace {

void set_error(std::string* error, const char* message) {
    if (error != nullptr) {
        *error = message;
    }
}

constexpr std::size_t kIoBufferSize = 64 * 1024;
constexpr std::uint32_t kHasherMagicNumber = 0x9e3779b9u;

std::uint32_t hasher_move(std::uint32_t seed) {
    return kHasherMagicNumber + (seed << 6) + (seed >> 2);
}

bool append_utf16(std::uint32_t value, std::vector<std::uint16_t>& output) {
    if (value > 0x10ffffu) {
        return false;
    }
    if (value <= 0xffffu) {
        // Preserve every 16-bit value, including surrogate-range values. The
        // original API consumes wchar_t units as key material, not text.
        output.push_back(static_cast<std::uint16_t>(value));
        return true;
    }

    value -= 0x10000u;
    output.push_back(static_cast<std::uint16_t>(0xd800u + (value >> 10)));
    output.push_back(static_cast<std::uint16_t>(0xdc00u + (value & 0x3ffu)));
    return true;
}

bool to_utf16(std::wstring_view input, std::vector<std::uint16_t>& output) {
    output.clear();
    output.reserve(input.size());

    if constexpr (sizeof(wchar_t) == 2) {
        for (const auto value : input) {
            output.push_back(static_cast<std::uint16_t>(value));
        }
        return true;
    } else if constexpr (sizeof(wchar_t) == 4) {
        for (const auto value : input) {
            if (!append_utf16(static_cast<std::uint32_t>(value), output)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void store_word_le(std::uint16_t word, std::uint8_t* output) {
    output[0] = static_cast<std::uint8_t>(word & 0xffu);
    output[1] = static_cast<std::uint8_t>(word >> 8);
}

}  // namespace

bool derive_key_and_iv(std::wstring_view input, Key& key, Iv& iv,
                       std::string* error) {
    std::vector<std::uint16_t> units;
    if (!to_utf16(input, units)) {
        set_error(error, "Input contains a wchar_t value outside the supported range");
        return false;
    }

    std::array<std::uint16_t, kKeyIvWordCount> material{};
    if (units.empty()) {
        // Match the legacy DefaultKey and DefaultIV byte arrays.
        for (std::size_t i = 0; i < 8; ++i) {
            const auto first = static_cast<std::uint8_t>(i * 2);
            const auto second = static_cast<std::uint8_t>(i * 2 + 1);
            material[i] = static_cast<std::uint16_t>(first | (second << 8));
            material[i + 8] = material[i];
        }
    } else if (units.size() < material.size()) {
        std::copy(units.begin(), units.end(), material.begin());
        auto seed = static_cast<std::uint32_t>(units.size());
        for (std::size_t i = units.size(); i > 0; --i) {
            seed ^= static_cast<std::uint32_t>(units[i - 1]) + hasher_move(seed);
        }
        for (std::size_t i = units.size(); i < material.size(); ++i) {
            const auto high = static_cast<std::uint16_t>((seed >> 8) & 0xffu);
            const auto low = static_cast<std::uint16_t>(seed & 0xffu);
            material[i] = static_cast<std::uint16_t>(high ^ low);
            seed ^= static_cast<std::uint32_t>(material[i]) + hasher_move(seed);
        }
    } else {
        std::copy_n(units.begin(), material.size(), material.begin());
        for (std::size_t i = material.size(); i < units.size(); ++i) {
            const auto value = units[i];
            for (std::size_t word = 0; word < 8; ++word) {
                material[word] ^= value;
                material[word + 8] ^= value;
            }
        }
    }

    for (std::size_t i = 0; i < 8; ++i) {
        store_word_le(material[i], key.data() + i * 2);
        store_word_le(material[i + 8], iv.data() + i * 2);
    }
    return true;
}

bool decrypt_cbc_stream(std::istream& ciphertext,
                        std::ostream& plaintext,
                        const Key& key,
                        const Iv& iv,
                        std::string* error) {
    EVP_CIPHER_CTX* raw_context = EVP_CIPHER_CTX_new();
    if (raw_context == nullptr) {
        set_error(error, "Could not allocate OpenSSL cipher context");
        return false;
    }

    const auto free_context = [](EVP_CIPHER_CTX* context) {
        EVP_CIPHER_CTX_free(context);
    };
    const std::unique_ptr<EVP_CIPHER_CTX, decltype(free_context)> context(
        raw_context, free_context);

    if (EVP_DecryptInit_ex(context.get(), EVP_aes_128_cbc(), nullptr,
                           key.data(), iv.data()) != 1) {
        set_error(error, "Could not initialize AES-128-CBC decryption");
        return false;
    }

    std::array<char, kIoBufferSize> input{};
    std::vector<unsigned char> output(kIoBufferSize + EVP_MAX_BLOCK_LENGTH);

    while (ciphertext) {
        ciphertext.read(input.data(), static_cast<std::streamsize>(input.size()));
        const auto count = ciphertext.gcount();
        if (count <= 0) {
            break;
        }

        int produced = 0;
        if (EVP_DecryptUpdate(context.get(), output.data(), &produced,
                              reinterpret_cast<const unsigned char*>(input.data()),
                              static_cast<int>(count)) != 1) {
            set_error(error, "AES-CBC decryption failed");
            return false;
        }
        if (produced > 0) {
            plaintext.write(reinterpret_cast<const char*>(output.data()), produced);
            if (!plaintext) {
                set_error(error, "Writing plaintext failed");
                return false;
            }
        }
    }

    if (ciphertext.bad()) {
        set_error(error, "Reading ciphertext failed");
        return false;
    }

    int final_size = 0;
    if (EVP_DecryptFinal_ex(context.get(), output.data(), &final_size) != 1) {
        set_error(error, "Invalid ciphertext or PKCS#7 padding");
        return false;
    }
    if (final_size > 0) {
        plaintext.write(reinterpret_cast<const char*>(output.data()), final_size);
        if (!plaintext) {
            set_error(error, "Writing final plaintext block failed");
            return false;
        }
    }
    return true;
}

bool sha256_stream(std::istream& input,
                   Sha256& digest,
                   std::string* error) {
    EVP_MD_CTX* raw_context = EVP_MD_CTX_new();
    if (raw_context == nullptr) {
        set_error(error, "Could not allocate OpenSSL digest context");
        return false;
    }

    const auto free_context = [](EVP_MD_CTX* context) {
        EVP_MD_CTX_free(context);
    };
    const std::unique_ptr<EVP_MD_CTX, decltype(free_context)> context(
        raw_context, free_context);

    if (EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr) != 1) {
        set_error(error, "Could not initialize SHA-256");
        return false;
    }

    std::array<char, kIoBufferSize> buffer{};
    while (input) {
        input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const auto count = input.gcount();
        if (count > 0 &&
            EVP_DigestUpdate(context.get(), buffer.data(),
                             static_cast<std::size_t>(count)) != 1) {
            set_error(error, "SHA-256 update failed");
            return false;
        }
    }
    if (input.bad()) {
        set_error(error, "Reading input for SHA-256 failed");
        return false;
    }

    unsigned int digest_size = 0;
    if (EVP_DigestFinal_ex(context.get(), digest.data(), &digest_size) != 1 ||
        digest_size != digest.size()) {
        set_error(error, "Finalizing SHA-256 failed");
        return false;
    }
    return true;
}

}  // namespace encryption_portable
