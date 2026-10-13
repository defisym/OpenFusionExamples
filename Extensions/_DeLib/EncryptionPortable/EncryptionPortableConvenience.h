#ifndef ENCRYPTION_PORTABLE_CONVENIENCE_H
#define ENCRYPTION_PORTABLE_CONVENIENCE_H

#include "EncryptionPortable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef EncryptionResult (*EncryptionReadCallback)(
    void* user_data, uint8_t* buffer, size_t capacity, size_t* bytes_read);
typedef EncryptionResult (*EncryptionWriteCallback)(
    void* user_data, const uint8_t* data, size_t data_length);

// One-shot helpers for data already in memory. Encryption output capacity
// must be input_length + 16 bytes; decryption capacity equal to ciphertext
// length is sufficient; hash capacity must be 16 (MD5) or 32 (SHA-256).
EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);
EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);
EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);

// Reads encrypted input incrementally and sends plaintext chunks to a caller
// callback. A successful read callback returning bytes_read == 0 means EOF.
// Output can have been delivered before a later read/padding error is found.
EncryptionResult Encryption_DecryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback,
    void* read_user_data,
    EncryptionWriteCallback write_callback,
    void* write_user_data);

// Reads encrypted input incrementally but collects the complete plaintext in
// the caller's buffer. Capacity equal to ciphertext length is sufficient.
// Discard the output buffer if the function returns an error.
EncryptionResult Encryption_DecryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback,
    void* read_user_data,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_CONVENIENCE_H
