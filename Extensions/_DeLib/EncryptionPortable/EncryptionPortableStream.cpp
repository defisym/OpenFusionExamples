#include "EncryptionPortableStream.h"

#include <array>

namespace {

constexpr size_t kReaderBufferSize = 64 * 1024;

// The single incremental reader/cipher/writer path. Encryption and decryption
// differ only in the selected mode.
EncryptionResult cipher_reader(
    EncryptionCipherMode mode,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    EncryptionWriteCallback write_callback, void* write_user_data) {
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
        result = Encryption_CipherFinal(context, output.data(), output.size(), &produced);
        if (result == ENCRYPTION_OK && produced != 0) {
            result = write_callback(write_user_data, output.data(), produced);
        }
    }

    Encryption_CipherDestroy(context);
    return result;
}

}  // namespace

extern "C" {

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

}  // extern "C"
