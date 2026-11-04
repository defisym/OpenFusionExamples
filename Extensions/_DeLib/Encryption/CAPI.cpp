#include "CAPI.h"
#include "Core.h"
#include "KeyGen.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>

// Shared exception/result translation is private to this implementation.

namespace encryption::c_api {

struct CallbackError { EncryptionResult result; };

inline void callback_result(EncryptionResult result) {
    if (result != ENCRYPTION_OK) throw CallbackError{result};
}

template<class F> EncryptionResult checked(F&& call) noexcept {
    try { call(); return ENCRYPTION_OK; }
    catch (const CallbackError& error) { return error.result; }
    catch (const std::bad_alloc&) { return ENCRYPTION_OUT_OF_MEMORY; }
    catch (const std::length_error&) { return ENCRYPTION_OUT_OF_MEMORY; }
    catch (const Error& error) {
        switch (error.code) {
        case ErrorCode::argument: return ENCRYPTION_INVALID_ARGUMENT;
        case ErrorCode::state: return ENCRYPTION_INVALID_STATE;
        case ErrorCode::padding: return ENCRYPTION_INVALID_PADDING;
        case ErrorCode::io: return ENCRYPTION_IO_ERROR;
        default: return ENCRYPTION_CRYPTO_ERROR;
        }
    } catch (...) { return ENCRYPTION_CRYPTO_ERROR; }
}

inline KeyIv material(const uint8_t* key, const uint8_t* iv) {
    KeyIv result{};
    std::copy_n(key, 16, result.key.begin());
    std::copy_n(iv, 16, result.iv.begin());
    return result;
}

} // namespace encryption::c_api

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

// Callback streams
namespace {

EncryptionResult cipher_reader(encryption::Mode mode,
    const uint8_t* key, const uint8_t* iv,
    EncryptionReadCallback read, void* read_user,
    EncryptionWriteCallback write, void* write_user) {
    if (!key || !iv || !read || !write) return ENCRYPTION_INVALID_ARGUMENT;

    return encryption::c_api::checked([&] {
        encryption::transform(encryption::c_api::material(key, iv), mode,
            [&](uint8_t* data, size_t capacity) {
                size_t count = 0;
                encryption::c_api::callback_result(read(read_user, data, capacity, &count));
                return count;
            },
            [&](const uint8_t* data, size_t size) {
                encryption::c_api::callback_result(write(write_user, data, size));
            });
    });
}
}

// Memory buffers
namespace {

struct BufferSink {
    uint8_t* output;
    size_t capacity;
    size_t size;
};

EncryptionResult append_to_buffer(void* user_data, const uint8_t* data,
                                  size_t data_length) {
    auto* sink = static_cast<BufferSink*>(user_data);
    if (data_length > sink->capacity - sink->size) {
        return ENCRYPTION_BUFFER_TOO_SMALL;
    }
    if (data_length != 0) {
        std::memcpy(sink->output + sink->size, data, data_length);
        sink->size += data_length;
    }
    return ENCRYPTION_OK;
}

EncryptionResult transform_buffer(encryption::Mode mode, const uint8_t* key, const uint8_t* iv,
    const uint8_t* input, size_t size, uint8_t* output, size_t capacity, size_t* written) {
    if (written) *written = 0;
    if (!key || !iv || !written || (!input && size)) return ENCRYPTION_INVALID_ARGUMENT;

    return encryption::c_api::checked([&] {
        const auto result = encryption::transform_buffer(encryption::c_api::material(key, iv), mode, input, size);
        if (!output || capacity < result.size())
            throw encryption::c_api::CallbackError{ENCRYPTION_BUFFER_TOO_SMALL};
        if (!result.empty()) std::memcpy(output, result.data(), result.size());
        *written = result.size();
    });
}

}  // namespace

