#pragma once

// Shared C++17 implementation. Every platform, including Windows, uses OpenSSL.
// Legacy Fusion projects import Encryption.props and keep compiling Encryption.cpp.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <openssl/evp.h>

namespace encryption {

using Byte = std::uint8_t;
using Bytes = std::vector<Byte>;
using Block = std::array<Byte, 16>;
struct KeyIv { Block key; Block iv; };
enum class Mode { decrypt, encrypt };
enum class Digest { md5, sha1, sha256, sha384, sha512 };
enum class ErrorCode { argument, state, crypto, padding, io };
class Error : public std::runtime_error {
public:
    ErrorCode code;
    Error(ErrorCode c, const char* message) : std::runtime_error(message), code(c) {}
};
inline void require(bool ok, ErrorCode code, const char* message) {
    if (!ok) throw Error(code, message);
}

namespace detail {
// Padding, buffering, file I/O and seeking are implemented once above EVP.
class CbcBlocks {
    struct Delete { void operator()(EVP_CIPHER_CTX* p) const { EVP_CIPHER_CTX_free(p); } };
    std::unique_ptr<EVP_CIPHER_CTX, Delete> context_{EVP_CIPHER_CTX_new()};
public:
    CbcBlocks(const KeyIv& material, Mode mode) {
        require(context_ != nullptr, ErrorCode::crypto, "Allocate AES context");
        require(EVP_CipherInit_ex(context_.get(), EVP_aes_128_cbc(), nullptr,
                    material.key.data(), material.iv.data(), mode == Mode::encrypt) == 1 &&
                EVP_CIPHER_CTX_set_padding(context_.get(), 0) == 1,
                ErrorCode::crypto, "Initialize AES");
    }
    CbcBlocks(const CbcBlocks&) = delete;
    CbcBlocks& operator=(const CbcBlocks&) = delete;
    void process(const Byte* input, std::size_t length, Byte* output) {
        require(length != 0 && length % 16 == 0 && length <= 65536,
                ErrorCode::argument, "Invalid AES block batch");
        int written = 0;
        require(EVP_CipherUpdate(context_.get(), output, &written, input,
                    static_cast<int>(length)) == 1 && static_cast<std::size_t>(written) == length,
                ErrorCode::crypto, "AES block operation");
    }
};
} // namespace detail

// Move-only owning cipher. Update accepts arbitrary chunk boundaries; finish
// adds/validates PKCS#7 exactly once. A failed operation poisons the context.
class Cipher {
    std::unique_ptr<detail::CbcBlocks> backend_;
    Mode mode_;
    Bytes pending_;
    bool done_ = false;
public:
    Cipher(const KeyIv& material, Mode mode)
        : backend_(std::make_unique<detail::CbcBlocks>(material, mode)), mode_(mode) {}
    Cipher(Cipher&&) noexcept = default;
    Cipher& operator=(Cipher&&) noexcept = default;
    Cipher(const Cipher&) = delete;
    Cipher& operator=(const Cipher&) = delete;
    Bytes update(const Byte* data, std::size_t size) {
        require(!done_ && backend_, ErrorCode::state, "Cipher is finished or moved");
        require(data != nullptr || size == 0, ErrorCode::argument, "Null cipher input");
        require(size <= (std::numeric_limits<std::size_t>::max)() - 16,
                ErrorCode::argument, "Cipher input too large");
        Bytes output;
        try {
            output.reserve(size + 16);
            while (size != 0) {
                const auto count = (std::min)(size, std::size_t{65536} - pending_.size());
                pending_.insert(pending_.end(), data, data + count);
                data += count; size -= count;
                const auto blocks = mode_ == Mode::decrypt
                    ? (pending_.size() - 1) / 16 : pending_.size() / 16;
                const auto length = blocks * 16;
                if (length) {
                    const auto offset = output.size();
                    output.resize(offset + length);
                    backend_->process(pending_.data(), length, output.data() + offset);
                    pending_.erase(pending_.begin(), pending_.begin() + length);
                }
            }
        } catch (...) { done_ = true; throw; }
        return output;
    }
    Bytes finish() {
        require(!done_ && backend_, ErrorCode::state, "Cipher is finished or moved");
        done_ = true;
        if (mode_ == Mode::encrypt) {
            const auto padding = static_cast<Byte>(16 - pending_.size());
            pending_.resize(16, padding);
        } else {
            require(pending_.size() == 16, ErrorCode::padding, "Truncated ciphertext");
        }
        Bytes output(16);
        backend_->process(pending_.data(), 16, output.data());
        if (mode_ == Mode::decrypt) {
            const auto padding = output.back();
            require(padding >= 1 && padding <= 16, ErrorCode::padding, "Invalid PKCS#7 padding");
            unsigned mismatch = 0;
            for (std::size_t i = 16 - padding; i < 16; ++i) mismatch |= output[i] ^ padding;
            require(mismatch == 0, ErrorCode::padding, "Invalid PKCS#7 padding");
            output.resize(16 - padding);
        }
        pending_.clear();
        return output;
    }
};

class Hash {
    struct Delete { void operator()(EVP_MD_CTX* p) const { EVP_MD_CTX_free(p); } };
    std::unique_ptr<EVP_MD_CTX, Delete> context_{EVP_MD_CTX_new()};
    std::size_t size_ = 0;
    bool done_ = false;
public:
    explicit Hash(Digest digest) {
        const EVP_MD* md = nullptr;
        switch (digest) {
        case Digest::md5: md = EVP_md5(); break;
        case Digest::sha1: md = EVP_sha1(); break;
        case Digest::sha256: md = EVP_sha256(); break;
        case Digest::sha384: md = EVP_sha384(); break;
        case Digest::sha512: md = EVP_sha512(); break;
        }
        require(context_ && md && EVP_DigestInit_ex(context_.get(), md, nullptr) == 1,
                ErrorCode::crypto, "Initialize hash");
        size_ = static_cast<std::size_t>(EVP_MD_size(md));
    }
    Hash(Hash&&) noexcept = default;
    Hash& operator=(Hash&&) noexcept = default;
    Hash(const Hash&) = delete;
    Hash& operator=(const Hash&) = delete;
    std::size_t size() const noexcept { return size_; }
    void update(const Byte* input, std::size_t length) {
        require(!done_ && context_, ErrorCode::state, "Hash is finished or moved");
        require(input || length == 0, ErrorCode::argument, "Null hash input");
        if (!length) return;
        if (EVP_DigestUpdate(context_.get(), input, length) != 1) {
            done_ = true; throw Error(ErrorCode::crypto, "Hash update");
        }
    }
    Bytes finish() {
        require(!done_ && context_, ErrorCode::state, "Hash is finished or moved");
        done_ = true;
        Bytes output(size_);
        unsigned written = 0;
        require(EVP_DigestFinal_ex(context_.get(), output.data(), &written) == 1 && written == size_,
                ErrorCode::crypto, "Hash finish");
        return output;
    }
};

// Reader returns 0 at EOF; writer consumes a complete chunk or throws.
template<class Reader, class Writer>
void transform(const KeyIv& material, Mode mode, Reader&& read, Writer&& write) {
    Cipher cipher(material, mode);
    std::array<Byte, 65536> input{};
    for (;;) {
        const auto count = read(input.data(), input.size());
        require(count <= input.size(), ErrorCode::argument, "Reader exceeded capacity");
        if (!count) break;
        const auto output = cipher.update(input.data(), count);
        if (!output.empty()) write(output.data(), output.size());
    }
    const auto output = cipher.finish();
    if (!output.empty()) write(output.data(), output.size());
}
inline Bytes transform_buffer(const KeyIv& material, Mode mode, const Byte* input, std::size_t size) {
    Cipher cipher(material, mode);
    auto output = cipher.update(input, size);
    const auto tail = cipher.finish();
    output.insert(output.end(), tail.begin(), tail.end());
    return output;
}

// Block-aligned CBC without padding, for the historical stream-helper facade.
inline Bytes transform_blocks(const KeyIv& material, Mode mode, const Byte* input, std::size_t size) {
    require((input || size == 0) && size % 16 == 0, ErrorCode::argument, "Unaligned CBC input");
    detail::CbcBlocks cipher(material, mode);
    Bytes output(size);
    for (std::size_t offset = 0; offset < size;) {
        const auto count = (std::min)(std::size_t{65536}, size - offset);
        cipher.process(input + offset, count, output.data() + offset);
        offset += count;
    }
    return output;
}

// Sources must support concurrent read_at calls if shared between decoders.
class RandomAccessSource {
public:
    virtual ~RandomAccessSource() = default;
    virtual std::uint64_t size() const = 0;
    virtual std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) = 0;
    void read_exact(std::uint64_t offset, Byte* output, std::size_t size) {
        while (size) {
            const auto count = read_at(offset, output, size);
            require(count != 0 && count <= size, ErrorCode::io, "Truncated source");
            offset += count; output += count; size -= count;
        }
    }
};

