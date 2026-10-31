#pragma once
// Source-compatible Fusion facade; every operation delegates to the new core.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h> // Old algorithm-name constants only, no BCrypt implementation.
#else
#include <cstdint>
using BYTE = std::uint8_t;
using DWORD = std::uint32_t;
using PBYTE = BYTE*;
using LPBYTE = BYTE*;
using LPCWSTR = const wchar_t*;
using LPWSTR = wchar_t*;
#define BCRYPT_AES_ALGORITHM L"AES"
#define BCRYPT_MD5_ALGORITHM L"MD5"
#define BCRYPT_SHA1_ALGORITHM L"SHA1"
#define BCRYPT_SHA256_ALGORITHM L"SHA256"
#define BCRYPT_SHA384_ALGORITHM L"SHA384"
#define BCRYPT_SHA512_ALGORITHM L"SHA512"
#endif
#include "EncryptionLegacy.h"
#include <string>
#include <tuple>
#include <cwchar>
#undef EncryptFile
#undef DecryptFile
inline constexpr bool ENCRY = true, DECRY = false;
inline constexpr int KEY_LENGTH = 16, STR_LENGTH = 8;
inline constexpr DWORD BUFFER_SIZE = 1024 * 256;
inline constexpr BYTE DefaultKey[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
inline constexpr BYTE DefaultIV[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
#define PROCESS_DIRECTLY

class Encryption {
    encryption::Bytes input_, output_;
    std::string input_string_, output_string_;
    std::wstring hash_string_;
    encryption::KeyIv material_ = encryption::legacy::derive(u"");
    DWORD buffer_size_ = BUFFER_SIZE;
    bool key_generated_ = false;
    static void store(encryption::Bytes& destination, const BYTE* data, std::size_t size);
    static PBYTE data(const encryption::Bytes& buffer) noexcept;
    static DWORD length(const encryption::Bytes& buffer) noexcept;
    bool process(encryption::Mode mode, LPCWSTR algorithm);
    bool process_file(const wchar_t* path, encryption::Mode mode, LPCWSTR algorithm);
public:
    Encryption() = default;
    ~Encryption() = default;
    Encryption(const Encryption&) = default;
    Encryption& operator=(const Encryption&) = default;
    Encryption(Encryption&&) noexcept = default;
    Encryption& operator=(Encryption&&) noexcept = default;
    auto GetKeyInfo() {
        return std::make_tuple(key_generated_ ? material_.key.data() : nullptr, DWORD{16},
            key_generated_ ? material_.iv.data() : nullptr, DWORD{16});
    }
    bool OpenFile(const wchar_t* file);
    bool SaveFile(const wchar_t* file, bool SaveSrc = false) const;
    static bool SaveFile(const wchar_t* file, PBYTE buffer, DWORD size);
    PBYTE GetData() const { return output_.empty() ? GetInputData() : GetOutputData(); }
    DWORD GetDataLength() const { return output_.empty() ? GetInputDataLength() : GetOutputDataLength(); }
    PBYTE GetInputData() const { return data(input_); }
    DWORD GetInputDataLength() const { return length(input_); }
    PBYTE GetOutputData() const { return data(output_); }
    DWORD GetOutputDataLength() const { return length(output_); }
    template<class T> void SetEncryptData(const T* buffer, DWORD count) {
        encryption::require(count <= (std::numeric_limits<DWORD>::max)() / sizeof(T),
            encryption::ErrorCode::argument, "Legacy input exceeds DWORD size");
        store(input_, reinterpret_cast<const BYTE*>(buffer), std::size_t(count) * sizeof(T));
        output_.clear(); hash_string_.clear();
    }
    void SetEncryptStr(const std::string& text);
    void SetEncryptStr(const char* text, DWORD size);
    void SetEncryptStr(const std::wstring& text);
    void SetEncryptStr(const wchar_t* text, DWORD size);
    char* GetInputStr();
    DWORD GetInputStrLength() const { return GetInputDataLength() + 1; }
    void ReleaseInputStr() { input_string_.clear(); }
    void ReleaseInputData() { input_.clear(); input_string_.clear(); }
    char* GetOutputStr();
    DWORD GetOutputStrLength() const { return GetOutputDataLength() + 1; }
    void ReleaseOutputStr() { output_string_.clear(); }
    void GenerateKey(const wchar_t* text);
    bool EncryptFile(const wchar_t* file, LPCWSTR algorithm = BCRYPT_AES_ALGORITHM);
    bool DecryptFile(const wchar_t* file, LPCWSTR algorithm = BCRYPT_AES_ALGORITHM);
    bool Encrypt(LPCWSTR algorithm = BCRYPT_AES_ALGORITHM) { return process(encryption::Mode::encrypt, algorithm); }
    bool Decrypt(LPCWSTR algorithm = BCRYPT_AES_ALGORITHM) { return process(encryption::Mode::decrypt, algorithm); }
    void SetBufferSize(DWORD size = BUFFER_SIZE) { buffer_size_ = (std::max)(DWORD{1}, size); }
    bool EncryptFileDirectly(const wchar_t* file, LPCWSTR algorithm = BCRYPT_AES_ALGORITHM) {
        return process_file(file, encryption::Mode::encrypt, algorithm);
    }
    bool DecryptFileDirectly(const wchar_t* file, LPCWSTR algorithm = BCRYPT_AES_ALGORITHM) {
        return process_file(file, encryption::Mode::decrypt, algorithm);
    }
    LPCWSTR GetHash(LPCWSTR algorithm = BCRYPT_MD5_ALGORITHM);
#ifdef _DEBUG
    bool CompareGeneratedKey(const wchar_t* left, const wchar_t* right) {
        const auto a = encryption::legacy::derive(encryption::legacy::utf16(left ? left : L""), encryption::legacy::native_word_size);
        const auto b = encryption::legacy::derive(encryption::legacy::utf16(right ? right : L""), encryption::legacy::native_word_size);
        return a.key == b.key && a.iv == b.iv;
    }
#endif
};
inline std::wstring GetFileHash(LPCWSTR file) {
    if (!file || !*file) return L"";
    Encryption hash;
    if (!hash.OpenFile(file)) return L"";
    const auto result = hash.GetHash();
    return result ? result : L"";
}
inline std::wstring GetFileHash(const std::wstring& file) { return GetFileHash(file.c_str()); }
inline std::wstring GetFileHash(LPBYTE bytes, DWORD size) {
    Encryption hash;
    hash.SetEncryptStr(reinterpret_cast<char*>(bytes), size);
    const auto result = hash.GetHash();
    return result ? result : L"";
}
