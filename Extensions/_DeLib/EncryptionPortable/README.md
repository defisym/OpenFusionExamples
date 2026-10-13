# Portable encryption sample

This standalone C++17 sample exposes a C-style API for code that consumes or
produces buffers, including FFmpeg AVIO callbacks. The crypto core has no
BCrypt or Win32 dependency. OpenSSL EVP supplies AES-128-CBC/PKCS#7 encryption
and decryption, plus streaming MD5 (the legacy `GetHash` default) and SHA-256.

## One decrypt backend, two output choices

Both file-reading modes use the same incremental CBC decryptor and padding
finalization. The caller chooses only where each plaintext chunk goes:

1. `Encryption_DecryptReader` sends chunks to a write callback. Use this for
   audio/video playback; the callback can feed an AVIO-side pending buffer.
2. `Encryption_DecryptReaderToBuffer` sends those same chunks to a buffer
   sink owned by the caller. Use this for image/texture loading when the full
   plaintext is needed in memory.

Both functions read ciphertext through the same `EncryptionReadCallback`.
The callback returns `ENCRYPTION_OK` with `bytes_read == 0` at EOF. A sample
adapter for a caller-opened C `FILE*` is:

```c
static EncryptionResult read_file(
    void *user_data, uint8_t *buffer, size_t capacity, size_t *bytes_read) {
    FILE *file = (FILE *)user_data;
    *bytes_read = fread(buffer, 1, capacity, file);
    return ferror(file) ? ENCRYPTION_CRYPTO_ERROR : ENCRYPTION_OK;
}
```

The file/path API stays with the caller, so Windows, Linux, macOS, Android, and
iOS can use their preferred file handles while sharing the same decryptor.

### Full-buffer image/texture output

The output buffer needs a safe capacity before decryption starts. CBC with
PKCS#7 padding always produces fewer plaintext bytes than ciphertext bytes,
so ciphertext size is a simple safe capacity. The exact plaintext length
depends on the final padding and is returned through `output_length` after
successful decryption.

C++ callers can use `EncryptionPortable.hpp`:

```cpp
#include "EncryptionPortable.hpp"
#include <vector>

bool load_texture(const encryption_portable::Key& key,
                  const encryption_portable::Iv& iv,
                  std::size_t encrypted_size,
                  EncryptionReadCallback read_file,
                  void* file) {
    encryption_portable::DecryptReaderContext decryptor(key, iv, encrypted_size);
if (!decryptor.IsValid()) {
    return false;
}
std::vector<std::uint8_t> pixels(decryptor.GetDecryptedBufferSize());
std::size_t pixel_size = 0;
const auto result = decryptor.DecryptToBuffer(
    read_file, file, pixels.data(), pixels.size(), &pixel_size);
if (result != ENCRYPTION_OK) {
    return false; // discard pixels; padding or I/O may have failed
}
pixels.resize(pixel_size);
    return true;
}
```

The C API can call `Encryption_DecryptReaderToBuffer` directly, passing a
buffer with capacity equal to the encrypted file size.

### Audio/video streaming output

`Encryption_DecryptReader` uses the exact same read/decrypt/final path, but
calls the supplied write callback for each plaintext chunk. For FFmpeg AVIO,
that callback should append plaintext to a pending queue; the AVIO read
callback drains that queue and requests more decrypted data when it runs dry.
Decryption may emit bytes before Final validates PKCS#7 padding, so treat a
later failure as invalidating the playback operation.

For in-memory input, the convenience functions
`Encryption_EncryptBuffer`, `Encryption_DecryptBuffer`, and
`Encryption_HashBuffer` handle the whole buffer in one call. Encryption
output capacity must be `input_length + 16`; decryption output capacity equal
to ciphertext length is sufficient.

## Incremental C API and FFmpeg seeking

The lower-level API is `Encryption_CipherCreate/Update/Final/Destroy`.
It is also useful when the caller needs to control buffering directly.
FFmpeg performs timestamp seeking at the demuxer layer, generally choosing a
keyframe and decoding forward. The AVIO seek callback works in byte offsets.
For plaintext offset `p`, restart CBC at its containing block:

```text
block_offset = p - (p % 16)
skip_bytes   = p % 16
seek_iv      = original_iv                         if block_offset == 0
               ciphertext[block_offset - 16..-1]   otherwise
```

Create a fresh decrypt context using `seek_iv`, read ciphertext starting at
`block_offset`, and discard `skip_bytes` of produced plaintext. For
`SEEK_END`/size queries, decrypt the final block once to validate padding and
determine the exact plaintext length. Seeking requires random access to the
ciphertext source; the two high-level reader helpers are sequential.

## Deterministic key and IV generation

`Encryption_DeriveKeyAndIv` normalizes `wchar_t` units using
`if constexpr(sizeof(wchar_t))`. On Windows, 16-bit values are copied
directly. On platforms with 32-bit `wchar_t`, values from 0 through 0xFFFF
are preserved as raw 16-bit key material, and larger valid Unicode values are
encoded as UTF-16 surrogate pairs. Values above U+10FFFF are rejected.

The function reproduces the existing 16-unit layout: the first eight 16-bit
units form the 16-byte AES key and the next eight form the 16-byte IV. Inputs
shorter than 16 units use the original fill algorithm; inputs longer than 16
units XOR each trailing unit into all key and IV words. Empty input uses the
legacy default byte arrays. Words are serialized explicitly as little-endian,
so CPU byte order does not affect the result.

The original short-input expansion used `size_t`, making results depend on
32-bit versus 64-bit builds. This implementation fixes that arithmetic to
32 bits, matching the 32-bit Windows behavior and making the result stable
across targets. If existing assets were generated by a 64-bit Windows build
with keys shorter than 16 UTF-16 units, those derived values may differ and
should be checked before switching them over.

This is intended for deterministic transparent asset encryption, not password
hardening. It adds no salt or random per-file metadata. CBC does not
authenticate ciphertext.

## Dependency and build

Install or provide OpenSSL for the target platform, then configure this
directory with CMake:

```sh
cmake -S . -B build -DOPENSSL_ROOT_DIR=/path/to/target/openssl
cmake --build build
```

For Android and iOS, use the appropriate NDK or Xcode toolchain and point
CMake's OpenSSL package paths at a cross-compiled OpenSSL installation. Do not
link a host `libcrypto` into a mobile target. This sample does not bundle
OpenSSL or claim per-platform build verification; dependency packaging remains
the consuming project's responsibility.
