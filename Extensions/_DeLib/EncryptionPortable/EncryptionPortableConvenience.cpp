#include "EncryptionPortableConvenience.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

namespace {

constexpr size_t kReaderBufferSize = 64 * 1024;

struct BufferSink {
    uint8_t* output;
    size_t capacity;
    size_t size;
};

EncryptionResult append_to_buffer(void* user_data,
                                  const uint8_t* data,
                                  size_t data_length) {
    auto* sink = static_cast<BufferSink*>(user_data);
    if (data_length > sink->capacity - sink->size) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }
    if (data_length != 0) {
        std::memcpy(sink->output + sink->size, data, data_length);
        sink->size += data_length;
    }
    return ENCRYPTION_OK;
}

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

// The single file-reader/decryptor backend. Streaming playback and full-buffer
// output differ only in the write callback supplied by the caller.
EncryptionResult decrypt_reader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback,
    void* read_user_data,
    EncryptionWriteCallback write_callback,
    void* write_user_data) {
    if (key == nullptr || iv == nullptr || read_callback == nullptr ||
        write_callback == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    EncryptionCipherContext* context = nullptr;
    auto result = Encryption_CipherCreate(ENCRYPTION_DECRYPT, key, iv, &context);
    if (result != ENCRYPTION_OK) {
        return result;
    }

    std::array<uint8_t, kReaderBufferSize> input{};
    std::array<uint8_t, kReaderBufferSize + ENCRYPTION_BLOCK_SIZE> plaintext{};

    for (;;) {
        size_t bytes_read = 0;
        result = read_callback(read_user_data, input.data(), input.size(), &bytes_read);
        if (result != ENCRYPTION_OK) {
            break;
        }
        if (bytes_read > input.size()) {
            result = ENCRYPTION_INVALID_ARGUMENT;
            break;
        }
        if (bytes_read == 0) {
            break;
        }

        size_t produced = 0;
        result = Encryption_CipherUpdate(context, input.data(), bytes_read,
                                         plaintext.data(), plaintext.size(), &produced);
        if (result != ENCRYPTION_OK) {
            break;
        }
        if (produced != 0) {
            result = write_callback(write_user_data, plaintext.data(), produced);
            if (result != ENCRYPTION_OK) {
                break;
            }
        }
    }

    if (result == ENCRYPTION_OK) {
        size_t produced = 0;
        result = Encryption_CipherFinal(context, plaintext.data(), plaintext.size(),
                                        &produced);
        if (result == ENCRYPTION_OK && produced != 0) {
            result = write_callback(write_user_data, plaintext.data(), produced);
        }
    }

    Encryption_CipherDestroy(context);
    return result;
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

EncryptionResult Encryption_DecryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    EncryptionWriteCallback write_callback, void* write_user_data) {
    return decrypt_reader(key, iv, read_callback, read_user_data,
                          write_callback, write_user_data);
}

EncryptionResult Encryption_DecryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) {
        *output_length = 0;
    }
    if (output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    BufferSink sink{output, output_capacity, 0};
    const auto result = decrypt_reader(key, iv, read_callback, read_user_data,
                                       append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) {
        *output_length = sink.size;
    }
    return result;
}

}  // extern "C"
