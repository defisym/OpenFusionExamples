#ifndef ENCRYPTION_PORTABLE_STREAM_H
#define ENCRYPTION_PORTABLE_STREAM_H

#include "EncryptionPortableCallbacks.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_STREAM_H
