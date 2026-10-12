# Portable encryption sample

This standalone C++17 sample exposes a C-style API for code that consumes or
produces buffers, including FFmpeg AVIO callbacks. The crypto core has no
BCrypt or Win32 dependency. OpenSSL EVP supplies AES-128-CBC/PKCS#7 encryption
and decryption, plus streaming MD5 (the legacy `GetHash` default) and SHA-256.

## One-shot memory helpers

For data already in memory, include `EncryptionPortableConvenience.h` and call
`Encryption_EncryptBuffer`, `Encryption_DecryptBuffer`, or
`Encryption_HashBuffer`. The helpers allocate temporary workspace internally,
create/destroy the incremental context, process PKCS#7 finalization, and write
the result to your buffer. They do not take ownership of caller buffers.

```c
#include "EncryptionPortableConvenience.h"
#include <string.h>

int encrypt_and_decrypt_asset(void) {
    const wchar_t key_input[] = L"0123456789abcdef";
    const uint8_t asset[] = "asset";
    uint8_t key[ENCRYPTION_AES_KEY_SIZE];
    uint8_t iv[ENCRYPTION_AES_IV_SIZE];
    uint8_t encrypted[sizeof(asset) + ENCRYPTION_BLOCK_SIZE];
    uint8_t decrypted[sizeof(encrypted)];
    size_t encrypted_size = 0;
    size_t decrypted_size = 0;

    if (Encryption_DeriveKeyAndIv(
            key_input, sizeof(key_input) / sizeof(key_input[0]) - 1,
            key, iv) != ENCRYPTION_OK) {
        return 1;
    }
    if (Encryption_EncryptBuffer(
            key, iv, asset, sizeof(asset) - 1,
            encrypted, sizeof(encrypted), &encrypted_size) != ENCRYPTION_OK) {
        return 2;
    }
    if (Encryption_DecryptBuffer(
            key, iv, encrypted, encrypted_size,
            decrypted, sizeof(decrypted), &decrypted_size) != ENCRYPTION_OK) {
        return 3;
    }

    return decrypted_size == sizeof(asset) - 1 &&
           memcmp(decrypted, asset, decrypted_size) == 0 ? 0 : 4;
}
```

For one-shot encryption, reserve `input_length + 16` bytes (PKCS#7 always adds
padding, including a full block for block-aligned input). For decryption,
output capacity equal to ciphertext length is sufficient. On invalid padding,
the one-shot decrypt helper does not copy partial plaintext into the caller's
output buffer.

## Incremental C API and FFmpeg AVIO

For streaming, include `EncryptionPortable.h`. Create a cipher context with
`Encryption_CipherCreate`, feed chunks to `Encryption_CipherUpdate`, consume
exactly the reported output byte count, and call `Encryption_CipherFinal` at
end-of-input. Encrypt and decrypt use the same API selected by
`EncryptionCipherMode`. Update output buffers must provide at least
`input_length + 16` bytes; Final output buffers must provide 16 bytes.

An AVIO read callback can keep a pending plaintext buffer and offset. When it
runs out, read ciphertext from the memory asset, feed it through Update, and
return available plaintext. At ciphertext EOF call Final once, deliver any
remaining bytes, then report EOF. This is sequential; see the seek notes
below. Decryption may emit bytes before Final validates PKCS#7 padding, and
CBC does not authenticate ciphertext.

The hash API has one-shot `Encryption_HashBuffer` and incremental
`Encryption_HashCreate/Update/Final/Destroy` forms. MD5 matches the existing
default hash; SHA-256 is also available.

## Seeking in CBC data

FFmpeg performs timestamp seeking at the demuxer layer, generally choosing a
keyframe and decoding forward. The AVIO seek callback works in byte offsets.
For a plaintext byte position `p`, restart CBC at the containing block:

```text
block_offset = p - (p % 16)
skip_bytes   = p % 16
seek_iv      = original_iv                         if block_offset == 0
               ciphertext[block_offset - 16..-1]   otherwise
```

Create a fresh decrypt context using `seek_iv`, begin reading ciphertext at
`block_offset`, then discard `skip_bytes` of produced plaintext before
returning data to AVIO. For `SEEK_END`/size queries, determine plaintext size
once by finalizing decryption of the last block and validating the padding.
Seeking is not implemented as a separate convenience function in this sample;
the incremental API provides the pieces to build the AVIO adapter. Seeking
requires random access to the ciphertext source.

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
hardening. It adds no salt or random per-file metadata.

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
