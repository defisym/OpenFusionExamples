#include "Encryption/CAPI.h"
#include "Encryption/CAPIBuffer.h"
#include "Encryption/CAPIStream.h"
#include "Encryption/CAPIFile.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
struct Source { const unsigned char* bytes; size_t length; };
static EncryptionResult fail_read(void* user, uint8_t* output, size_t capacity, size_t* count) {
    (void)user; (void)output; (void)capacity; *count = 0;
    return ENCRYPTION_IO_ERROR;
}
static EncryptionResult discard(void* user, const uint8_t* bytes, size_t size) {
    (void)user; (void)bytes; (void)size;
    return ENCRYPTION_OK;
}
static EncryptionResult read_once(void* user, uint8_t* output, size_t capacity, size_t* count) {
    struct Source* source = (struct Source*)user;
    *count = source->length < capacity ? source->length : capacity;
    memcpy(output, source->bytes, *count);
    source->bytes += *count; source->length -= *count;
    return ENCRYPTION_OK;
}
static EncryptionResult fail_write(void* user, const uint8_t* bytes, size_t size) {
    (void)user; (void)bytes; (void)size;
    return ENCRYPTION_IO_ERROR;
}
static EncryptionResult read_at(void* user, uint64_t offset, uint8_t* output, size_t capacity, size_t* count) {
    struct Source* source = (struct Source*)user;
    if (offset > source->length) return ENCRYPTION_IO_ERROR;
    *count = source->length - (size_t)offset;
    if (*count > capacity) *count = capacity;
    memcpy(output, source->bytes + offset, *count);
    return ENCRYPTION_OK;
}
int test_c_api(void) {
    uint8_t key[16], iv[16], cipher[64], output[64];
    size_t n = 0, count = 0;
    uint64_t length = 0;
    EncryptionCipherContext* context = NULL;
    EncryptionReaderContext* reader = NULL;
    const uint8_t plain[] = "sample asset";
    struct Source source;
    FILE* file;
    const uint8_t abc_md5[16] = {0x90,0x01,0x50,0x98,0x3c,0xd2,0x4f,0xb0,0xd6,0x96,0x3f,0x7d,0x28,0xe1,0x7f,0x72};
    const uint8_t empty_md5[16] = {0xd4,0x1d,0x8c,0xd9,0x8f,0x00,0xb2,0x04,0xe9,0x80,0x09,0x98,0xec,0xf8,0x42,0x7e};
    CHECK(Encryption_DeriveKeyAndIv(NULL, 0, key, iv) == ENCRYPTION_OK);
    CHECK(Encryption_EncryptReader(key, iv, fail_read, NULL, discard, NULL) == ENCRYPTION_IO_ERROR);
    source.bytes = plain; source.length = sizeof plain;
    CHECK(Encryption_EncryptReader(key, iv, read_once, &source, fail_write, NULL) == ENCRYPTION_IO_ERROR);
    CHECK(Encryption_EncryptBuffer(key, iv, plain, sizeof plain, cipher, sizeof cipher, &n) == ENCRYPTION_OK);
    source.bytes = cipher; source.length = n;
    CHECK(Encryption_ReaderCreate(n, read_at, &source, key, iv, &reader) == ENCRYPTION_OK);
    CHECK(Encryption_ReaderSize(reader, &length) == ENCRYPTION_OK && length == sizeof plain);
    CHECK(Encryption_ReaderSeek(reader, 3) == ENCRYPTION_OK);
    CHECK(Encryption_ReaderRead(reader, output, 64, &count) == ENCRYPTION_OK && count == sizeof plain - 3);
    CHECK(memcmp(output, plain + 3, count) == 0);
    CHECK(Encryption_ReaderRead(reader, output, 64, &count) == ENCRYPTION_OK && count == 0);
    CHECK(Encryption_ReaderSeek(reader, sizeof plain + 1) == ENCRYPTION_INVALID_ARGUMENT);
    Encryption_ReaderDestroy(reader);
    CHECK(Encryption_CipherCreate(ENCRYPTION_ENCRYPT, key, iv, &context) == ENCRYPTION_OK);
    CHECK(Encryption_CipherUpdate(context, plain, sizeof plain, output, 1, &count) == ENCRYPTION_BUFFER_TOO_SMALL);
    CHECK(Encryption_CipherUpdate(context, plain, sizeof plain, output, 64, &count) == ENCRYPTION_OK);
    CHECK(Encryption_CipherFinal(context, output, 1, &count) == ENCRYPTION_BUFFER_TOO_SMALL);
    CHECK(Encryption_CipherFinal(context, output, 64, &count) == ENCRYPTION_OK);
    CHECK(Encryption_CipherFinal(context, output, 64, &count) == ENCRYPTION_INVALID_STATE);
    CHECK(Encryption_CipherUpdate(context, NULL, 0, NULL, 0, &count) == ENCRYPTION_INVALID_STATE);
    Encryption_CipherDestroy(context);
    file = tmpfile();
    CHECK(file != NULL && fwrite("abc", 1, 3, file) == 3);
    rewind(file);
    CHECK(Encryption_HashFile(file, ENCRYPTION_HASH_MD5, output, sizeof output, &count) == ENCRYPTION_OK);
    CHECK(count == 16 && memcmp(output, abc_md5, 16) == 0);
    CHECK(Encryption_HashFile(file, ENCRYPTION_HASH_MD5, output, sizeof output, &count) == ENCRYPTION_OK);
    CHECK(count == 16 && memcmp(output, empty_md5, 16) == 0); /* caller's current position is EOF */
    CHECK(fclose(file) == 0);
    return 0;
}
