# Shared Android encryption runtime

`Android_Gradle/app/src/main/cpp/Encryption/CMakeLists.txt` defines the CMake
target `ofe_encryption`, which builds `libofe_encryption.so`. Its OpenSSL symbols
are private, and only Encryption_* functions are exported. The shared build cache
is `work/mobile-encryption/android/<ABI>` and is reused by plugin publishers.
The supplied Fini publisher builds/imports this common dependency, then links its
own libfini.so against it. It does not statically link encryption or libcrypto.

`Android_Gradle/cmake/Encryption/android_runtime.py` creates its own
`ToInstall/OFEEncryption.zip`, containing one
common library and matching libc++_shared.so per ABI, shared source/license
material, ABI version and SHA256 manifest. Plugin ZIPs must not contain these
common runtime binaries or their implementations. The client header is allowed.

For an exported Android project, stage the shared package **once**:

```sh
python3 Extensions/Android_Gradle/cmake/Encryption/android_runtime.py \
  --archive Extensions/Android_Gradle/cmake/Encryption/ToInstall/OFEEncryption.zip \
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

The C++ implementation remains in `_DeLib/Encryption`. Dependency preparation
uses `Extensions/_Script/Encryption/prepare_openssl.py`; no cipher is rewritten.
