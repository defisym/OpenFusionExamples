#include "EncryptionPortable.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <new>
#include <vector>

#include <openssl/evp.h>

struct EncryptionCipherContext {
    EVP_CIPHER_CTX* evp;
    EncryptionCipherMode mode;
    bool finalized;
};

struct EncryptionHashContext {
    EVP_MD_CTX* evp;
    size_t digest_size;
    bool finalized;
};

namespace {

constexpr uint32_t kHasherMagicNumber = 0x9e3779b9u;
constexpr size_t kIoBufferSize = 64 * 1024;

uint32_t hasher_move(uint32_t seed) {
    return kHasherMagicNumber + (seed << 6) + (seed >> 2);
}

bool append_utf16(uint32_t value, std::vector<uint16_t>& output) {
    if (value > 0x10ffffu) {
        return false;
    }
    if (value <= 0xffffu) {
        // Preserve every 16-bit value as key material, including surrogate-
        // range values. This is not text validation.
        output.push_back(static_cast<uint16_t>(value));
        return true;
    }

    value -= 0x10000u;
    output.push_back(static_cast<uint16_t>(0xd800u + (value >> 10)));
    output.push_back(static_cast<uint16_t>(0xdc00u + (value & 0x3ffu)));
    return true;
}

bool to_utf16_units(const wchar_t* input, size_t length,
                    std::vector<uint16_t>& output) {
    output.clear();
    output.reserve(length);

    if constexpr (sizeof(wchar_t) == 2) {
        for (size_t i = 0; i < length; ++i) {
            output.push_back(static_cast<uint16_t>(input[i]));
        }
        return true;
    } else if constexpr (sizeof(wchar_t) == 4) {
        for (size_t i = 0; i < length; ++i) {
            if (!append_utf16(static_cast<uint32_t>(input[i]), output)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void store_word_le(uint16_t word, uint8_t* output) {
    output[0] = static_cast<uint8_t>(word & 0xffu);
    output[1] = static_cast<uint8_t>(word >> 8);
}

const EVP_MD* get_digest(EncryptionHashAlgorithm algorithm) {
    switch (algorithm) {
    case ENCRYPTION_HASH_MD5:
        return EVP_md5();
    case ENCRYPTION_HASH_SHA256:
        return EVP_sha256();
    default:
        return nullptr;
    }
}

}  // namespace

extern "C" {

EncryptionResult Encryption_DeriveKeyAndIv(
    const wchar_t* input, size_t input_length,
    uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if ((input == nullptr && input_length != 0) || key == nullptr || iv == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    std::vector<uint16_t> units;
    if (input_length != 0 && !to_utf16_units(input, input_length, units)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    std::array<uint16_t, ENCRYPTION_KEY_IV_WORD_COUNT> material{};
    if (units.empty()) {
        // Match legacy DefaultKey/DefaultIV bytes 00,01,...,0F.
        for (size_t i = 0; i < 8; ++i) {
            const auto low = static_cast<uint8_t>(i * 2);
            const auto high = static_cast<uint8_t>(i * 2 + 1);
            material[i] = static_cast<uint16_t>(low | (high << 8));
            material[i + 8] = material[i];
        }
    } else if (units.size() < material.size()) {
        std::copy(units.begin(), units.end(), material.begin());
        uint32_t seed = static_cast<uint32_t>(units.size());
        for (size_t i = units.size(); i > 0; --i) {
            seed ^= static_cast<uint32_t>(units[i - 1]) + hasher_move(seed);
        }
        for (size_t i = units.size(); i < material.size(); ++i) {
            const uint16_t high = static_cast<uint16_t>((seed >> 8) & 0xffu);
            const uint16_t low = static_cast<uint16_t>(seed & 0xffu);
            material[i] = static_cast<uint16_t>(high ^ low);
            seed ^= static_cast<uint32_t>(material[i]) + hasher_move(seed);
        }
    } else {
        std::copy_n(units.begin(), material.size(), material.begin());
        for (size_t i = material.size(); i < units.size(); ++i) {
            const uint16_t value = units[i];
            for (size_t word = 0; word < 8; ++word) {
                material[word] ^= value;
                material[word + 8] ^= value;
            }
        }
    }

    for (size_t i = 0; i < 8; ++i) {
        store_word_le(material[i], key + i * 2);
        store_word_le(material[i + 8], iv + i * 2);
    }
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_CipherCreate(
    EncryptionCipherMode mode,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionCipherContext** context) {
    if (key == nullptr || iv == nullptr || context == nullptr ||
        (mode != ENCRYPTION_ENCRYPT && mode != ENCRYPTION_DECRYPT)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    *context = nullptr;

    EncryptionCipherContext* result = new (std::nothrow) EncryptionCipherContext{};
    if (result == nullptr) {
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    result->evp = EVP_CIPHER_CTX_new();
    result->mode = mode;
    result->finalized = false;
    if (result->evp == nullptr) {
        delete result;
        return ENCRYPTION_OUT_OF_MEMORY;
    }

    const int encrypt = mode == ENCRYPTION_ENCRYPT ? 1 : 0;
    if (EVP_CipherInit_ex(result->evp, EVP_aes_128_cbc(), nullptr,
                          key, iv, encrypt) != 1) {
        EVP_CIPHER_CTX_free(result->evp);
        delete result;
        return ENCRYPTION_CRYPTO_ERROR;
    }

    *context = result;
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_CipherUpdate(
    EncryptionCipherContext* context,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (context == nullptr || output_length == nullptr ||
        (input == nullptr && input_length != 0) ||
        (output == nullptr && output_capacity != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (context->finalized) {
        return ENCRYPTION_INVALID_STATE;
    }
    if (input_length == 0) {
        return ENCRYPTION_OK;
    }
    if (input_length > static_cast<size_t>(std::numeric_limits<int>::max()) ||
        input_length > std::numeric_limits<size_t>::max() - ENCRYPTION_BLOCK_SIZE) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (output == nullptr ||
        output_capacity < input_length + ENCRYPTION_BLOCK_SIZE) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }

    int produced = 0;
    if (EVP_CipherUpdate(context->evp, output, &produced, input,
                         static_cast<int>(input_length)) != 1) {
        return ENCRYPTION_CRYPTO_ERROR;
    }
    *output_length = static_cast<size_t>(produced);
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_CipherFinal(
    EncryptionCipherContext* context,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (context == nullptr || output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (context->finalized) {
        return ENCRYPTION_INVALID_STATE;
    }
    if (output_capacity < ENCRYPTION_BLOCK_SIZE) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }

    context->finalized = true;
    int produced = 0;
    if (EVP_CipherFinal_ex(context->evp, output, &produced) != 1) {
        return context->mode == ENCRYPTION_DECRYPT
            ? ENCRYPTION_INVALID_PADDING
            : ENCRYPTION_CRYPTO_ERROR;
    }
    *output_length = static_cast<size_t>(produced);
    return ENCRYPTION_OK;
}

void Encryption_CipherDestroy(EncryptionCipherContext* context) {
    if (context != nullptr) {
        EVP_CIPHER_CTX_free(context->evp);
        delete context;
    }
}

EncryptionResult Encryption_HashCreate(
    EncryptionHashAlgorithm algorithm,
    EncryptionHashContext** context) {
    if (context == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    *context = nullptr;

    const EVP_MD* digest = get_digest(algorithm);
    if (digest == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    EncryptionHashContext* result = new (std::nothrow) EncryptionHashContext{};
    if (result == nullptr) {
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    result->evp = EVP_MD_CTX_new();
    result->digest_size = static_cast<size_t>(EVP_MD_size(digest));
    result->finalized = false;
    if (result->evp == nullptr) {
        delete result;
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    if (EVP_DigestInit_ex(result->evp, digest, nullptr) != 1) {
        EVP_MD_CTX_free(result->evp);
        delete result;
        return ENCRYPTION_CRYPTO_ERROR;
    }

    *context = result;
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_HashUpdate(
    EncryptionHashContext* context,
    const uint8_t* input, size_t input_length) {
    if (context == nullptr || (input == nullptr && input_length != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (context->finalized) {
        return ENCRYPTION_INVALID_STATE;
    }
    if (input_length == 0) {
        return ENCRYPTION_OK;
    }
    return EVP_DigestUpdate(context->evp, input, input_length) == 1
        ? ENCRYPTION_OK
        : ENCRYPTION_CRYPTO_ERROR;
}

EncryptionResult Encryption_HashFinal(
    EncryptionHashContext* context,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (context == nullptr || output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (context->finalized) {
        return ENCRYPTION_INVALID_STATE;
    }
    if (output_capacity < context->digest_size) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }

    context->finalized = true;
    unsigned int produced = 0;
    if (EVP_DigestFinal_ex(context->evp, output, &produced) != 1) {
        return ENCRYPTION_CRYPTO_ERROR;
    }
    *output_length = static_cast<size_t>(produced);
    return ENCRYPTION_OK;
}

void Encryption_HashDestroy(EncryptionHashContext* context) {
    if (context != nullptr) {
        EVP_MD_CTX_free(context->evp);
        delete context;
    }
}

}  // extern "C"