class FileSource final : public RandomAccessSource {
    struct Close { void operator()(FILE* p) const { std::fclose(p); } };
    std::unique_ptr<FILE, Close> file_;
    std::uint64_t size_ = 0;
    std::mutex mutex_;
    static int seek(FILE* file, std::int64_t offset, int whence) {
#ifdef _WIN32
        return _fseeki64(file, offset, whence);
#else
        static_assert(sizeof(off_t) >= 8, "Build with _FILE_OFFSET_BITS=64");
        return fseeko(file, static_cast<off_t>(offset), whence);
#endif
    }
public:
    explicit FileSource(const std::filesystem::path& path) {
#ifdef _WIN32
        file_.reset(_wfopen(path.c_str(), L"rb"));
#else
        file_.reset(std::fopen(path.c_str(), "rb"));
#endif
        require(file_ != nullptr, ErrorCode::io, "Open asset");
        require(seek(file_.get(), 0, SEEK_END) == 0, ErrorCode::io, "Seek asset");
#ifdef _WIN32
        const auto size = _ftelli64(file_.get());
#else
        const auto size = ftello(file_.get());
#endif
        require(size >= 0, ErrorCode::io, "Get asset size");
        size_ = static_cast<std::uint64_t>(size);
    }
    std::uint64_t size() const override { return size_; }
    std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) override {
        require(output || capacity == 0, ErrorCode::argument, "Null source output");
        require(offset <= size_, ErrorCode::argument, "Source offset beyond EOF");
        const auto count = static_cast<std::size_t>((std::min)(size_ - offset,
                                        static_cast<std::uint64_t>(capacity)));
        if (!count) return 0;
        std::lock_guard<std::mutex> lock(mutex_);
        require(seek(file_.get(), static_cast<std::int64_t>(offset), SEEK_SET) == 0,
                ErrorCode::io, "Seek asset");
        const auto read = std::fread(output, 1, count, file_.get());
        require(!std::ferror(file_.get()), ErrorCode::io, "Read asset");
        return read;
    }
};

