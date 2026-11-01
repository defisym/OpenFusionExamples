# EncryptDependencies: Windows OpenSSL SDK preparation

EncryptDependencies is a Visual Studio utility project, following the repository's
FFmpeg/SDL dependency-project pattern. All nine existing Encryption consumers and
the native EncryptionPortable solution reference it. Build the solution normally:
preparation runs before ClCompile and reuses a complete matching SDK afterward.

Default output:

```text
Extensions/_3rdLib/Encrypt-sdk/x86/   # Win32
Extensions/_3rdLib/Encrypt-sdk/x64/   # x64
  include/openssl/
  lib/libcrypto.lib                 # static library, /MD
  debug/lib/libcrypto.lib           # static library, /MDd
  share/openssl/copyright
  ofe-encryption-sdk.json
```

The source build pins vcpkg commit `9a5dfef5d1a0438a7d24856a85bd60f309d355e9`
and OpenSSL **3.6.5** in EncryptVersion.cmake / vcpkg.json. Updating them is an
explicit dependency update. The source and tool downloads are public upstream
vcpkg/OpenSSL downloads. VS C++ tools, Git and CMake 3.21+ must be available;
vcpkg acquires the additional source-build tools it needs.

From a VS Developer Command Prompt, prepare without compiling an extension:

```bat
msbuild Extensions\_3rdLib\Encrypt-build\EncryptDependencies.vcxproj /m /p:Configuration=Release /p:Platform=Win32
```

Use Platform=x64 for x64. The source build follows PlatformToolset and the selected
VS instance. The overlay triplet explicitly selects static OpenSSL and dynamic CRT.
It prepares both configurations so Debug Unicode and Release/Run_Only Unicode
consumers use the correct runtime libraries.

## Custom paths and existing SDKs

- OfeOpenSSLRoot selects the architecture-specific SDK directory.
- OfeAutoBuildEncrypt defaults to true for the managed default, false for an explicit
  external root. Set it to true to prepare a custom destination.
- OfeOpenSSLLibDir overrides the selected lib/debug/lib directory when necessary.
- OfeEncryptCMake selects a CMake executable; the VS-bundled one is discovered first.
- OfeEncryptWorkDir changes the source/tool/download/build cache, defaulting to
  Encrypt-build/out. Generated SDKs and caches are ignored by Git.

For example:

```bat
msbuild Extensions\Encrypter\Encrypter.sln /p:Configuration="Release Unicode" /p:Platform=Win32 /p:OfeOpenSSLRoot=D:\SDKs\Encrypt\x86 /p:OfeAutoBuildEncrypt=true
```

An already prepared external SDK can be selected with OfeOpenSSLRoot alone. Match
architecture, static linkage and /MD vs /MDd. This does not install OpenSSL globally
or change PATH for application runtime loading.

## Prebuilt/offline SDKs

The encryption CI uploads Encrypt-sdk-Win32 and Encrypt-sdk-x64 artifacts containing
an OpenSSL SDK ZIP and its SHA256. Extract the outer Actions artifact ZIP, then
supply the contained openssl-x86.zip or openssl-x64.zip:

```bat
msbuild Extensions\_3rdLib\Encrypt-build\EncryptDependencies.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:OfeEncryptPrebuiltArchive=D:\Downloads\openssl-x86.zip /p:OfeEncryptPrebuiltSha256=THE_64_DIGIT_SHA256
```

OfeEncryptPrebuiltUrl can download the same archive from a chosen HTTPS location.
A SHA256 is required for either input. With a custom destination also pass
OfeOpenSSLRoot and OfeAutoBuildEncrypt=true. Prebuilt staging does not need Git,
vcpkg, Perl or NASM. It can also be invoked with CMake on other hosts:

```sh
cmake -DOFE_ARCH=x86 -DOFE_OPENSSL_ROOT=/path/to/sdk \
  -DOFE_PREBUILT_ARCHIVE=/path/to/openssl-x86.zip \
  -DOFE_PREBUILT_SHA256=THE_64_DIGIT_SHA256 -P EnsureSDK.cmake
```

The ZIP root must contain the SDK files above, including both configurations and
the license. ofe-encryption-sdk.json identifies arch (x86/x64), version (3.6.5),
linkage (static) and crt (dynamic). The downloaded archive's SHA256, required files,
manifest, version header and library archive signatures are checked before replacing
the current SDK. Native CI then compiles/links/runs against the produced libraries.

Preparation is serialized across projects. A signature covers version/configuration
and preparation scripts. Missing SDK files cause repair; repeated builds reuse the
existing SDK. Failed download, hash/metadata validation or builds preserve the
previous SDK. Logs and downloads remain under Encrypt-build/out. Once prepared,
a complete matching SDK is reusable without a network connection.
