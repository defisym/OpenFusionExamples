#pragma once

// Source-compatible Fusion facade; every operation delegates to the new core.

#include "EncryptionKeyGen.h"

#include <cstdint>
#include <string>
#include <tuple>
#include <cwchar>

// Windows SDK EFS aliases must not rename the compatibility methods.
#undef EncryptFile
#undef DecryptFile

inline constexpr std::uint32_t BUFFER_SIZE = 1024 * 256;

class Encryption {
    encryption::Bytes input_, output_;
    std::string input_string_, output_string_;
    std::wstring hash_string_;
    encryption::KeyIv material_ = encryption::keygen::from_utf16(u"");
    std::uint32_t buffer_size_ = BUFFER_SIZE;

    static bool store(encryption::Bytes& destination, const std::uint8_t* data, std::size_t size);
    static std::uint8_t* data(const encryption::Bytes& buffer) noexcept;
    static std::uint32_t length(const encryption::Bytes& buffer) noexcept;

    bool process(encryption::Mode mode, const wchar_t* algorithm);
    bool process_file(const wchar_t* path, encryption::Mode mode, const wchar_t* algorithm);

public:

    Encryption() = default;
    ~Encryption() = default;
    Encryption(const Encryption&) = default;
    Encryption& operator=(const Encryption&) = default;
    Encryption(Encryption&&) noexcept = default;
    Encryption& operator=(Encryption&&) noexcept = default;

    auto GetKeyInfo() {
        return std::make_tuple(material_.key.data(), std::uint32_t{16},
            material_.iv.data(), std::uint32_t{16});
    }

    bool OpenFile(const wchar_t* file);
    bool SaveFile(const wchar_t* file, bool SaveSrc = false) const;
    static bool SaveFile(const wchar_t* file, std::uint8_t* buffer, std::uint32_t size);

    std::uint8_t* GetData() const { return output_.empty() ? GetInputData() : GetOutputData(); }
    std::uint32_t GetDataLength() const { return output_.empty() ? GetInputDataLength() : GetOutputDataLength(); }

    std::uint8_t* GetInputData() const { return data(input_); }
    std::uint32_t GetInputDataLength() const { return length(input_); }

    std::uint8_t* GetOutputData() const { return data(output_); }
    std::uint32_t GetOutputDataLength() const { return length(output_); }

    // Reset data and cached results, retaining Key/IV and buffer settings.
    void ClearEncryptData() noexcept;

    template<class T> void SetEncryptData(const T* buffer, std::uint32_t count) {
        if (count > (std::numeric_limits<std::size_t>::max)() / sizeof(T)) {
            ClearEncryptData();
            return;
        }
        if (!store(input_, reinterpret_cast<const std::uint8_t*>(buffer), std::size_t(count) * sizeof(T))) {
            ClearEncryptData();
            return;
        }

        output_.clear(); hash_string_.clear();
    }

    void SetEncryptStr(const std::string& text);
    void SetEncryptStr(const char* text, std::uint32_t size);
    void SetEncryptStr(const std::wstring& text);
    void SetEncryptStr(const wchar_t* text, std::uint32_t size);

    char* GetInputStr();
    std::uint32_t GetInputStrLength() const { return GetInputDataLength() + 1; }
    void ReleaseInputStr() { input_string_.clear(); }
    void ReleaseInputData() { input_.clear(); input_string_.clear(); }

    char* GetOutputStr();
    std::uint32_t GetOutputStrLength() const { return GetOutputDataLength() + 1; }
    void ReleaseOutputStr() { output_string_.clear(); }

    void GenerateKey(const wchar_t* text);
    bool EncryptFile(const wchar_t* file, const wchar_t* algorithm = L"AES");
    bool DecryptFile(const wchar_t* file, const wchar_t* algorithm = L"AES");
    bool Encrypt(const wchar_t* algorithm = L"AES") { return process(encryption::Mode::encrypt, algorithm); }
    bool Decrypt(const wchar_t* algorithm = L"AES") { return process(encryption::Mode::decrypt, algorithm); }

    void SetBufferSize(std::uint32_t size = BUFFER_SIZE) { buffer_size_ = (std::max)(std::uint32_t{1}, size); }

    bool EncryptFileDirectly(const wchar_t* file, const wchar_t* algorithm = L"AES") {
        return process_file(file, encryption::Mode::encrypt, algorithm);
    }

    bool DecryptFileDirectly(const wchar_t* file, const wchar_t* algorithm = L"AES") {
        return process_file(file, encryption::Mode::decrypt, algorithm);
    }

    const wchar_t* GetHash(const wchar_t* algorithm = L"MD5");

#ifdef _DEBUG
    bool CompareGeneratedKey(const wchar_t* left, const wchar_t* right) {
        const auto a = encryption::keygen::from_utf16(encryption::text::utf16(left ? left : L""), encryption::keygen::native_seed_width);
        const auto b = encryption::keygen::from_utf16(encryption::text::utf16(right ? right : L""), encryption::keygen::native_seed_width);
        return a.key == b.key && a.iv == b.iv;
    }
#endif
};

inline std::wstring GetFileHash(const wchar_t* file) {
    if (!file || !*file) return L"";
    Encryption hash;
    if (!hash.OpenFile(file)) return L"";
    const auto result = hash.GetHash();
    return result ? result : L"";
}

inline std::wstring GetFileHash(const std::wstring& file) { return GetFileHash(file.c_str()); }

inline std::wstring GetFileHash(std::uint8_t* bytes, std::uint32_t size) {
    Encryption hash;
    hash.SetEncryptStr(reinterpret_cast<char*>(bytes), size);
    const auto result = hash.GetHash();
    return result ? result : L"";
}