// Pull-style, independently seekable CBC view for FFmpeg/other decoders.
// Only the last block is read on construction (padding/size), then a bounded
// 64 KiB window as requested. CBC has no authenticity; padding is not a MAC.
class DecryptReader {
    std::shared_ptr<RandomAccessSource> source_;
    KeyIv material_;
    std::uint64_t size_ = 0, position_ = 0, window_start_ = 0;
    Bytes window_;
    Bytes decrypt_range(std::uint64_t start, std::size_t length, bool final) {
        auto material = material_;
        if (start) source_->read_exact(start - 16, material.iv.data(), 16);
        Bytes encrypted(length), plain(length);
        source_->read_exact(start, encrypted.data(), length);
        detail::CbcBlocks cipher(material, Mode::decrypt);
        cipher.process(encrypted.data(), length, plain.data());
        if (final) {
            const auto padding = plain.back();
            require(padding >= 1 && padding <= 16, ErrorCode::padding, "Invalid asset padding");
            for (std::size_t i = plain.size() - padding; i < plain.size(); ++i)
                require(plain[i] == padding, ErrorCode::padding, "Invalid asset padding");
            plain.resize(plain.size() - padding);
        }
        return plain;
    }
public:
    DecryptReader(std::shared_ptr<RandomAccessSource> source, const KeyIv& material)
        : source_(std::move(source)), material_(material) {
        require(source_ != nullptr, ErrorCode::argument, "Null asset source");
        const auto length = source_->size();
        require(length >= 16 && length % 16 == 0 &&
                length <= static_cast<std::uint64_t>((std::numeric_limits<std::int64_t>::max)()),
                ErrorCode::padding, "Invalid encrypted asset size");
        const auto tail = decrypt_range(length - 16, 16, true);
        size_ = length - 16 + tail.size();
    }
    std::uint64_t size() const noexcept { return size_; }
    std::uint64_t tell() const noexcept { return position_; }
    void seek(std::uint64_t position) {
        require(position <= size_, ErrorCode::argument, "Seek beyond plaintext EOF");
        position_ = position;
    }
    std::unique_ptr<DecryptReader> clone() const {
        return std::make_unique<DecryptReader>(source_, material_);
    }
    std::size_t read(Byte* output, std::size_t capacity) {
        require(output || capacity == 0, ErrorCode::argument, "Null asset output");
        std::size_t total = 0;
        while (capacity && position_ < size_) {
            if (window_.empty() || position_ < window_start_ || position_ - window_start_ >= window_.size()) {
                const auto start = position_ / 16 * 16;
                const auto length = static_cast<std::size_t>((std::min)(
                    std::uint64_t{65536}, source_->size() - start));
                auto window = decrypt_range(start, length, start + length == source_->size());
                window_start_ = start;
                window_ = std::move(window);
            }
            const auto offset = static_cast<std::size_t>(position_ - window_start_);
            const auto count = (std::min)(capacity, window_.size() - offset);
            std::memcpy(output, window_.data() + offset, count);
            output += count; capacity -= count; total += count; position_ += count;
        }
        return total;
    }
};
} // namespace encryption
