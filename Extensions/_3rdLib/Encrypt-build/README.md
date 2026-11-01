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

The default path downloads OpenSSL **3.6.5** from the repository's fixed
[encrypt-sdk-openssl-3.6.5-v143 release](https://github.com/defisym/OpenFusionExamples/releases/tag/encrypt-sdk-openssl-3.6.5-v143).
EncryptVersion.cmake pins the URL and each architecture's SHA256, like the FFmpeg
SDK preparation. The packages were built and tested with VS 2022 (v143), static
OpenSSL and dynamic CRT; each contains Release and Debug libraries.

Build the solution normally. No manual download, vcpkg, Perl or NASM is needed.
VS C++ tools and CMake 3.21+ are sufficient; the VS-bundled CMake is discovered
automatically. Preparation does not silently fall back to building from source
when a download fails.

From a VS Developer Command Prompt, prepare without compiling an extension:

```bat
msbuild Extensions\_3rdLib\Encrypt-build\EncryptDependencies.vcxproj /m /p:Configuration=Release /p:Platform=Win32
```

Use Platform=x64 for x64. The SDK is validated and installed to the default path
above, and Encryption.props automatically supplies the include/library paths.
Subsequent builds reuse a complete matching SDK, including without network access.
Missing SDK files can be restored from the local archive cache.

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

Normally the fixed repository release is downloaded automatically. To use a local
archive, download openssl-x86.zip or openssl-x64.zip and its .sha256 file from the
release. A user-supplied archive or URL overrides the repository default:

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

## Explicit source builds

For another toolset, or to produce an updated SDK, opt in explicitly:

```bat
msbuild Extensions\_3rdLib\Encrypt-build\EncryptDependencies.vcxproj /m /p:Configuration=Release /p:Platform=Win32 /p:OfeEncryptBuildFromSource=true
```

This path pins vcpkg commit `9a5dfef5d1a0438a7d24856a85bd60f309d355e9` and OpenSSL
3.6.5 in EncryptVersion.cmake / vcpkg.json. It needs Windows, VS C++ tools, Git and
CMake; vcpkg acquires its additional build tools. The selected VS instance and
PlatformToolset are passed through. Both configurations use static libraries with
dynamic CRT. An explicit local archive/URL takes precedence over source mode.

## Publishing SDK releases

Encryption CI first tests the default downloaded SDK with native VS Release/Debug,
then builds source SDKs and uploads Encrypt-sdk-Win32 / Encrypt-sdk-x64 artifacts.
The Publish encryption SDK workflow promotes these archives, verifies their SHA256
and SDK layouts, and creates a versioned release without changing the repository's
latest extension release. Its release notes record the source commit and CI run.

The initial release uses successful CI run 41587062927. For a dependency update,
select a successful Encryption core run containing both SDK artifacts and publish
a new SDK tag through the workflow's manual run inputs. Update the version, URL
and per-architecture hashes in EncryptVersion.cmake together; keep existing tags
and assets unchanged. The publish workflow also runs when an encrypt-sdk-* tag is
pushed, using its configured source run.
