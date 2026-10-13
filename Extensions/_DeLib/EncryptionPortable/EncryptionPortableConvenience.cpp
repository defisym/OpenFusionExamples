#include "EncryptionPortableConvenience.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <cstdio>
#include <limits>
#include <new>
#include <vector>

namespace {

constexpr size_t kReaderBufferSize = 64 * 1024;

struct FileReader { FILE* file; };
struct FileWriter { FILE* file; };

EncryptionResult read_file(void* user_data, uint8_t* buffer, size_t capacity, size_t* bytes_read) {
    if (user_data == nullptr || buffer == nullptr || bytes_read == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    auto* reader = static_cast<FileReader*>(user_data);
    *bytes_read = std::fread(buffer, 1, capacity, reader->file);
    return std::ferror(reader->file) ? ENCRYPTION_IO_ERROR : ENCRYPTION_OK;
}

EncryptionResult write_file(void* user_data, const uint8_t* data, size_t data_length) {
    if (user_data == nullptr || (data == nullptr && data_length != 0)) return ENCRYPTION_INVALID_ARGUMENT;
    auto* writer = static_cast<FileWriter*>(user_data);
    return std::fwrite(data, 1, data_length, writer->file) == data_length
        ? ENCRYPTION_OK : ENCRYPTION_IO_ERROR;
}
struct MemoryReader {
    const uint8_t* input;
    size_t length;
    size_t offset;
};

EncryptionResult read_memory(void* user_data, uint8_t* buffer,
                             size_t capacity, size_t* bytes_read) {
    if (user_data == nullptr || buffer == nullptr || bytes_read == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* reader = static_cast<MemoryReader*>(user_data);
    const size_t remaining = reader->length - reader->offset;
    const size_t count = std::min(capacity, remaining);
    if (count != 0) {
        std::memcpy(buffer, reader->input + reader->offset, count);
        reader->offset += count;
    }
    *bytes_read = count;
    return ENCRYPTION_OK;
}

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

// One reader/cipher backend handles encryption and decryption. Callers choose
// the mode and output sink; both modes share chunking, update, and finalization.
EncryptionResult cipher_reader(
    EncryptionCipherMode mode,
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
    auto result = Encryption_CipherCreate(mode, key, iv, &context);
    if (result != ENCRYPTION_OK) {
        return result;
    }

    std::array<uint8_t, kReaderBufferSize> input{};
    std::array<uint8_t, kReaderBufferSize + ENCRYPTION_BLOCK_SIZE> output{};

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
                                         output.data(), output.size(), &produced);
        if (result != ENCRYPTION_OK) {
            break;
        }
        if (produced != 0) {
            result = write_callback(write_user_data, output.data(), produced);
            if (result != ENCRYPTION_OK) {
                break;
            }
        }
    }

    if (result == ENCRYPTION_OK) {
        size_t produced = 0;
        result = Encryption_CipherFinal(context, output.data(), output.size(),
                                        &produced);
        if (result == ENCRYPTION_OK && produced != 0) {
            result = write_callback(write_user_data, output.data(), produced);
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
    if (input == nullptr && input_length != 0) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (input_length > std::numeric_limits<size_t>::max() - ENCRYPTION_BLOCK_SIZE) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    std::vector<uint8_t> scratch;
    try {
        scratch.resize(input_length + ENCRYPTION_BLOCK_SIZE);
    } catch (const std::bad_alloc&) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    MemoryReader reader{input, input_length, 0};
    size_t produced = 0;
    const auto result = Encryption_EncryptReaderToBuffer(
        key, iv, read_memory, &reader, scratch.data(), scratch.size(), &produced);
    if (result != ENCRYPTION_OK) {
        if (output_length != nullptr) *output_length = 0;
        return result;
    }
    if (output_length == nullptr || output_capacity < produced || output == nullptr) {
        if (output_length != nullptr) *output_length = 0;
        return output_length == nullptr ? ENCRYPTION_INVALID_ARGUMENT
                                        : ENCRYPTION_BUFFER_TOO_SMALL;
    }
    std::memcpy(output, scratch.data(), produced);
    *output_length = produced;
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (input == nullptr && input_length != 0) {
        if (output_length != nullptr) {
            *output_length = 0;
        }
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    MemoryReader reader{input, input_length, 0};
    return Encryption_DecryptReaderToBuffer(
        key, iv, read_memory, &reader, output, output_capacity, output_length);
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

EncryptionResult Encryption_EncryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    EncryptionWriteCallback write_callback, void* write_user_data) {
    return cipher_reader(ENCRYPTION_ENCRYPT, key, iv, read_callback,
                         read_user_data, write_callback, write_user_data);
}

EncryptionResult Encryption_DecryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    EncryptionWriteCallback write_callback, void* write_user_data) {
    return cipher_reader(ENCRYPTION_DECRYPT, key, iv, read_callback,
                         read_user_data, write_callback, write_user_data);
}

EncryptionResult Encryption_EncryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    BufferSink sink{output, output_capacity, 0};
    const auto result = cipher_reader(ENCRYPTION_ENCRYPT, key, iv,
                                      read_callback, read_user_data,
                                      append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) *output_length = sink.size;
    return result;
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
    const auto result = cipher_reader(ENCRYPTION_DECRYPT, key, iv,
                                      read_callback, read_user_data,
                                      append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) {
        *output_length = sink.size;
    }
    return result;
}

EncryptionResult Encryption_EncryptFile(FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE], const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file}; FileWriter writer{output_file};
    return Encryption_EncryptReader(key, iv, read_file, &reader, write_file, &writer);
}

EncryptionResult Encryption_DecryptFile(FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE], const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file}; FileWriter writer{output_file};
    return Encryption_DecryptReader(key, iv, read_file, &reader, write_file, &writer);
}

EncryptionResult Encryption_EncryptFileToBuffer(FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE], const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) { if (output_length) *output_length = 0; return ENCRYPTION_INVALID_ARGUMENT; }
    FileReader reader{input_file};
    return Encryption_EncryptReaderToBuffer(key, iv, read_file, &reader, output, output_capacity, output_length);
}

EncryptionResult Encryption_DecryptFileToBuffer(FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE], const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) { if (output_length) *output_length = 0; return ENCRYPTION_INVALID_ARGUMENT; }
    FileReader reader{input_file};
    return Encryption_DecryptReaderToBuffer(key, iv, read_file, &reader, output, output_capacity, output_length);
}

EncryptionResult Encryption_HashFile(FILE* input_file, EncryptionHashAlgorithm algorithm,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length) *output_length = 0;
    if (input_file == nullptr || output == nullptr || output_length == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    EncryptionHashContext* context = nullptr;
    auto result = Encryption_HashCreate(algorithm, &context);
    if (result != ENCRYPTION_OK) return result;
    FileReader reader{input_file};
    std::array<uint8_t, 64 * 1024> buffer{};
    for (;;) {
        size_t bytes_read = 0;
        result = read_file(&reader, buffer.data(), buffer.size(), &bytes_read);
        if (result != ENCRYPTION_OK || bytes_read == 0) break;
        result = Encryption_HashUpdate(context, buffer.data(), bytes_read);
        if (result != ENCRYPTION_OK) break;
    }
    if (result == ENCRYPTION_OK) result = Encryption_HashFinal(context, output, output_capacity, output_length);
    Encryption_HashDestroy(context);
    return result;
}
}  // extern "C"
