#ifndef ENCRYPTION_PORTABLE_CONVENIENCE_H
#define ENCRYPTION_PORTABLE_CONVENIENCE_H

#include "EncryptionPortable.h"

#ifdef __cplusplus
extern "C" {
#endif

// One-shot helpers for data already held in memory. The output buffer must be
// at least input_length + 16 bytes for encryption, input_length bytes for
// decryption, and 16/32 bytes for MD5/SHA-256 respectively.
EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_CONVENIENCE_H
