#include "Encryption.h"

#include <optional>

namespace {

std::optional<encryption::Digest> digest(const wchar_t* name) {
    if (!name) return std::nullopt;

    const std::wstring_view value(name);
    if (value == L"MD5") return encryption::Digest::md5;
    if (value == L"SHA1") return encryption::Digest::sha1;
    if (value == L"SHA256") return encryption::Digest::sha256;
    if (value == L"SHA384") return encryption::Digest::sha384;
    if (value == L"SHA512") return encryption::Digest::sha512;
    return std::nullopt;
}

}

bool Encryption::store(encryption::Bytes& destination, const std::uint8_t* buffer, std::size_t size) {
    // The compatibility API exposes 32-bit lengths, including a string terminator.
    if ((!buffer && size) || size >= (std::numeric_limits<std::uint32_t>::max)()) {
        destination.clear();
        return false;
    }

    encryption::Bytes next(size + 1, 0);
    if (size) std::memcpy(next.data(), buffer, size);
    destination = std::move(next);
    return true;
}

std::uint8_t* Encryption::data(const encryption::Bytes& buffer) noexcept {
    return buffer.empty() ? nullptr : const_cast<std::uint8_t*>(buffer.data());
}

std::uint32_t Encryption::length(const encryption::Bytes& buffer) noexcept {
    return buffer.empty() ? 0 : static_cast<std::uint32_t>(buffer.size() - 1);
}

void Encryption::SetEncryptStr(const std::string& text) {
    store(input_, reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
    output_.clear(); hash_string_.clear();
}

void Encryption::SetEncryptStr(const char* text, std::uint32_t size) { SetEncryptData(text, size); }

void Encryption::SetEncryptStr(const std::wstring& text) {
    const auto bytes = encryption::text::utf16_bytes(text);
    store(input_, bytes.data(), bytes.size());
    output_.clear(); hash_string_.clear();
}

void Encryption::SetEncryptStr(const wchar_t* text, std::uint32_t size) {
    if (!text) {
        SetEncryptData(text, size);
        return;
    }

    SetEncryptStr(std::wstring(text, size));
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
    material_ = encryption::keygen::from_utf16(encryption::text::utf16(text ? text : L""),
        encryption::keygen::native_seed_width);
}

bool Encryption::OpenFile(const wchar_t* file) {
    input_.clear(); output_.clear(); hash_string_.clear();
    if (!file || !*file) return false;

    try {
        encryption::FileSource source(encryption::text::path(file));
        if (source.size() >= (std::numeric_limits<std::uint32_t>::max)()) return false;

        encryption::Bytes next(static_cast<std::size_t>(source.size()) + 1, 0);
        source.read_exact(0, next.data(), next.size() - 1);
        input_ = std::move(next);
        return true;
    } catch (const encryption::Error&) { return false; }
}

bool Encryption::SaveFile(const wchar_t* file, std::uint8_t* buffer, std::uint32_t size) {
    if (!file || !*file || !buffer) return false;

    try {
        const auto path = encryption::text::path(file);
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

bool Encryption::process(encryption::Mode mode, const wchar_t* algorithm) {
    output_.clear(); hash_string_.clear();
    if (input_.empty() || !algorithm || std::wcscmp(algorithm, L"AES")) return false;

    try {
        const auto bytes = encryption::transform_buffer(material_, mode, input_.data(), length(input_));

        return store(output_, bytes.data(), bytes.size());
    } catch (const encryption::Error&) { return false; }
}

bool Encryption::EncryptFile(const wchar_t* file, const wchar_t* algorithm) {
    const bool result = OpenFile(file) && Encrypt(algorithm);
    ReleaseInputData();
    return result;
}

bool Encryption::DecryptFile(const wchar_t* file, const wchar_t* algorithm) {
    const bool result = OpenFile(file) && Decrypt(algorithm);
    ReleaseInputData();
    return result;
}

bool Encryption::process_file(const wchar_t* file, encryption::Mode mode, const wchar_t* algorithm) {
    input_.clear(); output_.clear(); hash_string_.clear();
    if (!file || !*file || !algorithm || std::wcscmp(algorithm, L"AES")) return false;

    try {
        encryption::FileSource source(encryption::text::path(file));

        encryption::Bytes result;
        std::uint64_t position = 0;

        const auto read = [&](std::uint8_t* buffer, std::size_t capacity) {
            const auto requested = static_cast<std::size_t>((std::min)(std::uint64_t(capacity), std::uint64_t(buffer_size_) * 16));
            const auto count = source.read_at(position, buffer, requested);
            position += count;
            return count;
        };

        encryption::transform(material_, mode, read, [&](const std::uint8_t* bytes, std::size_t size) {
            result.insert(result.end(), bytes, bytes + size);
        });

        return store(output_, result.data(), result.size());
    } catch (const encryption::Error&) { return false; }
}

const wchar_t* Encryption::GetHash(const wchar_t* algorithm) {
    output_.clear(); hash_string_.clear();
    const auto selected = digest(algorithm);
    if (input_.empty() || !selected) return nullptr;

    try {
        encryption::Hash hash(*selected);
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
