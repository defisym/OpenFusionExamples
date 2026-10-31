#include "Encryption.h"

namespace {
encryption::Digest digest(LPCWSTR name) {
    encryption::require(name != nullptr, encryption::ErrorCode::argument, "Null digest");
    const std::wstring_view value(name);
    if (value == L"MD5") return encryption::Digest::md5;
    if (value == L"SHA1") return encryption::Digest::sha1;
    if (value == L"SHA256") return encryption::Digest::sha256;
    if (value == L"SHA384") return encryption::Digest::sha384;
    if (value == L"SHA512") return encryption::Digest::sha512;
    throw encryption::Error(encryption::ErrorCode::argument, "Unsupported legacy digest");
}
}
void Encryption::store(encryption::Bytes& destination, const BYTE* buffer, std::size_t size) {
    encryption::require(buffer || size == 0, encryption::ErrorCode::argument, "Null legacy input");
    encryption::require(size < (std::numeric_limits<DWORD>::max)(),
        encryption::ErrorCode::argument, "Legacy buffer exceeds DWORD size");
    encryption::Bytes next(size + 1, 0);
    if (size) std::memcpy(next.data(), buffer, size);
    destination = std::move(next);
}
PBYTE Encryption::data(const encryption::Bytes& buffer) noexcept {
    return buffer.empty() ? nullptr : const_cast<PBYTE>(buffer.data());
}
DWORD Encryption::length(const encryption::Bytes& buffer) noexcept {
    return buffer.empty() ? 0 : static_cast<DWORD>(buffer.size() - 1);
}
void Encryption::SetEncryptStr(const std::string& text) {
    store(input_, reinterpret_cast<const BYTE*>(text.data()), text.size());
    output_.clear(); hash_string_.clear();
}
void Encryption::SetEncryptStr(const char* text, DWORD size) { SetEncryptData(text, size); }
void Encryption::SetEncryptStr(const std::wstring& text) {
    const auto bytes = encryption::legacy::utf16_bytes(text);
    store(input_, bytes.data(), bytes.size());
    output_.clear(); hash_string_.clear();
}
void Encryption::SetEncryptStr(const wchar_t* text, DWORD size) {
    encryption::require(text || size == 0, encryption::ErrorCode::argument, "Null wide input");
    SetEncryptStr(text ? std::wstring(text, size) : std::wstring());
}
char* Encryption::GetInputStr() {
    if (input_.empty()) return nullptr;
    input_string_.assign(reinterpret_cast<const char*>(input_.data()), length(input_));
    return input_string_.data();
}
char* Encryption::GetOutputStr() {
    if (output_.empty()) return nullptr;
    output_string_.assign(reinterpret_cast<const char*>(output_.data()), length(output_));
    return output_string_.data();
}
void Encryption::GenerateKey(const wchar_t* text) {
    material_ = encryption::legacy::derive(encryption::legacy::utf16(text ? text : L""),
        encryption::legacy::native_word_size);
    key_generated_ = true;
}
bool Encryption::OpenFile(const wchar_t* file) {
    input_.clear(); output_.clear(); hash_string_.clear();
    try {
        encryption::FileSource source(encryption::legacy::path(file));
        encryption::require(source.size() < (std::numeric_limits<DWORD>::max)(),
            encryption::ErrorCode::argument, "Legacy file exceeds DWORD size");
        encryption::Bytes next(static_cast<std::size_t>(source.size()) + 1, 0);
        source.read_exact(0, next.data(), next.size() - 1);
        input_ = std::move(next);
        return true;
    } catch (const encryption::Error&) { return false; }
}
bool Encryption::SaveFile(const wchar_t* file, PBYTE buffer, DWORD size) {
    if (!buffer) return false;
    try {
        const auto path = encryption::legacy::path(file);
#ifdef _WIN32
        FILE* output = _wfopen(path.c_str(), L"wb");
#else
        FILE* output = std::fopen(path.c_str(), "wb");
#endif
        if (!output) return false;
        const bool written = std::fwrite(buffer, 1, size, output) == size;
        const bool closed = std::fclose(output) == 0;
        return written && closed;
    } catch (const encryption::Error&) { return false; }
}
bool Encryption::SaveFile(const wchar_t* file, bool source) const {
    return source ? SaveFile(file, GetInputData(), GetInputDataLength())
                  : SaveFile(file, GetOutputData(), GetOutputDataLength());
}
bool Encryption::process(encryption::Mode mode, LPCWSTR algorithm) {
    output_.clear(); hash_string_.clear();
    if (input_.empty() || !algorithm || std::wcscmp(algorithm, BCRYPT_AES_ALGORITHM)) return false;
    try {
        if (!key_generated_) GenerateKey(L"");
        const auto bytes = encryption::transform_buffer(material_, mode, input_.data(), length(input_));
        store(output_, bytes.data(), bytes.size());
        return true;
    } catch (const encryption::Error&) { return false; }
}
bool Encryption::EncryptFile(const wchar_t* file, LPCWSTR algorithm) {
    const bool result = OpenFile(file) && Encrypt(algorithm);
    ReleaseInputData();
    return result;
}
bool Encryption::DecryptFile(const wchar_t* file, LPCWSTR algorithm) {
    const bool result = OpenFile(file) && Decrypt(algorithm);
    ReleaseInputData();
    return result;
}
bool Encryption::process_file(const wchar_t* file, encryption::Mode mode, LPCWSTR algorithm) {
    input_.clear(); output_.clear(); hash_string_.clear();
    if (!algorithm || std::wcscmp(algorithm, BCRYPT_AES_ALGORITHM)) return false;
    try {
        if (!key_generated_) GenerateKey(L"");
        encryption::FileSource source(encryption::legacy::path(file));
        encryption::require(source.size() < (std::numeric_limits<DWORD>::max)() - 16,
            encryption::ErrorCode::argument, "Legacy file exceeds DWORD output size");
        encryption::Bytes result;
        std::uint64_t position = 0;
        const auto read = [&](BYTE* buffer, std::size_t capacity) {
            const auto requested = static_cast<std::size_t>((std::min)(std::uint64_t(capacity), std::uint64_t(buffer_size_) * 16));
            const auto count = source.read_at(position, buffer, requested);
            position += count;
            return count;
        };
        encryption::transform(material_, mode, read, [&](const BYTE* bytes, std::size_t size) {
            result.insert(result.end(), bytes, bytes + size);
        });
        store(output_, result.data(), result.size());
        return true;
    } catch (const encryption::Error&) { return false; }
}
LPCWSTR Encryption::GetHash(LPCWSTR algorithm) {
    output_.clear(); hash_string_.clear();
    if (input_.empty()) return nullptr;
    try {
        encryption::Hash hash(digest(algorithm));
        hash.update(input_.data(), length(input_));
        const auto bytes = hash.finish();
        store(output_, bytes.data(), bytes.size());
        constexpr wchar_t hex[] = L"0123456789abcdef";
        for (const auto byte : bytes) {
            hash_string_.push_back(hex[byte >> 4]);
            hash_string_.push_back(hex[byte & 15]);
        }
        return hash_string_.c_str();
    } catch (const encryption::Error&) { return nullptr; }
}
