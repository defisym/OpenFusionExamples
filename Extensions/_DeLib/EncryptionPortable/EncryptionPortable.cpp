#include "EncryptionPortable.h"

#include <algorithm>
#include <array>
#include <climits>
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace encryption_portable {
namespace {

void set_error(std::string* error, const char* message) {
    if (error != nullptr) {
        *error = message;
    }
}

constexpr std::size_t kIoBufferSize = 64 * 1024;

}  // namespace

bool make_salt(Salt& salt, std::string* error) {
    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1) {
        set_error(error, "OpenSSL random number generation failed");
        return false;
    }
    return true;
}

bool derive_key_and_iv(const std::string& password_utf8,
                       const Salt& salt,
                       Key& key,
                       Iv& iv,
                       std::uint32_t iterations,
                       std::string* error) {
    if (password_utf8.size() > static_cast<std::size_t>(INT_MAX) ||
        iterations == 0 || iterations > static_cast<std::uint32_t>(INT_MAX)) {
        set_error(error, "Invalid password length or PBKDF2 iteration count");
        return false;
    }

    std::array<std::uint8_t, kKeySize + kIvSize> material{};
    if (PKCS5_PBKDF2_HMAC(password_utf8.data(),
                          static_cast<int>(password_utf8.size()),
                          salt.data(),
                          static_cast<int>(salt.size()),
                          static_cast<int>(iterations),
                          EVP_sha256(),
                          static_cast<int>(material.size()),
                          material.data()) != 1) {
        set_error(error, "PBKDF2-HMAC-SHA-256 failed");
        return false;
    }

    std::copy_n(material.begin(), key.size(), key.begin());
    std::copy_n(material.begin() + key.size(), iv.size(), iv.begin());
    OPENSSL_cleanse(material.data(), material.size());
    return true;
}

bool decrypt_cbc_stream(std::istream& ciphertext,
                        std::ostream& plaintext,
                        const Key& key,
                        const Iv& iv,
                        std::string* error) {
    EVP_CIPHER_CTX* raw_context = EVP_CIPHER_CTX_new();
    if (raw_context == nullptr) {
        set_error(error, "Could not allocate OpenSSL cipher context");
        return false;
    }

    const auto free_context = [](EVP_CIPHER_CTX* context) {
        EVP_CIPHER_CTX_free(context);
    };
    const std::unique_ptr<EVP_CIPHER_CTX, decltype(free_context)> context(
        raw_context, free_context);

    if (EVP_DecryptInit_ex(context.get(), EVP_aes_256_cbc(), nullptr,
                           key.data(), iv.data()) != 1) {
        set_error(error, "Could not initialize AES-256-CBC decryption");
        return false;
    }

    std::array<char, kIoBufferSize> input{};
    std::vector<unsigned char> output(kIoBufferSize + EVP_MAX_BLOCK_LENGTH);

    while (ciphertext) {
        ciphertext.read(input.data(), static_cast<std::streamsize>(input.size()));
        const auto count = ciphertext.gcount();
        if (count <= 0) {
            break;
        }

        int produced = 0;
        if (EVP_DecryptUpdate(context.get(),
                              output.data(),
                              &produced,
                              reinterpret_cast<const unsigned char*>(input.data()),
                              static_cast<int>(count)) != 1) {
            set_error(error, "AES-CBC decryption failed");
            return false;
        }
        if (produced > 0) {
            plaintext.write(reinterpret_cast<const char*>(output.data()), produced);
            if (!plaintext) {
                set_error(error, "Writing plaintext failed");
                return false;
            }
        }
    }

    if (ciphertext.bad()) {
        set_error(error, "Reading ciphertext failed");
        return false;
    }

    int final_size = 0;
    if (EVP_DecryptFinal_ex(context.get(), output.data(), &final_size) != 1) {
        set_error(error, "Invalid ciphertext or PKCS#7 padding");
        return false;
    }
    if (final_size > 0) {
        plaintext.write(reinterpret_cast<const char*>(output.data()), final_size);
        if (!plaintext) {
            set_error(error, "Writing final plaintext block failed");
            return false;
        }
    }
    return true;
}

bool sha256_stream(std::istream& input,
                   Sha256& digest,
                   std::string* error) {
    EVP_MD_CTX* raw_context = EVP_MD_CTX_new();
    if (raw_context == nullptr) {
        set_error(error, "Could not allocate OpenSSL digest context");
        return false;
    }

    const auto free_context = [](EVP_MD_CTX* context) {
        EVP_MD_CTX_free(context);
    };
    const std::unique_ptr<EVP_MD_CTX, decltype(free_context)> context(
        raw_context, free_context);

    if (EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr) != 1) {
        set_error(error, "Could not initialize SHA-256");
        return false;
    }

    std::array<char, kIoBufferSize> buffer{};
    while (input) {
        input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const auto count = input.gcount();
        if (count > 0 &&
            EVP_DigestUpdate(context.get(), buffer.data(),
                             static_cast<std::size_t>(count)) != 1) {
            set_error(error, "SHA-256 update failed");
            return false;
        }
    }
    if (input.bad()) {
        set_error(error, "Reading input for SHA-256 failed");
        return false;
    }

    unsigned int digest_size = 0;
    if (EVP_DigestFinal_ex(context.get(), digest.data(), &digest_size) != 1 ||
        digest_size != digest.size()) {
        set_error(error, "Finalizing SHA-256 failed");
        return false;
    }
    return true;
}

}  // namespace encryption_portable
