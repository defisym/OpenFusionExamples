#ifndef ENCRYPTION_PORTABLE_HPP
#define ENCRYPTION_PORTABLE_HPP

#include "EncryptionPortableConvenience.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace encryption_portable {

using Key = std::array<std::uint8_t, ENCRYPTION_AES_KEY_SIZE>;
using Iv = std::array<std::uint8_t, ENCRYPTION_AES_IV_SIZE>;

// Small C++ convenience wrapper over the same C callback-based decryptor.
// encrypted_size must be the size of the complete AES-CBC ciphertext.
class DecryptReaderContext final {
public:
    DecryptReaderContext(const Key& key, const Iv& iv, std::size_t encrypted_size)
        : key_(key),
          iv_(iv),
          encrypted_size_(encrypted_size),
          valid_(encrypted_size >= ENCRYPTION_BLOCK_SIZE &&
                 encrypted_size % ENCRYPTION_BLOCK_SIZE == 0) {}

    bool IsValid() const noexcept { return valid_; }

    // Returns a safe allocation capacity, not the exact plaintext size.
    // PKCS#7 padding makes the exact size known only after decrypting the tail.
    std::size_t GetDecryptedBufferSize() const noexcept {
        return valid_ ? encrypted_size_ : 0;
    }

    EncryptionResult DecryptToCallback(
        EncryptionReadCallback read_callback,
        void* read_user_data,
        EncryptionWriteCallback write_callback,
        void* write_user_data) const {
        if (!valid_) {
            return ENCRYPTION_INVALID_ARGUMENT;
        }
        return Encryption_DecryptReader(
            key_.data(), iv_.data(), read_callback, read_user_data,
            write_callback, write_user_data);
    }

    EncryptionResult DecryptToBuffer(
        EncryptionReadCallback read_callback,
        void* read_user_data,
        std::uint8_t* output,
        std::size_t output_capacity,
        std::size_t* output_length) const {
        if (!valid_) {
            if (output_length != nullptr) {
                *output_length = 0;
            }
            return ENCRYPTION_INVALID_ARGUMENT;
        }
        if (output_capacity < GetDecryptedBufferSize()) {
            if (output_length != nullptr) {
                *output_length = 0;
            }
            return ENCRYPTION_BUFFER_TOO_SMALL;
        }
        return Encryption_DecryptReaderToBuffer(
            key_.data(), iv_.data(), read_callback, read_user_data,
            output, output_capacity, output_length);
    }

private:
    Key key_;
    Iv iv_;
    std::size_t encrypted_size_;
    bool valid_;
};

}  // namespace encryption_portable

#endif  // ENCRYPTION_PORTABLE_HPP
