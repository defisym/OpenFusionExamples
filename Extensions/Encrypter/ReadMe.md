# ReadMe

En/decrypt files through the shared C++ OpenSSL core

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/Encrypter/ToInstall/Files/Help/Encrypter/Encrypter.md)

## Android status — shared core reviewed, extension runtime deferred

Reviewed 2026-11-04 against master `36812170` (portable encryption, PR #91).
Windows and mobile consumers can now use the same C++ OpenSSL EVP implementation,
`Encryption/KeyGen.h`, C API and seekable `DecryptReader`.

FFMpeg and SDL_Mixer use a thin `MediaCrypto` JNI binding to that original core.
Java owns Fusion file access, private-cache materialization and source leases;
it no longer implements password expansion, AES buffering or padding. JNI supplies
UTF-16 units and explicitly selects the 32-bit seed used by existing Fusion Win32
assets, independently of Android's process pointer size.

The shared core and JNI tests restore the supplied old encrypted JPEG byte for
byte. They cover empty/short/long/Unicode keys, NUL/surrogate units, chunk/padding
boundaries and failures. Android API 21 crypto JNI cross-builds use all four ABIs
and the OpenSSL version selected by Windows (`EncryptVersion.cmake`). The API
21–23 32-bit file-I/O compatibility branch preserves the other platforms' paths.
A full licensed Fusion APK and new on-device encryption validation remain outstanding.

The standalone Encrypter Fusion extension (its ACE bindings and export package)
remains deferred; sharing its core with media plugins is not that extension port.
Do not invent a separate mobile crypto implementation to work around a core
platform dependency. Identify the dependency and refactor the shared core or its
platform adaptor first.

See the [Android media guide](../Android_Gradle/Media-JNI.md).
