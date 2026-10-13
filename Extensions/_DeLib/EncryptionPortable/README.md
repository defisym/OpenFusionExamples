# Portable encryption sample

This standalone C++17 sample exposes a C-style API for code that consumes or
produces buffers, including FFmpeg AVIO callbacks. The crypto core has no
BCrypt or Win32 dependency. OpenSSL EVP supplies AES-128-CBC/PKCS#7 encryption
and decryption, plus streaming MD5 (the legacy `GetHash` default) and SHA-256.

## One streaming cipher backend, multiple output choices

Encryption and decryption both read input incrementally through a C callback,
run the same CBC update/final loop, and send output chunks to a C write callback.
The selected mode controls whether PKCS#7 padding is added or validated.

1. `Encryption_EncryptReader` encrypts source chunks and writes ciphertext
   chunks to the callback. Use this to encrypt large files without holding the
   full input or output in memory.
2. `Encryption_DecryptReader` decrypts source chunks and writes plaintext
   chunks to the callback. Use this for audio/video playback.
3. `Encryption_EncryptReaderToBuffer` and `Encryption_DecryptReaderToBuffer`
   use the same reader/cipher loop with a caller-buffer sink for applications
   that need a complete result in memory.

All reader functions use the same `EncryptionReadCallback` for input.
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
iOS can use their preferred file handles while sharing the same cipher backend.

### Full-buffer image/texture output

For decryption, ciphertext size is a safe output capacity; the exact
plaintext size depends on the final PKCS#7 padding and is returned through
`output_length` after successful decryption. For encryption, reserve plaintext
size plus one AES block for padding.

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

`Encryption_DecryptReader` calls the supplied write callback for each plaintext
chunk. `Encryption_EncryptReader` has the same callback shape and backend, but
emits ciphertext chunks. Both retain only the current input/output chunks and
cipher state. For FFmpeg AVIO,
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

`Encryption_DeriveKeyAndIv` accepts `const void*` plus a byte count. It
never interprets strings or depends on `wchar_t`, UTF-8, UTF-16, or platform
default encodings. If an application supplies a readable string, pass its
chosen encoded bytes and exact byte length; the function treats them only as
key material.

The function forms little-endian 16-bit units from the input bytes. An odd
final byte becomes the low byte of the last unit, with a zero high byte. The
first 16 units fill the fixed key/IV material; each extra unit is XORed into
all key and IV words. Short input cycles its units until all 16 words are
filled. Empty input retains the legacy default byte arrays. Output words are
serialized explicitly as little-endian, so CPU byte order does not affect
the result.

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


## FILE-based convenience functions

File variants are available for encryption, decryption, and hashing:
Encryption_EncryptFile, Encryption_DecryptFile, Encryption_EncryptFileToBuffer,
Encryption_DecryptFileToBuffer, and Encryption_HashFile. They take caller-owned
FILE handles instead of paths, so the caller chooses platform-specific path
handling and encoding. Functions start at each handle's current position; they
neither rewind nor close handles.

File-to-file calls require distinct source and destination files, not merely
different FILE handles that refer to the same path. Never open the source path
as a truncating destination. The caller opens and closes both handles. If an
operation fails, discard the partial destination file.

Example file-to-file encryption:

```c
#include <stdio.h>
#include "EncryptionPortableConvenience.h"

FILE* source = fopen(source_path, "rb");
if (source == NULL) return 1;
FILE* destination = fopen(encrypted_path, "wb");
if (destination == NULL) { fclose(source); return 2; }
EncryptionResult result = Encryption_EncryptFile(source, destination, key, iv);
const int close_error = fclose(destination) != 0;
fclose(source);
if (result != ENCRYPTION_OK || close_error) { /* discard destination */ }
```

For file-to-memory decryption, allocate ciphertext_size bytes and call
Encryption_DecryptFileToBuffer; on success, output_length gives the exact
plaintext size. Encryption_EncryptFileToBuffer provides the matching
file-to-buffer operation. Encryption_HashFile computes MD5 or SHA-256
incrementally without loading the whole file.

The original EncryptFile did not overwrite its source: it loaded the source
into memory, retained the encrypted output in the Encryption object, and wrote
a file only when SaveFile was called with a destination path. EncryptFileDirectly
also kept its output in memory. The new file-to-file helpers require separate
source and destination files.
