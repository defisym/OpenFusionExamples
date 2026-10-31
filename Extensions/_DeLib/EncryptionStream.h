#pragma once
#include "Encryption.h"

// Historical push API. New code should use encryption::Cipher/DecryptReader.
class EncryptionStreamHelper {
    encryption::KeyIv material_;
    bool valid_ = true;
public:
    explicit EncryptionStreamHelper(const wchar_t* text,
        LPCWSTR algorithm = BCRYPT_AES_ALGORITHM)
        : material_(encryption::legacy::derive(encryption::legacy::utf16(text ? text : L""),
              encryption::legacy::native_word_size)),
          valid_(algorithm && std::wcscmp(algorithm, BCRYPT_AES_ALGORITHM) == 0) {}
    EncryptionStreamHelper(const EncryptionStreamHelper&) = default;
    EncryptionStreamHelper& operator=(const EncryptionStreamHelper&) = default;
    size_t Stream(PBYTE input, size_t size, PBYTE output, size_t capacity,
        encryption::Mode mode, bool padding) {
        if (!valid_ || !output || (!input && size)) return 0;
        if (size > (std::numeric_limits<size_t>::max)() - 16 ||
            capacity < (mode == encryption::Mode::encrypt && padding ? size / 16 * 16 + 16 : size)) return 0;
        try {
            // Capture chaining input before writing (in-place decryption is allowed).
            auto iv = material_.iv;
            if (mode == encryption::Mode::decrypt && size >= 16)
                std::copy_n(input + size - 16, 16, iv.begin());
            const auto result = padding
                ? encryption::transform_buffer(material_, mode, input, size)
                : encryption::transform_blocks(material_, mode, input, size);
            if (!result.empty()) std::memcpy(output, result.data(), result.size());
            if (mode == encryption::Mode::encrypt && result.size() >= 16)
                std::copy_n(result.end() - 16, 16, iv.begin());
            material_.iv = iv;
            if (padding) valid_ = false;
            return result.size();
        } catch (const encryption::Error&) { valid_ = false; return 0; }
    }
#ifdef _WIN32
    using HandlerType = decltype(BCryptEncrypt);
    size_t Stream(PBYTE input, size_t size, PBYTE output, size_t capacity,
        HandlerType handler, bool padding) {
        // These function pointers are legacy mode tags; they are never called.
        if (handler != BCryptEncrypt && handler != BCryptDecrypt) return 0;
        return Stream(input, size, output, capacity,
            handler == BCryptEncrypt ? encryption::Mode::encrypt : encryption::Mode::decrypt, padding);
    }
#endif
};
