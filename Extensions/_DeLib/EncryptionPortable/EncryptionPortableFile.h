#ifndef ENCRYPTION_PORTABLE_FILE_H
#define ENCRYPTION_PORTABLE_FILE_H

#include "EncryptionPortableCallbacks.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// FILE helpers use caller-owned handles from their current positions.
// They neither rewind nor close handles. Source and destination must be
// distinct files; discard partial output if an operation fails.
EncryptionResult Encryption_EncryptFileStream(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback,
    void* write_user_data);
EncryptionResult Encryption_DecryptFileStream(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback,
    void* write_user_data);
EncryptionResult Encryption_EncryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]);
EncryptionResult Encryption_DecryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]);
EncryptionResult Encryption_EncryptFileToBuffer(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length);
EncryptionResult Encryption_DecryptFileToBuffer(
    FILE* input_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    uint8_t* output, size_t output_capacity, size_t* output_length);
EncryptionResult Encryption_HashFile(
    FILE* input_file, EncryptionHashAlgorithm algorithm,
    uint8_t* output, size_t output_capacity, size_t* output_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_FILE_H
