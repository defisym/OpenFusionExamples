# ReadMe

En/decrypt file with WinAPI CNG

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/Encrypter/ToInstall/Files/Help/Encrypter/Encrypter.md)

## Android status — deferred pending refactoring

Recorded 2026-11-01: pause further Encrypter Android work until the refactoring
is complete. Resume on a subsequent request; this is not a completed extension port.

- The standalone Encrypter extension and its Fusion encrypt/decrypt actions have
  not been ported to Android.
- FFMpeg and SDL_Mixer currently use `MediaSource.java` to reproduce Win32
  `Encryption::GenerateKey()` key/IV derivation and decrypt media with Android
  `Cipher` (AES-128-CBC, AES-compatible PKCS5/PKCS7 padding). They do not depend
  on an Encrypter runtime object.
- Existing checks cover derivation vectors and generated encrypted samples,
  including Android playback. Direct interoperability tests using the actual
  Windows Encrypter have not been run.

After refactoring, review reuse of the shared encryption implementation before
continuing the standalone Android port. Verify actual Windows-generated files
on Android and, once Android encryption exists, the reverse direction; include
short/long/Unicode keys, padding boundaries and invalid-key/ciphertext behavior.

See the [Android media guide](../Android_Gradle/Media-JNI.md#deferred-encrypter-work).
