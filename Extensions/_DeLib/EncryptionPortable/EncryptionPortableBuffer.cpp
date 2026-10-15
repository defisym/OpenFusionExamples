#include "EncryptionPortableBuffer.h"
#include "EncryptionPortableStream.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

namespace {

struct MemoryReader {
    const uint8_t* input;
    size_t size;
    size_t offset;
};

EncryptionResult read_memory(void* user_data, uint8_t* buffer,
                             size_t capacity, size_t* bytes_read) {
    if (user_data == nullptr || buffer == nullptr || bytes_read == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* reader = static_cast<MemoryReader*>(user_data);
    const size_t remaining = reader->size - reader->offset;
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

EncryptionResult append_to_buffer(void* user_data, const uint8_t* data,
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

}  // namespace

extern "C" {

EncryptionResult Encryption_EncryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output == nullptr || output_length == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    BufferSink sink{output, output_capacity, 0};
    const auto result = Encryption_EncryptReader(
        key, iv, read_callback, read_user_data, append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) *output_length = sink.size;
    return result;
}

EncryptionResult Encryption_DecryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output == nullptr || output_length == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    BufferSink sink{output, output_capacity, 0};
    const auto result = Encryption_DecryptReader(
        key, iv, read_callback, read_user_data, append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) *output_length = sink.size;
    return result;
}

EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output_length == nullptr || (input == nullptr && input_length != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (input_length > std::numeric_limits<size_t>::max() - ENCRYPTION_BLOCK_SIZE) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }

    std::vector<uint8_t> scratch;
    try {
        scratch.resize(input_length + ENCRYPTION_BLOCK_SIZE);
    } catch (const std::bad_alloc&) {
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    MemoryReader reader{input, input_length, 0};
    size_t produced = 0;
    const auto result = Encryption_EncryptReaderToBuffer(
        key, iv, read_memory, &reader, scratch.data(), scratch.size(), &produced);
    if (result != ENCRYPTION_OK) return result;
    if (output == nullptr || output_capacity < produced) return ENCRYPTION_BUFFER_TOO_SMALL;
    std::memcpy(output, scratch.data(), produced);
    *output_length = produced;
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output_length == nullptr || (input == nullptr && input_length != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    if (input_length == 0) return ENCRYPTION_INVALID_PADDING;

    std::vector<uint8_t> scratch;
    try {
        scratch.resize(input_length);
    } catch (const std::bad_alloc&) {
        return ENCRYPTION_OUT_OF_MEMORY;
    }
    MemoryReader reader{input, input_length, 0};
    size_t produced = 0;
    const auto result = Encryption_DecryptReaderToBuffer(
        key, iv, read_memory, &reader, scratch.data(), scratch.size(), &produced);
    if (result != ENCRYPTION_OK) return result;
    if (output == nullptr || output_capacity < produced) return ENCRYPTION_BUFFER_TOO_SMALL;
    if (produced != 0) std::memcpy(output, scratch.data(), produced);
    *output_length = produced;
    return ENCRYPTION_OK;
}

EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output_length == nullptr || (input == nullptr && input_length != 0) ||
        output == nullptr) return ENCRYPTION_INVALID_ARGUMENT;

    EncryptionHashContext* context = nullptr;
    auto result = Encryption_HashCreate(algorithm, &context);
    if (result == ENCRYPTION_OK) result = Encryption_HashUpdate(context, input, input_length);
    if (result == ENCRYPTION_OK) result = Encryption_HashFinal(context, output, output_capacity, output_length);
    Encryption_HashDestroy(context);
    return result;
}

}  // extern "C"
