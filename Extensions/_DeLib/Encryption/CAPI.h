#ifndef ENCRYPTION_CAPI_H
#define ENCRYPTION_CAPI_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) && defined(ENCRYPTION_SHARED)
# ifdef ENCRYPTION_BUILD
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

typedef EncryptionResult (*EncryptionReadCallback)(
    void* user_data, uint8_t* buffer, size_t capacity, size_t* bytes_read);
typedef EncryptionResult (*EncryptionWriteCallback)(
    void* user_data, const uint8_t* data, size_t data_length);

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

// Arbitrary-length password-to-Key/IV mapping from UTF-16LE bytes (32-bit seed).
// Odd trailing bytes have a zero high byte. Empty input selects legacy defaults.
// Feed the result to CipherCreate; equivalent text must use the same UTF-16 units.
ENCRYPTION_API EncryptionResult Encryption_DeriveKeyAndIv(
    const void* input,
    size_t input_size,
    uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    uint8_t iv[ENCRYPTION_AES_IV_SIZE]);

ENCRYPTION_API EncryptionResult Encryption_DeriveKeyAndIvWithSeedWidth(
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

// ------------------------------------------
// Callback streams
// ------------------------------------------

// Push-style streaming: reads until EOF and sends each result chunk to the
// writer callback. bytes_read == 0 with ENCRYPTION_OK signals EOF.
ENCRYPTION_API EncryptionResult Encryption_EncryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback,
    void* read_user_data,
    EncryptionWriteCallback write_callback,
    void* write_user_data);
ENCRYPTION_API EncryptionResult Encryption_DecryptReader(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback,
    void* read_user_data,
    EncryptionWriteCallback write_callback,
    void* write_user_data);

// ------------------------------------------
// Memory buffers
// ------------------------------------------

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

// ------------------------------------------
// FILE handles
// ------------------------------------------

// FILE helpers use caller-owned handles from their current positions.
// They neither rewind nor close handles. Source and destination must be
// distinct files; discard partial output if an operation fails.
ENCRYPTION_API EncryptionResult Encryption_EncryptFileStream(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback,
    void* write_user_data);
ENCRYPTION_API EncryptionResult Encryption_DecryptFileStream(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback,
    void* write_user_data);
ENCRYPTION_API EncryptionResult Encryption_EncryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]);
ENCRYPTION_API EncryptionResult Encryption_DecryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]);
ENCRYPTION_API EncryptionResult Encryption_EncryptFileToBuffer(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_DecryptFileToBuffer(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length);
ENCRYPTION_API EncryptionResult Encryption_HashFile(
    FILE* input_file, EncryptionHashAlgorithm algorithm,
    uint8_t* output, size_t output_capacity, size_t* output_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_CAPI_H
