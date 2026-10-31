# Shared encryption core and Fusion compatibility

All platforms, **including Windows**, use the same OpenSSL EVP implementation.
`EncryptionCore.h` is the C++17 API. `Encryption.h` / `Encryption.cpp` only retain
the historical Fusion interface and delegate to that core. There is no fallback
to the former BCrypt encryption/hash implementation.

## Interfaces and ownership

- `encryption::Cipher` owns an incremental AES-128-CBC operation. Supply an explicit
  `KeyIv`, call `update` with arbitrary chunks, then call `finish` once for PKCS#7.
  Cipher and Hash contexts are movable; an update/final failure invalidates them.
- `encryption::transform` connects a reader and writer with bounded 64 KiB input
  chunks. The reader returns zero at EOF; the writer consumes the whole chunk or
  throws. `transform_buffer` is the explicitly full-memory convenience operation.
- `encryption::DecryptReader` provides pull reads, a 64-bit plaintext size/cursor,
  absolute seeking and independent clones over a shared `RandomAccessSource`.
  It reads only the last ciphertext block and its predecessor to validate padding
  and determine size, then caches at most 64 KiB of plaintext on demand. FileSource
  serializes positioned reads so clones can serve separate decoders safely.
- `EncryptionAvio.h` adapts that reader to FFmpeg AVIO: EOF, byte seeks,
  `AVSEEK_SIZE`, negative/overflow checks and independent secondary demuxers.
  Close AVFormatContext before destroying its DecryptAvio.
- `EncryptionPortable*.h` expose C functions with opaque, explicitly destroyed
  contexts, caller-owned buffers and exception-to-result translation. The C ABI
  supports incremental ciphers/hashes, callback streams, FILE adapters and pull
  readers (`Encryption_ReaderCreate/Read/Seek/Size/Destroy`). Callback storage must
  outlive its reader and C callbacks must not throw. Buffer outputs must be
  discarded on failure; stream writers may already have received partial output.

```cpp
#include "EncryptionCore.h"

auto source = std::make_shared<encryption::FileSource>(asset_path);
encryption::DecryptReader reader(source, material); // explicit key + IV
reader.seek(12345);
std::array<std::uint8_t, 4096> bytes;
auto count = reader.read(bytes.data(), bytes.size());
```

The unused prototype `DecryptReaderContext` C++ buffer wrapper is removed; C++ callers
use the core directly. C stream adapters delegate to the core streaming loop.

The historical Encryption API keeps its class/method names, algorithm-name
parameters, DWORD lengths, string accessors and output ownership. FILE/direct
operations still collect output in the object for SaveFile; they do not overwrite
the source. Rebuild existing extensions: C++ class layout/binary ABI is not retained.
C++ callers should use the core for large/streamed assets instead of the legacy
DWORD/full-buffer accessors. Copies of the facade now own independent buffers.

FFMpeg's normal encrypted-file path uses DecryptAvio when memory caching is off.
The existing explicit memory-cache option keeps its full-buffer behavior. Actions,
expressions and file/key parameters retain their existing interface. Thumbnail /
secondary frame decoding gets an independent stream, including while playback seeks.

## Existing asset compatibility

`EncryptionLegacy.h` isolates the old password convention. It explicitly converts
wide strings to Windows UTF-16 units and reproduces the original seed expansion
for inputs shorter than 16 units, and XOR folding for longer inputs. Empty input
retains bytes `00..0f`. The modern core does not interpret passwords or string encodings.

The old algorithm used `size_t`; short passwords can yield different IVs in
32-bit and 64-bit Windows tools. The facade preserves its Windows architecture's
behavior; non-Windows defaults to the Fusion Win32 format. C++ callers can choose
`legacy::WordSize::win32` or `win64`. C callers pass UTF-16LE bytes to
`Encryption_DeriveKeyAndIv` (Win32), or use `Encryption_DeriveLegacyKeyAndIv` with
32 or 64 explicitly. Android Java adapters should pass UTF-16 units explicitly,
not reinterpret a native `wchar_t` or use modified UTF-8. No encoding detection or
silent fallback between asset formats is performed.

AES-CBC and deterministic legacy key/IV derivation are retained for compatibility;
there is no new file header. CBC does not authenticate ciphertext: valid padding
is not proof of the right password or unmodified content. This is an asset format,
not a password-hardening API.

