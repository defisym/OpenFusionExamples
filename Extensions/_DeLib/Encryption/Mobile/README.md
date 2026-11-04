# Shared mobile encryption runtime

This module is owned by the shared Encryption library, not by Fini or another
plugin. The existing CAPI.cpp, Core.h and KeyGen.h implement the algorithms.
Clients use CAPI.h; only this runtime links libcrypto. C ABI version is 1.

## Android

CMake target `ofe_encryption` builds `libofe_encryption.so`. Its OpenSSL symbols
are private, and only Encryption_* functions are exported. The shared build cache
is `work/mobile-encryption/android/<ABI>` and is reused by plugin publishers.
The supplied Fini publisher builds/imports this common dependency, then links its
own libfini.so against it. It does not statically link encryption or libcrypto.

`android_runtime.publish` creates `ToInstall/OFEEncryption.zip`, containing one
common library and matching libc++_shared.so per ABI, shared source/license
material, ABI version and SHA256 manifest. Plugin ZIPs must not contain these
common runtime binaries or their implementations. The client header is allowed.

For an exported Android project, stage the shared package **once**:

```sh
python3 Extensions/_DeLib/Encryption/Mobile/android_runtime.py \
  --archive Extensions/_DeLib/Encryption/Mobile/ToInstall/OFEEncryption.zip \
  --project /path/to/exported-android-project
```

Or pass `--android-project /path/to/project` to the Fini publisher. This only
adds the common native libraries; it never replaces Gradle or Runtime templates.
Repeated installation of the identical runtime is a no-op. Conflicting libraries
are rejected before copying anything. Every dependent plugin uses the same
SONAME and the APK must contain one copy per ABI. A separate common ZIP is not
automatically selected by Fusion merely by being placed in Data/Runtime/Android;
use this staging step for the exported project. Automatic common-dependency
selection by the proprietary Fusion exporter has not been established here.

## iOS

`build_ios.py` builds a **dynamic** OFEEncryption.framework for device arm64 and
simulator arm64/x86_64, then combines them into OFEEncryption.xcframework.
Only it links libcrypto; Fini.xcframework contains the adapter without crypto.
Add and embed/sign the shared framework once in the final app; link all plugins
against that same framework. Do not merge libcrypto into each plugin archive.
The common library exposes the stable Encryption_* C ABI and keeps EVP symbols
private through an export list. Its install name is
`@rpath/OFEEncryption.framework/OFEEncryption`.

```sh
python3 Extensions/_DeLib/Encryption/Mobile/build_ios.py \
  --openssl work/mobile-encryption/openssl/ios \
  --output work/mobile-encryption/ios --prepare-openssl
```

Source preparation is shared too: prepare_openssl.py pins OpenSSL 3.6.5 and its
SHA512 to the repository's vcpkg revision
9a5dfef5d1a0438a7d24856a85bd60f309d355e9. It prepares target dependencies, never a
second encryption algorithm. Automatic source builds need Linux/macOS, Perl and
make. Windows publishers can use prebuilt target OpenSSL prefixes via --openssl.
