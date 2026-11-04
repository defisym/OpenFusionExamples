# Shared mobile encryption runtime

The existing `_DeLib/Encryption/CAPI.cpp`, Core.h and KeyGen.h implement the
algorithms. This directory contains shared dependency-preparation tooling;
Android integration belongs to Android_Gradle and Apple integration to iOS.
Clients use CAPI.h; only this runtime links libcrypto. C ABI version is 1.

## Dependency preparation

Source preparation is shared too: prepare_openssl.py pins OpenSSL 3.6.5 and its
SHA512 to the repository's vcpkg revision
9a5dfef5d1a0438a7d24856a85bd60f309d355e9. It prepares target dependencies, never a
second encryption algorithm. Automatic source builds need Linux/macOS, Perl and
make. Windows publishers can use prebuilt target OpenSSL prefixes via --openssl.

Platform build/package integration lives in its own project:

- Android: `Extensions/Android_Gradle/cmake/Encryption/README.md`.
- iOS: `Extensions/iOS/Encryption/README.md`.
