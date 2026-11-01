#ifndef ENCRYPTION_PORTABLE_BUFFER_H
#define ENCRYPTION_PORTABLE_BUFFER_H

#include "EncryptionPortableCallbacks.h"

#ifdef __cplusplus
extern "C" {
#endif

// One-shot helpers for data already in memory. Encryption needs input_length
// + 16 output bytes; decryption needs at most ciphertext length bytes.
ENCRYPTION_API EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length);

// Reads through a callback and collects the result in caller-owned memory.
// Discard output if an error is returned.
ENCRYPTION_API EncryptionResult Encryption_EncryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_DecryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_BUFFER_H