Intentional bug fixes in the facade: file/read/write failures propagate; failed
operations clear stale output; releasing a temporary string does not erase the
corresponding binary buffer's length; empty files work; wide input is UTF-16LE on
all platforms. Caller-owned data is never freed.

## Windows: Visual Studio builds

Open `EncryptionPortable.sln` in VS 2022 or build with MSBuild from a VS Developer
Command Prompt. Select Win32 for Fusion or x64 for native tools. Release and Debug
use `/MD` and `/MDd`, matching the existing extension projects, and preserve the
repository's `/Zc:wchar_t-` convention. Override PlatformToolset for another VS.

Provide a **static** target OpenSSL SDK with `include/openssl/evp.h`, Release
`lib/libcrypto.lib` and Debug `debug/lib/libcrypto.lib`. For example, with vcpkg:

```bat
vcpkg install openssl:x86-windows-static-md openssl:x64-windows-static-md
msbuild EncryptionPortable.sln /m /p:Configuration=Release /p:Platform=Win32 /p:OfeOpenSSLRoot=C:\vcpkg\installed\x86-windows-static-md
build\Win32\Release\EncryptionTests.exe
```

`../Encryption.props` is imported by all nine existing projects compiling
Encryption.cpp (Deque2D, Encrypter, FFMpeg, Fini, NeoPic, NeoStr, SDL_Mixer,
StringSplitter and WinAPI). It adds OpenSSL includes, architecture/configuration
library paths and system link dependencies. Set OfeOpenSSLRoot globally on the
MSBuild command or via a shared user property sheet; VCPKG_ROOT is also discovered.
OfeOpenSSLLibDir can override an SDK with another directory layout. VS fails clearly
if the target SDK is missing. Do not mix x86/x64, static/dynamic CRTs or host/target
libraries. The old Encryption.cpp project item remains the source entry point.

Static OpenSSL avoids introducing a shared runtime/version/install dependency
across independently distributed .mfx files. The small core contains no global
cache/service. It can also be built as one shared C ABI library where a consumer
controls deployment; static linkage does not deduplicate libcrypto across plugins.
A common dynamic deployment can be evaluated using actual linked sizes and the
exporter's loader contract. The isolated legacy Stream helper accepts BCrypt
function pointers as mode tags for source compatibility; it never invokes them.

## Other platforms: library builds

Use target OpenSSL (prefer a supported OpenSSL 3 release) and CMake. Tests are opt-in
so a mobile cross-build does not try to execute a target binary on the host.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR=/path/to/target/openssl
cmake --build build --parallel
cmake --install build --prefix /path/to/staging
```

This stages the library and public headers under `include/encryption`. Set
BUILD_SHARED_LIBS=ON for a C ABI shared library; Windows C declarations have explicit
DLL exports/imports. Existing Windows plugins compile the C++ facade themselves.

For Android, configure once per ABI with the NDK toolchain, ANDROID_ABI and the
consumer's native API level. Supply ABI-specific OpenSSL headers/libraries. The
plugin's publisher must prebuild/package its owned native libraries and required
STL/dependency binaries under jniLibs; do not replace Fusion's Gradle templates.
The core needs no Java Runtime or JNI changes by itself.

For iOS, compile separate device/simulator libraries with Apple's SDK and matching
OpenSSL slices, then include the required libraries/headers in the consuming
plugin's static-library/XCFramework build. Verify the consumer's deployment target
supports its libc++ filesystem use. This core does not publish an .ext, replace
Runtime sources, or establish that a Fusion mobile exporter packages dependencies.
Use the platform's existing publisher and verify its final link/install/export.

## Verification

```sh
cmake -S . -B build -DENCRYPTION_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The suite covers NIST CBC answers, frozen old Win32/Win64 derivation vectors,
Unicode, arbitrary chunks, finalization/errors, short reads, random/EOF seeking,
>4 GiB offsets, bounded first-read I/O, facade operations and compilation/calls
from real C. If pkg-config locates FFmpeg development libraries, CMake also tests
real encrypted WAV demuxing, payload equality, AVIO sizes, timestamp seeking and
independent primary/secondary contexts. `.github/workflows/encryption.yml` builds
with native VS MSBuild on Windows and CMake on Linux/macOS. Full Fusion .mfx,
Android APK and Apple device/exporter validation are separate from these core tests.
