#include "Encryption.h"
#include "Encryption/CAPI.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
using namespace encryption;
namespace {
void check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
Bytes read(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    check(file.is_open(), "Open asset compatibility fixture");
    return Bytes(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}
std::string sha256(const Bytes& bytes) {
    Hash hash(Digest::sha256); hash.update(bytes.data(), bytes.size());
    std::string result;
    constexpr char digits[] = "0123456789abcdef";
    for (const auto byte : hash.finish()) { result += digits[byte >> 4]; result += digits[byte & 15]; }
    return result;
}
std::filesystem::path find_fixtures() {
    auto parent = std::filesystem::current_path();
    for (;;) {
        auto path = parent / "Extensions/NeoPic/ToInstall/Files/Examples/NeoPic";
        if (std::filesystem::exists(path / "Test_Encrypt.dat")) return path;
        if (parent.parent_path() == parent) break;
        parent = parent.parent_path();
    }
    throw std::runtime_error("Supply Test_Encrypt.dat and Test_Origin.jpg paths to EncryptionTests");
}
}
void test_existing_asset(int argc, char** argv) {
    std::filesystem::path encrypted_path, original_path;
    if (argc >= 3) { encrypted_path = argv[1]; original_path = argv[2]; }
    else {
        const auto fixtures = find_fixtures();
        encrypted_path = fixtures / "Test_Encrypt.dat"; original_path = fixtures / "Test_Origin.jpg";
    }
    const auto encrypted = read(encrypted_path), original = read(original_path);
    // These are the supplied old ciphertext and its existing repository original.
    check(sha256(encrypted) == "fe86c6641b02d5ad6f2629e441c872c796453aeb98938cc17abdbf76d777d199", "Old ciphertext fixture changed");
    check(sha256(original) == "da90458ef9ad3c0db78b208c29b397aece46f352f7065b2c96dd36140423d437", "Original JPEG fixture changed");
    const auto key = keygen::from_utf8("12345678ABCDEFGH");
    check(transform_buffer(key, Mode::decrypt, encrypted.data(), encrypted.size()) == original, "Core must restore the exact old JPEG bytes");
    check(transform_buffer(key, Mode::encrypt, original.data(), original.size()) == encrypted, "Core must reproduce the exact old ciphertext");
    for (const auto mode : {Mode::encrypt, Mode::decrypt}) {
        const auto& source = mode == Mode::encrypt ? original : encrypted;
        const auto& expected = mode == Mode::encrypt ? encrypted : original;
        for (const std::size_t chunk : {17u, 4093u, 65536u}) {
            std::size_t offset = 0; Bytes output;
            transform(key, mode, [&](Byte* buffer, std::size_t capacity) {
                const auto count = (std::min)({capacity, chunk, source.size() - offset});
                if (count) std::memcpy(buffer, source.data() + offset, count);
                offset += count; return count;
            }, [&](const Byte* data, std::size_t count) { output.insert(output.end(), data, data + count); });
            check(output == expected, "Streaming must match the old file for unaligned chunks");
        }
    }
    DecryptReader reader(std::make_shared<FileSource>(encrypted_path), key);
    check(reader.size() == original.size(), "Seekable plaintext size");
    std::mt19937 random(2026);
    std::array<Byte, 4097> output{};
    for (int i = 0; i < 100; ++i) {
        const auto position = i == 0 ? original.size() : random() % (original.size() + 1);
        reader.seek(position);
        const auto count = reader.read(output.data(), output.size());
        check(count == (std::min)(output.size(), original.size() - position), "Seekable read length");
        check(std::equal(output.begin(), output.begin() + count, original.begin() + position), "Random seek must match original bytes");
    }
    Bytes c_output(encrypted.size() + 16); std::size_t count = 0;
    check(Encryption_DecryptBuffer(key.key.data(), key.iv.data(), encrypted.data(), encrypted.size(),
        c_output.data(), c_output.size(), &count) == ENCRYPTION_OK, "C ABI old-file decryption");
    c_output.resize(count); check(c_output == original, "C ABI original JPEG equality");
    c_output.resize(original.size() + 16);
    check(Encryption_EncryptBuffer(key.key.data(), key.iv.data(), original.data(), original.size(),
        c_output.data(), c_output.size(), &count) == ENCRYPTION_OK, "C ABI old-file encryption");
    c_output.resize(count); check(c_output == encrypted, "C ABI original ciphertext equality");
    Encryption facade; facade.GenerateKey(L"12345678ABCDEFGH");
    check(facade.DecryptFileDirectly(encrypted_path.wstring().c_str()), "Existing DecryptFileDirectly API");
    check(facade.GetOutputDataLength() == original.size() &&
        std::equal(original.begin(), original.end(), facade.GetOutputData()), "Facade original JPEG equality");
    if (argc >= 4) check(facade.SaveFile(std::filesystem::path(argv[3]).wstring().c_str()), "Save decrypted compatibility preview");
    check(facade.EncryptFileDirectly(original_path.wstring().c_str()), "Existing EncryptFileDirectly API");
    check(facade.GetOutputDataLength() == encrypted.size() &&
        std::equal(encrypted.begin(), encrypted.end(), facade.GetOutputData()), "Facade original ciphertext equality");
    std::cout << "PASS: supplied old asset; " << original.size() << " plaintext bytes, " << encrypted.size()
              << " ciphertext bytes; exact decrypt/re-encrypt, C ABI/facade, streaming and 100 seeks\n";
}