// FILE handles
namespace {

struct FileReader { FILE* file; };

struct FileWriter { FILE* file; };

EncryptionResult read_file(void* user_data, uint8_t* buffer,
                           size_t capacity, size_t* bytes_read) {
    if (user_data == nullptr || buffer == nullptr || bytes_read == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* reader = static_cast<FileReader*>(user_data);
    *bytes_read = std::fread(buffer, 1, capacity, reader->file);
    return std::ferror(reader->file) ? ENCRYPTION_IO_ERROR : ENCRYPTION_OK;
}

EncryptionResult write_file(void* user_data, const uint8_t* data,
                            size_t data_length) {
    if (user_data == nullptr || (data == nullptr && data_length != 0)) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    auto* writer = static_cast<FileWriter*>(user_data);
    return std::fwrite(data, 1, data_length, writer->file) == data_length
        ? ENCRYPTION_OK : ENCRYPTION_IO_ERROR;
}

}  // namespace

extern "C" {

EncryptionResult Encryption_DeriveKeyAndIv(const void* input, size_t size,
    uint8_t key[16], uint8_t iv[16]) {
    return Encryption_DeriveKeyAndIvWithSeedWidth(input, size, 32, key, iv);
}

EncryptionResult Encryption_DeriveKeyAndIvWithSeedWidth(const void* input, size_t size,
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
        const auto result = encryption::keygen::from_utf16(text, word_bits == 32
            ? encryption::keygen::SeedWidth::bits32 : encryption::keygen::SeedWidth::bits64);
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

// ------------------------------------------
// Callback streams
// ------------------------------------------

EncryptionResult Encryption_EncryptReader(const uint8_t key[16], const uint8_t iv[16],
    EncryptionReadCallback read, void* read_user, EncryptionWriteCallback write, void* write_user) {
    return cipher_reader(encryption::Mode::encrypt, key, iv, read, read_user, write, write_user);
}

EncryptionResult Encryption_DecryptReader(const uint8_t key[16], const uint8_t iv[16],
    EncryptionReadCallback read, void* read_user, EncryptionWriteCallback write, void* write_user) {
    return cipher_reader(encryption::Mode::decrypt, key, iv, read, read_user, write, write_user);
}

// ------------------------------------------
// Memory buffers
// ------------------------------------------

EncryptionResult Encryption_EncryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output == nullptr || output_length == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    BufferSink sink{output, output_capacity, 0};
    const auto result = Encryption_EncryptReader(
        key, iv, read_callback, read_user_data, append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) *output_length = sink.size;
    return result;
}

EncryptionResult Encryption_DecryptReaderToBuffer(
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionReadCallback read_callback, void* read_user_data,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output == nullptr || output_length == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    BufferSink sink{output, output_capacity, 0};
    const auto result = Encryption_DecryptReader(
        key, iv, read_callback, read_user_data, append_to_buffer, &sink);
    if (result == ENCRYPTION_OK) *output_length = sink.size;
    return result;
}

EncryptionResult Encryption_EncryptBuffer(
    const uint8_t key[16], const uint8_t iv[16], const uint8_t* input, size_t size,
    uint8_t* output, size_t capacity, size_t* written) {
    return transform_buffer(encryption::Mode::encrypt, key, iv, input, size, output, capacity, written);
}

EncryptionResult Encryption_DecryptBuffer(
    const uint8_t key[16], const uint8_t iv[16], const uint8_t* input, size_t size,
    uint8_t* output, size_t capacity, size_t* written) {
    return transform_buffer(encryption::Mode::decrypt, key, iv, input, size, output, capacity, written);
}

EncryptionResult Encryption_HashBuffer(
    EncryptionHashAlgorithm algorithm,
    const uint8_t* input, size_t input_length,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (output_length == nullptr || (input == nullptr && input_length != 0) ||
        output == nullptr) return ENCRYPTION_INVALID_ARGUMENT;

    EncryptionHashContext* context = nullptr;
    auto result = Encryption_HashCreate(algorithm, &context);
    if (result == ENCRYPTION_OK) result = Encryption_HashUpdate(context, input, input_length);
    if (result == ENCRYPTION_OK) result = Encryption_HashFinal(context, output, output_capacity, output_length);
    Encryption_HashDestroy(context);
    return result;
}

// ------------------------------------------
// FILE handles
// ------------------------------------------

EncryptionResult Encryption_EncryptFileStream(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback, void* write_user_data) {
    if (input_file == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file};
    return Encryption_EncryptReader(key, iv, read_file, &reader,
                                    write_callback, write_user_data);
}

EncryptionResult Encryption_DecryptFileStream(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE],
    EncryptionWriteCallback write_callback, void* write_user_data) {
    if (input_file == nullptr) return ENCRYPTION_INVALID_ARGUMENT;
    FileReader reader{input_file};
    return Encryption_DecryptReader(key, iv, read_file, &reader,
                                    write_callback, write_user_data);
}

EncryptionResult Encryption_EncryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    FileWriter writer{output_file};
    return Encryption_EncryptReader(key, iv, read_file, &reader,
                                    write_file, &writer);
}

EncryptionResult Encryption_DecryptFile(
    FILE* input_file, FILE* output_file,
    const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE]) {
    if (input_file == nullptr || output_file == nullptr || input_file == output_file) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    FileWriter writer{output_file};
    return Encryption_DecryptReader(key, iv, read_file, &reader,
                                    write_file, &writer);
}

EncryptionResult Encryption_EncryptFileToBuffer(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE], uint8_t* output,
    size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    return Encryption_EncryptReaderToBuffer(key, iv, read_file, &reader,
                                             output, output_capacity, output_length);
}

EncryptionResult Encryption_DecryptFileToBuffer(
    FILE* input_file, const uint8_t key[ENCRYPTION_AES_KEY_SIZE],
    const uint8_t iv[ENCRYPTION_AES_IV_SIZE], uint8_t* output,
    size_t output_capacity, size_t* output_length) {
    if (input_file == nullptr) {
        if (output_length != nullptr) *output_length = 0;
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    FileReader reader{input_file};
    return Encryption_DecryptReaderToBuffer(key, iv, read_file, &reader,
                                             output, output_capacity, output_length);
}

EncryptionResult Encryption_HashFile(
    FILE* input_file, EncryptionHashAlgorithm algorithm,
    uint8_t* output, size_t output_capacity, size_t* output_length) {
    if (output_length != nullptr) *output_length = 0;
    if (input_file == nullptr || output == nullptr || output_length == nullptr) {
        return ENCRYPTION_INVALID_ARGUMENT;
    }
    constexpr size_t capacity = 64 * 1024;
    std::unique_ptr<uint8_t[]> buffer(new (std::nothrow) uint8_t[capacity]);
    if (!buffer) return ENCRYPTION_OUT_OF_MEMORY;

    EncryptionHashContext* context = nullptr;
    auto result = Encryption_HashCreate(algorithm, &context);
    if (result != ENCRYPTION_OK) return result;

    FileReader reader{input_file};
    for (;;) {
        size_t bytes_read = 0;
        result = read_file(&reader, buffer.get(), capacity, &bytes_read);
        if (result != ENCRYPTION_OK || bytes_read == 0) break;
        result = Encryption_HashUpdate(context, buffer.get(), bytes_read);
        if (result != ENCRYPTION_OK) break;
    }
    if (result == ENCRYPTION_OK) {
        result = Encryption_HashFinal(context, output, output_capacity, output_length);
    }

    Encryption_HashDestroy(context);
    return result;
}

} // extern "C"
