# Fini Android JNI port

## Architecture

Android development files live in this Gradle tree: `app/src/main` for the
Java/JNI adapters, `cmake/Fini` for native/toolchain configuration and
`tests/fini` for validation and the ACE contract. The portable C++ Runtime is
enabled by `FINI_PORTABLE_RUNTIME` in the original `../Fini/IniCore.h`;
iOS development and Mac handoff inputs live in
`../iOS/Fini`. `../Fini` remains the Windows VS project and release staging area.

Android uses `CRunFini.java` -> `FiniNative.java` -> JNI `libfini.so` -> the shared
C++ implementation. Java handles only Fusion parameters, runtime objects, events,
file/resource resolution and lifecycle. INI storage remains SimpleIni. Windows
and mobile share `Fini/IniCore.h`; native codecs reuse `_3rdLib/Base64.h`,
`Compress/zlibInterface.h`, `_DeLib/StrNum.h`, and the existing
`Encryption/Core.h` + `Encryption/KeyGen.h`. There is no Java encryption or INI engine.
JNI native objects have validated registry handles and explicit, idempotent cleanup.
Requests belong to Fusion's runtime thread. Java UTF-16 is converted explicitly,
including supplementary characters and embedded NUL in string codec operations.

Encryption uses the original `_DeLib/Encryption` sources. Android integration
lives in `app/src/main/cpp/Encryption` and `cmake/Encryption`; iOS integration
lives in `../iOS/Encryption`. Fini includes only CAPI.h and calls the existing
Encryption_* C ABI. Android links libfini.so against libofe_encryption.so. iOS
links Fini's adapter archive against a single dynamic OFEEncryption.framework.
Only the common runtime links libcrypto. See the shared module's README for the
one-time exported-app installation and framework embed/sign steps.

For iOS, CRunFini.mm uses Objective-C++ and the same C++ core; JNI is Android only.
The generated standalone `../iOS/Fini/Fini.xcodeproj` can be opened directly in Xcode.
Its FiniXCFramework scheme builds the adapter and prepares the separate shared
runtime. SDK sources are referenced locally and are never copied into plugin ZIPs.

## Windows compatibility and reviewed shared changes

The original condition IDs 0..3, action IDs 0..19 and expression IDs 0..10 are
preserved; `tests/fini/ACE.csv` records parameter order/types and generated events. Version 2/3 edit properties and keep-over-frame state are supported.
Object serialization uses Win32's 4-byte flags, 16-byte numeric CValue records,
little-endian position integers and NUL-terminated UTF-16LE strings, with the
target object's value/string counts. Invalid or mismatched object blobs are
rejected before changing the target. Runtime object fields themselves are never
reinterpreted as Windows structures. Numeric parsing uses the existing `_stof`.

Encryption reuses legacy Win32 key derivation, AES-128-CBC and PKCS#7, with raw
ciphertext and no new header. Empty passwords mean plaintext. Win64-generated
short-password assets require explicitly different derivation and are not
silently auto-detected. INI and Base64 strings are UTF-8. SimpleIni emits native
line endings (LF on mobile, CRLF on Windows); both load on either platform.

Shared changes in the PR for author review:

- Extracted SimpleIni create/set/delete/copy/save operations to `IniCore.h`.
  Windows create/set/delete/copy now call that code too.
- Narrowed `StrNum.h`'s include from the umbrella to its actual StringUtilities
  dependency so native builds do not require iconv for numeric parsing.
- Narrowed zlibInterface's include to PathUtilities, guarded MSVC linker pragmas,
  and added an optional output bound plus zero-size/growth checks. The existing
  four-argument API remains valid; mobile sets a 64 MiB decompression limit.

Intentional defensive differences: uninitialized access is safe; malformed
Base64/object data produces an empty/false result and Android `lastError` (iOS
logs the error); failed file loads retain existing INI state. Failed auto save
stops data replacement instead of discarding modified data. Release suppresses
auto save. A destroyed localization link falls back to the original text.
Android relative output names resolve under the app's files directory. Reads use
Fusion's `openHFile`, including embedded resources; writes require filesystem
paths, not content-provider URIs. iOS uses `getPathForWriting`/`loadResourceData`.

