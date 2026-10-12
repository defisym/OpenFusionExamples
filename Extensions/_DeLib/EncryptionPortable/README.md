# Portable encryption sample

This is a small, independent C++17 sample that removes Windows BCrypt and
Windows file/path APIs from the cryptographic core. The same OpenSSL EVP code
can be built for Windows, Linux, macOS, Android NDK, and iOS when that target
has an OpenSSL 1.1.1+ `libcrypto` package built for its ABI.

It provides AES-256-CBC with PKCS#7 padding, PBKDF2-HMAC-SHA-256 key/IV
derivation from UTF-8 password bytes and a per-file random salt, streaming
CBC decryption, and streaming SHA-256. Input and output are standard C++
streams, so path handling stays with the calling application. OpenSSL is the
only platform-specific boundary.

## Dependency and build

Install or provide OpenSSL for the target platform, then configure this
directory with CMake:

```sh
cmake -S . -B build -DOPENSSL_ROOT_DIR=/path/to/target/openssl
cmake --build build
```

For Android and iOS, configure CMake with the appropriate NDK or Xcode toolchain
and point `OPENSSL_ROOT_DIR` (or CMake's OpenSSL package paths) at a
cross-compiled OpenSSL installation. Do not link a host `libcrypto` into a
mobile target. The crypto source itself does not use Win32, POSIX, JNI, or
Objective-C APIs. This sample does not bundle or build OpenSSL for each
platform; dependency packaging remains the consuming project's responsibility.

## File format and compatibility

For a new file, generate a fresh `Salt` with `make_salt`, derive a key and IV,
then encrypt with AES-256-CBC/PKCS#7 using the matching OpenSSL EVP encryption
path. Store a format version and the salt with the ciphertext. Never reuse the
same salt for different files encrypted under the same password.

This is a new format. It is **not** byte-compatible with the current
`Encryption::GenerateKey` behavior, which copies/combines native
`wchar_t` bytes and then uses AES-128-CBC. Keep the existing Windows
implementation for legacy ciphertext until a separately tested compatibility
decoder is added. A future integration should select the decoder from a
versioned file header rather than guessing.

CBC does not authenticate ciphertext. A valid padding result does not prove
that the key is correct or that the data was not modified. For security-sensitive
new formats, prefer an authenticated streaming format such as AES-GCM with a
carefully designed chunked format, or add and verify a MAC before accepting the
plaintext. The decrypt function can leave partial output on failure; write to a
temporary file and publish it only after success.
