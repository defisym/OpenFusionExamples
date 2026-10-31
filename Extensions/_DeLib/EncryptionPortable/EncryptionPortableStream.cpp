#include "EncryptionPortableStream.h"
#include "EncryptionPortableInternal.h"

namespace {
EncryptionResult cipher_reader(encryption::Mode mode,
    const uint8_t* key, const uint8_t* iv,
    EncryptionReadCallback read, void* read_user,
    EncryptionWriteCallback write, void* write_user) {
    if (!key || !iv || !read || !write) return ENCRYPTION_INVALID_ARGUMENT;
    return encryption::c_api::checked([&] {
        encryption::transform(encryption::c_api::material(key, iv), mode,
            [&](uint8_t* data, size_t capacity) {
                size_t count = 0;
                encryption::c_api::callback_result(read(read_user, data, capacity, &count));
                return count;
            },
            [&](const uint8_t* data, size_t size) {
                encryption::c_api::callback_result(write(write_user, data, size));
            });
    });
}
}
extern "C" {
EncryptionResult Encryption_EncryptReader(const uint8_t key[16], const uint8_t iv[16],
    EncryptionReadCallback read, void* read_user, EncryptionWriteCallback write, void* write_user) {
    return cipher_reader(encryption::Mode::encrypt, key, iv, read, read_user, write, write_user);
}
EncryptionResult Encryption_DecryptReader(const uint8_t key[16], const uint8_t iv[16],
    EncryptionReadCallback read, void* read_user, EncryptionWriteCallback write, void* write_user) {
    return cipher_reader(encryption::Mode::decrypt, key, iv, read, read_user, write, write_user);
}
} // extern C
