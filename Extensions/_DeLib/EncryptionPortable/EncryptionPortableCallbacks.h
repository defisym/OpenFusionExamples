#ifndef ENCRYPTION_PORTABLE_CALLBACKS_H
#define ENCRYPTION_PORTABLE_CALLBACKS_H

#include "EncryptionPortable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef EncryptionResult (*EncryptionReadCallback)(
    void* user_data, uint8_t* buffer, size_t capacity, size_t* bytes_read);
typedef EncryptionResult (*EncryptionWriteCallback)(
    void* user_data, const uint8_t* data, size_t data_length);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENCRYPTION_PORTABLE_CALLBACKS_H
