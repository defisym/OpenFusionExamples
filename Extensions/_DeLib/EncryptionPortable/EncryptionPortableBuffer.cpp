#include "EncryptionPortableBuffer.h"
#include "EncryptionPortableStream.h"
#include "EncryptionPortableInternal.h"

#include <algorithm>
#include <cstring>

namespace {

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

EncryptionResult transform_buffer(encryption::Mode mode, const uint8_t* key, const uint8_t* iv,
    const uint8_t* input, size_t size, uint8_t* output, size_t capacity, size_t* written) {
    if (written) *written = 0;
    if (!key || !iv || !written || (!input && size)) return ENCRYPTION_INVALID_ARGUMENT;
    return encryption::c_api::checked([&] {
        const auto result = encryption::transform_buffer(encryption::c_api::material(key, iv), mode, input, size);
        if (!output || capacity < result.size())
            throw encryption::c_api::CallbackError{ENCRYPTION_BUFFER_TOO_SMALL};
        if (!result.empty()) std::memcpy(output, result.data(), result.size());
        *written = result.size();
    });
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
    const uint8_t key[16], const uint8_t iv[16], const uint8_t* input, size_t size,
    uint8_t* output, size_t capacity, size_t* written) {
    return transform_buffer(encryption::Mode::encrypt, key, iv, input, size, output, capacity, written);
}
EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[16], const uint8_t iv[16], const uint8_t* input, size_t size,
    uint8_t* output, size_t capacity, size_t* written) {
    return transform_buffer(encryption::Mode::decrypt, key, iv, input, size, output, capacity, written);
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
