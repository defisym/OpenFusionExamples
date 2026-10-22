#include "EncryptionPortableFile.h"
#include "EncryptionPortableBuffer.h"
#include "EncryptionPortableStream.h"

#include <array>
#include <cstdio>

namespace {

struct FileReader { FILE* file; };
struct FileWriter { FILE* file; };

EncryptionResult read_file(void* user_data, uint8_t* buffer,
                           size_t capacity, size_t* bytes_read) {
    if (user_data == nullptr || buffer == nullptr || bytes_read == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* reader = static_cast<FileReader*>(user_data);
    *bytes_read = std::fread(buffer, 1, capacity, reader->file);
    return std::ferror(reader->file) ? ENCRYPTION_IO_ERROR : ENCRYPTION_OK;
}

EncryptionResult write_file(void* user_data, const uint8_t* data,
                            size_t data_length) {
    if (user_data == nullptr || (data == nullptr && data_length != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* writer = static_cast<FileWriter*>(user_data);
    return std::fwrite(data, 1, data_length, writer->file) == data_length
        ? ENCRYPTION_OK : ENCRYPTION_IO_ERROR;
}

}  // namespace

extern "C" {

EncryptionResult Encryption_EncryptFileStream(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback, void* write_user_data) {
    if (input_file == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file};
    return Encryption_EncryptReader(key, iv, read_file, &reader,
                                    write_callback, write_user_data);
}

EncryptionResult Encryption_DecryptFileStream(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback, void* write_user_data) {
    if (input_file == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file};
    return Encryption_DecryptReader(key, iv, read_file, &reader,
                                    write_callback, write_user_data);
}

EncryptionResult Encryption_EncryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    FileWriter writer{output_file};
    return Encryption_EncryptReader(key, iv, read_file, &reader,
                                    write_file, &writer);
}

EncryptionResult Encryption_DecryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    FileWriter writer{output_file};
    return Encryption_DecryptReader(key, iv, read_file, &reader,
                                    write_file, &writer);
}

EncryptionResult Encryption_EncryptFileToBuffer(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE], uint8_t* output,
    size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    return Encryption_EncryptReaderToBuffer(key, iv, read_file, &reader,
                                             output, output_capacity, output_length);
}

EncryptionResult Encryption_DecryptFileToBuffer(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE], uint8_t* output,
    size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    return Encryption_DecryptReaderToBuffer(key, iv, read_file, &reader,
                                             output, output_capacity, output_length);
}

EncryptionResult Encryption_HashFile(
    FILE* input_file, EncryptionHashAlgorithm algorithm,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (input_file == nullptr || output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
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
    if (result == ENCRYPTION_OK) {
        result = Encryption_HashFinal(context, output, output_capacity, output_length);
    }
    Encryption_HashDestroy(context);
    return result;
}

}  // extern "C"
