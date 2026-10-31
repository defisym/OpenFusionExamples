#include "EncryptionPortableInternal.h"
#include "../EncryptionLegacy.h"
using encryption::c_api::checked;
using encryption::c_api::material;

struct EncryptionCipherContext {
    encryption::Cipher cipher;
    explicit EncryptionCipherContext(const encryption::KeyIv& material, encryption::Mode mode)
        : cipher(material, mode) {}
};
struct EncryptionHashContext {
    encryption::Hash hash;
    explicit EncryptionHashContext(encryption::Digest algorithm) : hash(algorithm) {}
};
struct EncryptionReaderContext {
    encryption::DecryptReader reader;
    explicit EncryptionReaderContext(encryption::DecryptReader value) : reader(std::move(value)) {}
};
class CallbackSource final : public encryption::RandomAccessSource {
    uint64_t size_;
    EncryptionReadAtCallback callback_;
    void* user_;
public:
    CallbackSource(uint64_t size, EncryptionReadAtCallback callback, void* user)
        : size_(size), callback_(callback), user_(user) {}
    uint64_t size() const override { return size_; }
    size_t read_at(uint64_t offset, uint8_t* output, size_t capacity) override {
        size_t count = 0;
        const auto result = callback_(user_, offset, output, capacity, &count);
        encryption::require(result == ENCRYPTION_OK && count <= capacity,
            encryption::ErrorCode::io, "Read ciphertext callback");
        return count;
    }
};
extern "C" {
EncryptionResult Encryption_DeriveKeyAndIv(const void* input, size_t size,
    uint8_t key[16], uint8_t iv[16]) {
    return Encryption_DeriveLegacyKeyAndIv(input, size, 32, key, iv);
}
EncryptionResult Encryption_DeriveLegacyKeyAndIv(const void* input, size_t size,
    unsigned word_bits, uint8_t key[16], uint8_t iv[16]) {
    if ((!input && size) || !key || !iv || (word_bits != 32 && word_bits != 64))
        return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] {
        const auto* bytes = static_cast<const uint8_t*>(input);
        std::u16string text;
        text.reserve(size / 2 + size % 2);
        for (size_t i = 0; i < size;) {
            uint16_t unit = bytes[i++];
            if (i < size) unit |= uint16_t(bytes[i++]) << 8;
            text.push_back(static_cast<char16_t>(unit));
        }
        const auto result = encryption::legacy::derive(text, word_bits == 32
            ? encryption::legacy::WordSize::win32 : encryption::legacy::WordSize::win64);
        std::copy(result.key.begin(), result.key.end(), key);
        std::copy(result.iv.begin(), result.iv.end(), iv);
    });
}
EncryptionResult Encryption_CipherCreate(EncryptionCipherMode mode, const uint8_t key[16],
    const uint8_t iv[16], EncryptionCipherContext** context) {
    if (context) *context = nullptr;
    if (!key || !iv || !context || (mode != ENCRYPTION_ENCRYPT && mode != ENCRYPTION_DECRYPT))
        return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] { *context = new EncryptionCipherContext(material(key, iv),
        mode == ENCRYPTION_ENCRYPT ? encryption::Mode::encrypt : encryption::Mode::decrypt); });
}
EncryptionResult Encryption_CipherUpdate(EncryptionCipherContext* context,
    const uint8_t* input, size_t length, uint8_t* output, size_t capacity, size_t* written) {
    if (written) *written = 0;
    if (!context || !written || (!input && length) || (!output && capacity))
        return ENCRYPTION_INVALID_ARGUMENT;
    if (length > (std::numeric_limits<size_t>::max)() - 16) return ENCRYPTION_INVALID_ARGUMENT;
    if (length && (!output || capacity < length + 16)) return ENCRYPTION_BUFFER_TOO_SMALL;
    return checked([&] {
        const auto result = context->cipher.update(input, length);
        if (!result.empty()) std::memcpy(output, result.data(), result.size());
        *written = result.size();
    });
}
EncryptionResult Encryption_CipherFinal(EncryptionCipherContext* context,
    uint8_t* output, size_t capacity, size_t* written) {
    if (written) *written = 0;
    if (!context || !output || !written) return ENCRYPTION_INVALID_ARGUMENT;
    if (capacity < 16) return ENCRYPTION_BUFFER_TOO_SMALL;
    return checked([&] {
        const auto result = context->cipher.finish();
        if (!result.empty()) std::memcpy(output, result.data(), result.size());
        *written = result.size();
    });
}
void Encryption_CipherDestroy(EncryptionCipherContext* context) { delete context; }
EncryptionResult Encryption_HashCreate(EncryptionHashAlgorithm algorithm, EncryptionHashContext** context) {
    if (context) *context = nullptr;
    if (!context || (algorithm != ENCRYPTION_HASH_MD5 && algorithm != ENCRYPTION_HASH_SHA256))
        return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] { *context = new EncryptionHashContext(algorithm == ENCRYPTION_HASH_MD5
        ? encryption::Digest::md5 : encryption::Digest::sha256); });
}
EncryptionResult Encryption_HashUpdate(EncryptionHashContext* context, const uint8_t* input, size_t length) {
    if (!context || (!input && length)) return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] { context->hash.update(input, length); });
}
EncryptionResult Encryption_HashFinal(EncryptionHashContext* context,
    uint8_t* output, size_t capacity, size_t* written) {
    if (written) *written = 0;
    if (!context || !output || !written) return ENCRYPTION_INVALID_ARGUMENT;
    if (capacity < context->hash.size()) return ENCRYPTION_BUFFER_TOO_SMALL;
    return checked([&] {
        const auto result = context->hash.finish();
        std::memcpy(output, result.data(), result.size());
        *written = result.size();
    });
}
void Encryption_HashDestroy(EncryptionHashContext* context) { delete context; }
EncryptionResult Encryption_ReaderCreate(uint64_t size, EncryptionReadAtCallback read_at,
    void* user, const uint8_t key[16], const uint8_t iv[16], EncryptionReaderContext** context) {
    if (context) *context = nullptr;
    if (!read_at || !key || !iv || !context) return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] {
        *context = new EncryptionReaderContext(encryption::DecryptReader(
            std::make_shared<CallbackSource>(size, read_at, user), material(key, iv)));
    });
}
EncryptionResult Encryption_ReaderRead(EncryptionReaderContext* context,
    uint8_t* output, size_t capacity, size_t* count) {
    if (count) *count = 0;
    if (!context || !count || (!output && capacity)) return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] { *count = context->reader.read(output, capacity); });
}
EncryptionResult Encryption_ReaderSeek(EncryptionReaderContext* context, uint64_t offset) {
    if (!context) return ENCRYPTION_INVALID_ARGUMENT;
    return checked([&] { context->reader.seek(offset); });
}
EncryptionResult Encryption_ReaderSize(const EncryptionReaderContext* context, uint64_t* size) {
    if (!context || !size) return ENCRYPTION_INVALID_ARGUMENT;
    *size = context->reader.size();
    return ENCRYPTION_OK;
}
void Encryption_ReaderDestroy(EncryptionReaderContext* context) { delete context; }
} // extern C
