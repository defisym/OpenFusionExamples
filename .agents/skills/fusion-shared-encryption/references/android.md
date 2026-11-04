# Android shared encryption

Use alongside fusion-android-porting and android-runtime-parity. Keep Fusion object
access/events in Java and algorithms in C++; use JNI to call the plugin native core.
The plugin library imports libofe_encryption.so through the C ABI. Only that common
library links libcrypto. Reuse
`Android_Gradle/app/src/main/cpp/Encryption/CMakeLists.txt` and
`Android_Gradle/cmake/Encryption/android_runtime.py`; both consume the original
Encryption sources. Keep Android adaptations in its platform project.

Use one common build/cache/output across plugins. Emit a separate OFEEncryption.zip
with matching libc++_shared.so, licenses, ABI/version and hashes. Client ZIPs own only
their JNI library/Java/resources and an explicit dependency; no common implementation
or duplicate crypto binaries. Do not replace application Gradle/ProGuard/Runtime files.
Stage the common dependency once in the final exported app, with conflict/hash checks.
Do not assume Fusion includes an arbitrary common ZIP just because it is installed.

Follow the target publisher's SDK/NDK/CMake/Ninja discovery and sdkmanager preparation
flow; search installed SDK/VS/PATH tools first. Keep work/cache/logs outside commits.
Check actual readiness after installation. Preserve proxy and certificate settings.

Inspect DT_NEEDED/SONAME and dynamic symbols: the client imports Encryption_* from
the common library and defines no encryption/EVP implementation. The shared library
exports its C ABI and hides EVP symbols. Inspect each ABI's final package and APK,
including one compatible STL. Apply 16 KiB checks to supported 64-bit Android targets;
do not misreport the NDK's 32-bit 4 KiB STL as a broken 64-bit build.