## Android developer publishing

Python 3.9+, JDK compatible with the installed exporter, CMake 3.22+, Ninja and
NDK 28+ are required. The existing Android development configuration selects
NDK 28.2.13676358. The standalone Fini publisher builds native API 26 and defaults
to armeabi-v7a, arm64-v8a, x86 and x86_64. Supply an OpenSSL static/PIC build for
each ABI at `<openssl-root>/<ABI>/include` and `lib/libcrypto.a`, built for the
same native API/STL. Put OpenSSL's LICENSE.txt at that root, or pass --openssl-license. The publisher
includes it and the NDK NOTICE with the redistributed libraries.
No host Linux crypto library may be used in an Android build.

```sh
python3 Extensions/_Script/Fini.py --ndk /path/to/ndk/28.2.13676358 \
  --openssl /path/to/android-openssl --cmake /path/to/cmake --ninja /path/to/ninja
```

`Fini.bat` is the Windows launcher for the same publisher. VS `Run_Only Unicode`
invokes it after Build, including Java-only changes; Release and Debug remain
Windows builds. Toolchain settings can also use ANDROID_SDK_ROOT/ANDROID_HOME,
ANDROID_NDK_HOME and FINI_OPENSSL_ROOT. Optional `--fusion <Fusion-install>`
installs into that instance only after successful compilation/validation.
The publisher preserves `work/fini-android/publisher.log` and fails on missing
build tools or target OpenSSL; it does not substitute stale native libraries.

Outputs:

- `Extensions/Fini/ToInstall/Files/Data/Runtime/Android/Fini.zip`
- `Extensions/Fini_Release.zip`

The runtime ZIP owns the two Java classes, each selected ABI's libfini.so,
additive JNI/R8 rules and the plugin's source/license material. It carries a
manifest dependency on OFEEncryption/1, not a private crypto or STL implementation.
The separate common package is
`Extensions/Android_Gradle/cmake/Encryption/ToInstall/OFEEncryption.zip`.
Stage it once in an exported Android project using the common module's installer
or the publisher's `--android-project` option. Automatic dependency selection by
Fusion itself has not been established; merely copying the common ZIP into
Data/Runtime/Android is insufficient evidence of integration.

NDK discovery/preparation follows FTW's workflow: explicit paths, SDK environment
variables and local.properties, platform defaults, SDK/VS/PATH CMake/Ninja, then
sdkmanager for missing pinned components. If Command-line Tools are absent, the
publisher can fetch the pinned revision using Google metadata and its checksum.
Run `Fini.py --prepare-only` to verify tool readiness. Existing SDK licenses are
respected; first-time installations need the SDK's license setup. Build logs and
Android user caches stay under work/. On Unix, absent target OpenSSL prefixes are
prepared from pinned source by the common library's script. Native Windows builds
can supply ABI-specific OpenSSL with --openssl.

64-bit native libraries are checked for 16 KiB load alignment; 32-bit NDK STL
libraries retain their shipped 4 KiB alignment. Source/SDK Runtime/Gradle template
replacements are excluded. Fusion's ordinary APK export does not compile this C++.

## iOS developer build (separate workflow)

The ready project and instructions are in `../iOS/Fini/README.md`. On your Mac:

```sh
python3 Extensions/iOS/Fini/generate_project.py --runtime /path/to/exported-project
open Extensions/iOS/Fini/Fini.xcodeproj
```

Select FiniXCFramework and Product > Build. It uses Xcode 15+/iOS 17+, the licensed
Runtime headers, and the shared OpenSSL preparation script. An explicit target
OpenSSL prefix can be configured with --openssl; otherwise a common cache is used.
Outputs are `work/fini-ios/Fini.xcframework` (adapter, no crypto) and
`work/mobile-encryption/ios/OFEEncryption.xcframework` (shared dynamic runtime).
Embed/sign OFEEncryption once in the consuming app; add Fini's library and retain
its class with -ObjC. Link libc++, Foundation and libz. Other plugins use the same
framework. Do not merge libcrypto into individual plugin archives.

