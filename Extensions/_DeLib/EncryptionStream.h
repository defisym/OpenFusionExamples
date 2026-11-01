#pragma once

#include "Encryption.h"

// Historical push API. New code should use encryption::Cipher/DecryptReader.
class EncryptionStreamHelper {
    encryption::KeyIv material_;
    bool valid_ = true;

public:

    explicit EncryptionStreamHelper(const wchar_t* text,
        const wchar_t* algorithm = L"AES")
        : material_(encryption::keygen::from_utf16(encryption::text::utf16(text ? text : L""),
              encryption::keygen::native_seed_width)),
          valid_(algorithm && std::wcscmp(algorithm, L"AES") == 0) {}
    EncryptionStreamHelper(const EncryptionStreamHelper&) = default;
    EncryptionStreamHelper& operator=(const EncryptionStreamHelper&) = default;

    size_t Stream(std::uint8_t* input, size_t size, std::uint8_t* output, size_t capacity,
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

};
