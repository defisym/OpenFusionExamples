#ifndef ENCRYPTION_PORTABLE_H
#define ENCRYPTION_PORTABLE_H

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) && defined(ENCRYPTION_PORTABLE_SHARED)
# ifdef ENCRYPTION_PORTABLE_BUILD
#  define ENCRYPTION_API __declspec(dllexport)
# else
#  define ENCRYPTION_API __declspec(dllimport)
# endif
#else
# define ENCRYPTION_API
#endif

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
    ENCRYPTION_INVALID_STATE,
    ENCRYPTION_IO_ERROR
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
typedef struct EncryptionReaderContext EncryptionReaderContext;

// Random access to caller-owned ciphertext. Callback and user_data must outlive
// the reader. A successful short read is allowed; zero before EOF is an I/O error.
typedef EncryptionResult (*EncryptionReadAtCallback)(void* user_data,
    uint64_t offset, uint8_t* output, size_t capacity, size_t* bytes_read);
ENCRYPTION_API EncryptionResult Encryption_ReaderCreate(uint64_t encrypted_size,
    EncryptionReadAtCallback read_at, void* user_data,
    const uint8_t key[16], const uint8_t iv[16], EncryptionReaderContext** context);
ENCRYPTION_API EncryptionResult Encryption_ReaderRead(EncryptionReaderContext* context,
    uint8_t* output, size_t capacity, size_t* bytes_read);
ENCRYPTION_API EncryptionResult Encryption_ReaderSeek(EncryptionReaderContext* context, uint64_t offset);
ENCRYPTION_API EncryptionResult Encryption_ReaderSize(const EncryptionReaderContext* context, uint64_t* size);
ENCRYPTION_API void Encryption_ReaderDestroy(EncryptionReaderContext* context);

// Legacy Win32 asset derivation from explicitly supplied UTF-16LE bytes.
// Odd trailing bytes have a zero high byte. Empty input selects legacy defaults.
// New applications should pass their own key/IV to CipherCreate.
ENCRYPTION_API EncryptionResult Encryption_DeriveKeyAndIv(
    const void* input,
    size_t input_size,
    uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    uint8_t iv[ENCRYPTION_AES_IV_SIZE]);

ENCRYPTION_API EncryptionResult Encryption_DeriveLegacyKeyAndIv(
    const void* input, size_t input_size, unsigned word_bits,
    uint8_t key[ENCRYPTION_AES_KEY_SIZE], uint8_t iv[ENCRYPTION_AES_IV_SIZE]);

// Incremental AES-128-CBC with PKCS#7 padding. Create one context per
// operation. Each Update output buffer must have input_length + 16 bytes
// capacity. Call Final exactly once at end-of-input; it may emit up to 16
// bytes. Update/Final report actual output length through output_length.
ENCRYPTION_API EncryptionResult Encryption_CipherCreate(
    EncryptionCipherMode mode,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionCipherContext** context);
ENCRYPTION_API EncryptionResult Encryption_CipherUpdate(
    EncryptionCipherContext* context,
    const uint8_t* input,
    size_t input_length,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_CipherFinal(
    EncryptionCipherContext* context,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
ENCRYPTION_API void Encryption_CipherDestroy(EncryptionCipherContext* context);

// Incremental hashes. MD5 is available to match the legacy GetHash default;
// SHA-256 is available for new callers.
ENCRYPTION_API EncryptionResult Encryption_HashCreate(
    EncryptionHashAlgorithm algorithm,
    EncryptionHashContext** context);
ENCRYPTION_API EncryptionResult Encryption_HashUpdate(
    EncryptionHashContext* context,
    const uint8_t* input,
    size_t input_length);
ENCRYPTION_API EncryptionResult Encryption_HashFinal(
    EncryptionHashContext* context,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_length);
ENCRYPTION_API void Encryption_HashDestroy(EncryptionHashContext* context);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_H