Runtime sources live in iOS/Fini/Runtime; licensed SDK sources stay local.
Windows creates and round-trips Fini.ext using the shared codec
from PR #100. The exported configurator links native archives and embeds the
common framework once. Apple compilation, Fusion export/final-link and device
validation remain separate checks on the author's Mac.

## Validation performed in this environment

```sh
export JAVA_HOME=/workspace/java-toolchain/usr/lib/jvm/java-21-openjdk-amd64
export PATH="$JAVA_HOME/bin:$PATH"
python3 Extensions/Android_Gradle/tests/fini/run.py \
  --cmake /workspace/onboarding-venv/bin/cmake --json work/fini-json
python3 Extensions/Android_Gradle/tests/fini/test_publisher.py
python3 Extensions/Android_Gradle/tests/object_helpers/run.py
git diff --check
```

The test-only JSON-java 20240303 sources were fetched from stleary/JSON-java into
work/fini-json; Android uses its built-in org.json. The host runner can instead
accept a local JSON-java JAR. Runtime test doubles exercise the actual production
Java adapter with the actual JNI dynamic library and SDK CValue source.

Passed: shared-core INI/UTF-8/case lookup, numeric values, section copy/delete,
Base64/zlib, invalid and truncated inputs, Windows object/position formats,
auto save/release, localization, all 20 actions/11 expressions/4 conditions,
Unicode/NUL JNI strings, invalid/closed handles, cross-frame transfer, destruction
inside an iteration callback, exact decryption and ciphertext reproduction of
the existing NeoPic Windows sample. Two publisher tests cover SDK exclusion,
additive R8 rules, ABI/alignment rejection and output preservation on failure.
The unchanged object-helper suite passed 56 checks.

Windows helpers ObjectBase/ObjectSelection/ObjectCreation were compared with
reviewed revision 234676b7ed306f19a35ab6d69519a0ba6aa52598: no changes, so no helper
synchronization or reviewed-revision update was needed. Android's existing runtime
representations and documented platform differences remain intact.

The NDK and CMake were subsequently prepared through the new publisher flow.
Shared runtime and plugin target-ABI builds are validated separately from the host
tests. The final binary/package audit checks all four ABI outputs, DT_NEEDED,
shared API exports, absence of crypto definitions in libfini, package ownership,
and one-time common-runtime staging. Xcode project syntax was parsed with the
independent OpenStep parser 2.0.3. Generated IDs/references/schemes were also checked.
Not established here: Windows MSBuild, a full Fusion Android APK build, Apple
compilation/linking, real Fusion .ext export/final-link and device validation.

A review source bundle (not an installable runtime) can be created with:

```sh
python3 Extensions/_Script/Fini.py --source-bundle work/Fini-mobile-source.zip
```

## Mac build/push -> Windows pull/VS packaging

After a successful FiniXCFramework build, the scripts stage these owned artifacts:

- `Extensions/iOS/Fini/Prebuilt/Fini.xcframework.zip` and `manifest.json`.
- `Extensions/iOS/Encryption/Prebuilt/OFEEncryption.xcframework.zip`
  and `manifest.json`, shared once by all dependent plugins.

The author commits/pushes those four files after building on Mac. No script pushes
Git automatically. On Windows, git pull followed by VS Run_Only Unicode invokes
the existing Fini.bat/Fini.py entry point. It validates SHA256, required slices and
source freshness, then updates Fini-iOS.zip and the separate OFEEncryption-iOS.zip,
installs them when FusionRoot/the normal Fusion directory exists, and rebuilds
Fini_Release.zip. Windows does not need Xcode. To run just this handoff step:

```sh
python Extensions/_Script/Fini.py --ios-only
```

No Mac binaries are included until the author builds them. Before that first
build the publisher reports the missing handoff and leaves Apple outputs alone;
an incomplete, corrupt or source-stale committed pair fails instead of replacing
a known-good package. The handoff creates native XCFramework distribution ZIPs;
Windows also generates a format-verified Fini.ext with the shared PR #100 codec.
Actual exporter round-trip, final-link and device checks remain separate.
