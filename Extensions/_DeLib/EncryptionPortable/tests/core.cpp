#include "Encryption.h"
#include "EncryptionStream.h"
#include "EncryptionPortable.h"
#include <chrono>
#include <iostream>
#include <random>

#define CHECK(condition) do { if (!(condition)) throw std::runtime_error(#condition); } while (false)
extern "C" int test_c_api(void);
using namespace encryption;
Bytes hex(const std::string& text) {
    Bytes result;
    for (std::size_t i = 0; i < text.size(); i += 2)
        result.push_back(static_cast<Byte>(std::stoul(text.substr(i, 2), nullptr, 16)));
    return result;
}
Block block(const std::string& text) {
    Block result{};
    const auto bytes = hex(text);
    CHECK(bytes.size() == 16);
    std::copy(bytes.begin(), bytes.end(), result.begin());
    return result;
}
template<class Call> void fails(Call&& call, ErrorCode code) {
    try { call(); } catch (const Error& e) { CHECK(e.code == code); return; }
    throw std::runtime_error("Expected encryption::Error");
}
void known_answers() {
    // NIST SP 800-38A F.2.1, independent of round-trip tests.
    const KeyIv material{block("2b7e151628aed2a6abf7158809cf4f3c"), block("000102030405060708090a0b0c0d0e0f")};
    const auto plain = hex("6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e51"
                           "30c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710");
    const auto encrypted = hex("7649abac8119b246cee98e9b12e9197d5086cb9b507219ee95db113a917678b2"
                               "73bed6b8e3c1743b7116e69e222295163ff1caa1681fac09120eca307586e1a7");
    CHECK(transform_blocks(material, Mode::encrypt, plain.data(), plain.size()) == encrypted);
    CHECK(transform_blocks(material, Mode::decrypt, encrypted.data(), encrypted.size()) == plain);
    Hash hash(Digest::sha256);
    hash.update(reinterpret_cast<const Byte*>("abc"), 3);
    CHECK(hash.finish() == hex("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
    fails([&] { hash.finish(); }, ErrorCode::state);

    // Frozen outputs of the original Windows GenerateKey algorithm.
    struct Vector { const char16_t* text; const char* key; const char* iv32; const char* iv64; };
    const Vector vectors[] = {
        {u"abc", "6100620063002d0026009b00f900c100", "e7005c0096001d003900e0005000ad00", "e7005c0096001d00b9000000ab006c00"},
        {u"12345678", "31003200330034003500360037003800", "e1004b006b009e0058009100ef00d400", "e1004b006b009e00d800700051006800"},
        {u"123456789012345", "31003200330034003500360037003800", "39003000310032003300340035001400", "3900300031003200330034003500be00"},
        {u"1234567890123456", "31003200330034003500360037003800", "39003000310032003300340035003600", "39003000310032003300340035003600"},
        {u"12345678901234567", "06000500040003000200010000000f00", "0e000700060005000400030002000100", "0e000700060005000400030002000100"},
        {u"\u5bc6\u94a5\U0001f600", "c65ba5943dd800de4100a1000200e400", "af00850042002f00d50011008a001900", "af00850042002f0055003100b2001a00"}
    };
    for (const auto& v : vectors) {
        const auto a = legacy::derive(v.text, legacy::WordSize::win32);
        const auto b = legacy::derive(v.text, legacy::WordSize::win64);
        CHECK(a.key == block(v.key) && a.iv == block(v.iv32));
        CHECK(b.key == block(v.key) && b.iv == block(v.iv64));
    }
}
class MemorySource final : public RandomAccessSource {
public:
    Bytes bytes;
    std::uint64_t transferred = 0;
    std::size_t max_read = 65536;
    explicit MemorySource(Bytes value) : bytes(std::move(value)) {}
    std::uint64_t size() const override { return bytes.size(); }
    std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) override {
        CHECK(offset <= bytes.size());
        const auto count = (std::min)({capacity, max_read, bytes.size() - static_cast<std::size_t>(offset)});
        if (count) std::memcpy(output, bytes.data() + offset, count);
        transferred += count;
        return count;
    }
};
void streaming_and_seeking() {
    const auto key = legacy::derive(u"abc");
    std::mt19937 random(42);
    for (const std::size_t size : {0u,1u,15u,16u,17u,65535u,65536u,65537u,200003u}) {
        Bytes input(size);
        for (auto& byte : input) byte = static_cast<Byte>(random());
        const auto expected = transform_buffer(key, Mode::encrypt, input.data(), input.size());
        for (const std::size_t chunk : {1u,15u,16u,17u,65536u}) {
            Cipher cipher(key, Mode::encrypt);
            Bytes output;
            for (std::size_t i = 0; i < size;) {
                const auto n = (std::min)(chunk, size - i);
                const auto bytes = cipher.update(input.data() + i, n);
                output.insert(output.end(), bytes.begin(), bytes.end()); i += n;
            }
            const auto tail = cipher.finish(); output.insert(output.end(), tail.begin(), tail.end());
            CHECK(output == expected);
            fails([&] { cipher.update(nullptr, 0); }, ErrorCode::state);
            fails([&] { cipher.finish(); }, ErrorCode::state);
            Cipher decrypt(key, Mode::decrypt);
            Bytes plain;
            for (std::size_t i = 0; i < output.size();) {
                const auto n = (std::min)(chunk, output.size() - i);
                const auto bytes = decrypt.update(output.data() + i, n);
                plain.insert(plain.end(), bytes.begin(), bytes.end()); i += n;
            }
            const auto last = decrypt.finish(); plain.insert(plain.end(), last.begin(), last.end());
            CHECK(plain == input);
        }
        auto source = std::make_shared<MemorySource>(expected);
        source->max_read = 7; // Exercise read_exact with short source reads.
        DecryptReader reader(source, key);
        CHECK(reader.size() == size && source->transferred <= 32);
        auto second = reader.clone();
        for (int i = 0; i < 25; ++i) {
            const auto position = size ? random() % (size + 1) : 0;
            reader.seek(position);
            std::array<Byte, 4097> buffer{};
            const auto n = reader.read(buffer.data(), buffer.size());
            CHECK(n == (std::min)(buffer.size(), size - position));
            CHECK(std::equal(buffer.begin(), buffer.begin() + n, input.begin() + position));
            CHECK(second->tell() == 0);
        }
        reader.seek(size);
        Byte byte = 0;
        CHECK(reader.read(&byte, 1) == 0);
        fails([&] { reader.seek(size + 1); }, ErrorCode::argument);
        auto truncated = expected; truncated.pop_back();
        fails([&] { transform_buffer(key, Mode::decrypt, truncated.data(), truncated.size()); }, ErrorCode::padding);
        auto bad_padding = expected;
        // Alter the byte in the preceding CBC block/IV controlling the final pad byte.
        auto bad_key = key;
        if (bad_padding.size() == 16) bad_key.iv.back() ^= 16;
        else bad_padding[bad_padding.size() - 17] ^= 16;
        fails([&] { DecryptReader bad(std::make_shared<MemorySource>(bad_padding), bad_key); }, ErrorCode::padding);
    }
    auto source = std::make_shared<MemorySource>(transform_buffer(key, Mode::encrypt, Bytes(8*1024*1024).data(), 8*1024*1024));
    DecryptReader reader(source, key);
    Byte first{}; reader.read(&first, 1);
    CHECK(source->transferred <= 65568); // First read must not decrypt the whole asset.
    auto failing_source = std::make_shared<MemorySource>(source->bytes);
    DecryptReader failing_reader(failing_source, key);
    failing_source->max_read = 0;
    fails([&] { failing_reader.read(&first, 1); }, ErrorCode::io);
    failing_source->max_read = 100;
    CHECK(failing_reader.read(&first, 1) == 1); // No stale window after an I/O failure.
}
void large_offsets() {
    class SparseSource final : public RandomAccessSource {
        Block tail_;
    public:
        explicit SparseSource(Block tail) : tail_(tail) {}
        std::uint64_t size() const override { return (std::uint64_t{1} << 32) + 32; }
        std::size_t read_at(std::uint64_t offset, Byte* output, std::size_t capacity) override {
            const auto count = static_cast<std::size_t>((std::min)(std::uint64_t(capacity), size() - offset));
            std::fill_n(output, count, Byte{0});
            for (std::size_t i = 0; i < count; ++i)
                if (offset + i >= size() - 16) output[i] = tail_[offset + i - (size() - 16)];
            return count;
        }
    };
    const KeyIv key{};
    const auto encrypted = transform_buffer(key, Mode::encrypt, nullptr, 0);
    Block tail{}; std::copy(encrypted.begin(), encrypted.end(), tail.begin());
    DecryptReader reader(std::make_shared<SparseSource>(tail), key);
    CHECK(reader.size() == (std::uint64_t{1} << 32) + 16);
    reader.seek((std::uint64_t{1} << 32) + 3);
    std::array<Byte, 4> output{};
    CHECK(reader.read(output.data(), output.size()) == output.size());
    const Block zero{};
    const auto expected = transform_blocks(key, Mode::decrypt, zero.data(), zero.size());
    CHECK(std::equal(output.begin(), output.end(), expected.begin() + 3));
}
void compatibility() {
    Encryption e;
    e.GenerateKey(L"abc");
    const std::string text("hello\0asset", 11);
    e.SetEncryptStr(text);
    CHECK(e.Encrypt());
    const Bytes cipher(e.GetOutputData(), e.GetOutputData() + e.GetOutputDataLength());
    CHECK(e.GetOutputStr() != nullptr);
    e.ReleaseOutputStr();
    CHECK(e.GetOutputDataLength() == cipher.size());
    e.SetEncryptData(cipher.data(), static_cast<DWORD>(cipher.size()));
    CHECK(e.Decrypt());
    CHECK(std::string(e.GetOutputStr(), e.GetOutputDataLength()) == text);
    e.SetEncryptStr(std::wstring(L"\u5bc6\U0001f600"));
    CHECK(e.GetInputDataLength() == 6); // Windows UTF-16 storage on every host.
    e.SetEncryptStr(std::string("abc"));
    CHECK(std::wstring(e.GetHash()) == L"900150983cd24fb0d6963f7d28e17f72");
    CHECK(e.GetOutputDataLength() == 16);
    e.SetEncryptStr(std::string());
    CHECK(e.Encrypt() && e.GetOutputDataLength() == 16);
    e.SetEncryptData(e.GetOutputData(), e.GetOutputDataLength());
    CHECK(e.Decrypt() && e.GetOutputDataLength() == 0 && e.GetOutputData());
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto file = std::filesystem::temp_directory_path().wstring() + L"/ofe-\u5bc6\u6587-" + std::to_wstring(stamp);
    CHECK(e.SaveFile(file.c_str()));
    CHECK(e.OpenFile(file.c_str()) && e.GetInputDataLength() == 0);
    e.SetEncryptStr(text);
    CHECK(e.SaveFile(file.c_str(), true));
    CHECK(e.EncryptFileDirectly(file.c_str()));
    const Bytes output(e.GetOutputData(), e.GetOutputData() + e.GetOutputDataLength());
    CHECK(e.SaveFile(file.c_str()));
    CHECK(e.DecryptFileDirectly(file.c_str()));
    CHECK(std::string(e.GetOutputStr(), e.GetOutputDataLength()) == text);
    CHECK(e.EncryptFile(L"/missing/ofe-asset") == false && !e.GetOutputData());
    std::filesystem::remove(legacy::path(file.c_str()));
    EncryptionStreamHelper stream(L"abc");
    Bytes src(32, 42), result(48);
    auto n = stream.Stream(src.data(), 16, result.data(), result.size(), Mode::encrypt, false);
    n += stream.Stream(src.data() + 16, 16, result.data() + n, result.size() - n, Mode::encrypt, true);
    const auto material = legacy::derive(u"abc", legacy::native_word_size);
    CHECK(result == transform_buffer(material, Mode::encrypt, src.data(), src.size()) && n == 48);
    EncryptionStreamHelper decrypt_stream(L"abc");
    Bytes restored(48);
    auto count = decrypt_stream.Stream(result.data(), 16, restored.data(), restored.size(), Mode::decrypt, false);
    count += decrypt_stream.Stream(result.data() + 16, 32, restored.data() + count,
        restored.size() - count, Mode::decrypt, true);
    restored.resize(count);
    CHECK(restored == src);
#ifdef _WIN32
    EncryptionStreamHelper old_signature(L"abc");
    Bytes legacy_output(48);
    CHECK(old_signature.Stream(src.data(), src.size(), legacy_output.data(), legacy_output.size(),
        BCryptEncrypt, true) == 48 && legacy_output == result);
#endif
}
int main() {
    try {
        known_answers(); streaming_and_seeking(); large_offsets(); compatibility();
        CHECK(test_c_api() == 0);
        std::cout << "PASS: NIST AES, legacy Win32/Win64 vectors, short reads, random seeks, >4GiB, facade, C ABI\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
