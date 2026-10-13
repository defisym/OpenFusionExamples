#ifndef ENCRYPTION_PORTABLE_H
#define ENCRYPTION_PORTABLE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENCRYPTION_AES_KEY_SIZE 16
#define ENCRYPTION_AES_IV_SIZE 16
#define ENCRYPTION_BLOCK_SIZE 16
#define ENCRYPTION_MD5_SIZE 16
#define ENCRYPTION_SHA256_SIZE 32
#define ENCRYPTION_KEY_IV_WORD_COUNT 16

typedef enum EncryptionResult {
    ENCRYPTION_OK = 0,
    ENCRYPTION_INVALID_ARGUMENT,
    ENCRYPTION_OUT_OF_MEMORY,
    ENCRYPTION_BUFFER_TOO_SMALL,
    ENCRYPTION_CRYPTO_ERROR,
    ENCRYPTION_INVALID_PADDING,
    ENCRYPTION_INVALID_STATE
} EncryptionResult;

typedef enum EncryptionCipherMode {
    ENCRYPTION_DECRYPT = 0,
    ENCRYPTION_ENCRYPT = 1
} EncryptionCipherMode;

typedef enum EncryptionHashAlgorithm {
    ENCRYPTION_HASH_MD5 = 0,
    ENCRYPTION_HASH_SHA256 = 1
} EncryptionHashAlgorithm;

typedef struct EncryptionCipherContext EncryptionCipherContext;
typedef struct EncryptionHashContext EncryptionHashContext;

// Derives a deterministic AES key and IV from arbitrary input bytes.
// input_size is measured in bytes; an empty buffer selects legacy defaults.
EncryptionResult Encryption_DeriveKeyAndIv(
    const void* input,
    size_t input_size,
    uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    uint8_t iv[ENCRYPTION_AES_IV_SIZE]);

// Incremental AES-128-CBC with PKCS#7 padding. Create one context per
// operation. Each Update output buffer must have input_length + 16 bytes
// capacity. Call Final exactly once at end-of-input; it may emit up to 16
// bytes. Update/Final report actual output length through output_length.
EncryptionResult Encryption_CipherCreate(
    EncryptionCipherMode mode,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionCipherContext** context);
EncryptionResult Encryption_CipherUpdate(
    EncryptionCipherContext* context,
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
EncryptionResult Encryption_CipherFinal(
    EncryptionCipherContext* context,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
void Encryption_CipherDestroy(EncryptionCipherContext* context);

// Incremental hashes. MD5 is available to match the legacy GetHash default;
// SHA-256 is available for new callers.
EncryptionResult Encryption_HashCreate(
    EncryptionHashAlgorithm algorithm,
    EncryptionHashContext** context);
EncryptionResult Encryption_HashUpdate(
    EncryptionHashContext* context,
    const uint8_t* input,
    size_t input_length);
EncryptionResult Encryption_HashFinal(
    EncryptionHashContext* context,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
void Encryption_HashDestroy(EncryptionHashContext* context);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_H
