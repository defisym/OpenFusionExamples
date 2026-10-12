#include "EncryptionPortableConvenience.h"

#include <algorithm>
#include <limits>
#include <new>
#include <vector>

namespace {

EncryptionResult cipher_buffer(
    EncryptionCipherMode mode,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (key == nullptr || iv == nullptr || output_length == nullptr ||
        (input == nullptr && input_length != 0) ||
        input_length > static_cast<size_t>(std::numeric_limits<int>::max()) ||
        input_length > std::numeric_limits<size_t>::max() - ENCRYPTION_BLOCK_SIZE) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    std::vector<uint8_t> scratch;
    try {
        scratch.resize(input_length + ENCRYPTION_BLOCK_SIZE);
    } catch (const std::bad_alloc&) {
        return ENCRYPTION_OUT_OF_MEMORY;
    }

    EncryptionCipherContext* context = nullptr;
    auto result = Encryption_CipherCreate(mode, key, iv, &context);
    if (result != ENCRYPTION_OK) {
        return result;
    }

    size_t produced = 0;
    result = Encryption_CipherUpdate(context, input, input_length,
                                     scratch.data(), scratch.size(), &produced);
    if (result == ENCRYPTION_OK) {
        size_t final_size = 0;
        result = Encryption_CipherFinal(context, scratch.data() + produced,
                                        scratch.size() - produced, &final_size);
        produced += final_size;
    }
    Encryption_CipherDestroy(context);

    if (result != ENCRYPTION_OK) {
        return result;
    }
    if (output_capacity < produced || (output == nullptr && produced != 0)) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }
    if (produced != 0) {
        std::copy(scratch.begin(), scratch.begin() + produced, output);
    }
    *output_length = produced;
    return ENCRYPTION_OK;
}

}  // namespace

extern "C" {

EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    return cipher_buffer(ENCRYPTION_ENCRYPT, key, iv, input, input_length,
                         output, output_capacity, output_length);
}

EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    return cipher_buffer(ENCRYPTION_DECRYPT, key, iv, input, input_length,
                         output, output_capacity, output_length);
}

EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (output_length == nullptr || (input == nullptr && input_length != 0) ||
        output == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    EncryptionHashContext* context = nullptr;
    auto result = Encryption_HashCreate(algorithm, &context);
    if (result != ENCRYPTION_OK) {
        return result;
    }
    result = Encryption_HashUpdate(context, input, input_length);
    if (result == ENCRYPTION_OK) {
        result = Encryption_HashFinal(context, output, output_capacity, output_length);
    }
    Encryption_HashDestroy(context);
    return result;
}

}  // extern "C"
