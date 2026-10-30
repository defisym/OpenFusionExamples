# FindTheWay Android JNI port

Architecture: Fusion Java ACE dispatch -> JNI parameter/result marshalling -> the shared
`_DeLib/FindTheWay.h` and `_DeLib/AOE.h` implementations.

Pathfinding, map costs, line/movement/attack ranges, ZOC, AOE, isometric conversion, and caching
run in C++. Java `FindTheWayClass` / `AOEClass` provide marshalling and resource management.
Object selection, creation, Fusion parameters, and events use the mobile Runtime's Java APIs.
The shared core remains header-only; there is no separate Android algorithm implementation.

This document describes this extension. For another port, derive commands, dependencies, and
paths from that extension's own build configuration and publisher.

## Review and validation status

The changes separate shared helpers, core fixes, map data, and Android JNI for review. Shared
C++ behavior changes affect Windows and require author review before merging. Host checks,
four-ABI Android compilation, and real Fusion Runtime Debug APK builds have passed.
The author also confirmed Windows publishing/build success and reported successful Android
testing after the backdrop-border correction. This feedback is not exhaustive device or API coverage.

Platform differences in shared helpers are concentrated in `GeneralDefinition.h`. The
`DeLib::Helpers` CMake interface supplies include paths and non-Windows iconv dependencies.
See [GeneralDefinition-Portability.md](../_DeLib/GeneralDefinition-Portability.md).

## Fusion interface

Public IDs match `Extensions/FindTheWay/Main.h`: 29 conditions, 21 actions, and 20 expressions.
Condition 28 is OnIterateMap; action 20 is IterateMap. Isometric grid sizes use the current C++
encoding: width in the low 16 bits and height in the high 16 bits. Map/path iterations snapshot
coordinates before invoking events, allowing nesting; path iteration updates the step index,
while map iteration updates only coordinates, matching C++.

Conditions 12 (image-file map), 26 (Active texture map), and 27 (desktop surface-pointer map)
consume their parameters, release the old map, and return false. They do not report false success.
Condition 2 supports event-driven collision-map generation. Direct mobile collision APIs,
backdrop generation, selection, and creation retain Java Runtime integration; host API fixtures
do not establish their behavior on a device.

## Lifetime, callbacks, and coordinates

Each map has an opaque `long` handle. A JNI registry owns C++ `shared_ptr` instances and each call
temporarily retains ownership, so a nested event may release a map without invalidating the
active call. Java `close()` is idempotent; rebuilding and destroying the extension release resources.
No `JNIEnv*` or Java-owner local reference is retained across calls. Callbacks execute synchronously
on the calling Fusion thread.

ProGuard/R8 consumer rules preserve JNI callback names. Java callback exceptions remain pending;
C++ exceptions are converted to Java exceptions rather than crossing the JNI ABI. String conversion
explicitly handles UTF-16 and platform `wchar_t` widths, including supplementary characters and
embedded NUL; it does not rely on `NewStringUTF` as a general Unicode conversion.

The main grid image uses the cell center as its hotspot anchor. Right/bottom border anchors
include each frame's hotspot difference from the main frame, so Runtime hotspot handling leaves
all three images aligned to the Windows reference origin. There is no DEBUG coordinate switch.
Application edge offsets and outer-boundary rules are preserved.

## Build and publish

Build the VS `FindTheWay.vcxproj` **Run_Only Unicode** configuration, or run:

```bat
Extensions\_Script\FindTheWay.bat
```

Both invoke the Python publisher and create
`Extensions/FindTheWay/ToInstall/Files/Data/Runtime/Android/FindTheWay.zip`, then install/archive
according to the publisher's configuration. Python 3.10+ is required; no pip or 7-Zip is needed.
Initial dependency/source downloads need network access. Release Unicode builds only the
Windows editor extension; Debug Unicode does not publish Android automatically. RunOnly reaches
the publisher even when Windows output is already up to date. Set the MSBuild property
`/p:FindTheWayPackageAndroid=false` to skip Android publishing for a RunOnly build.

The publisher prebuilds all supported JNI ABIs. Fusion's APK export compiles Java, links resources,
and packages libraries using its own templates. The extension ZIP does not replace
`app/build.gradle`, `settings.gradle`, or the application's ProGuard file, and does not modify
the installed `RuntimeAndroid.zip`.

### Tool discovery

SDK lookup checks `ANDROID_HOME`, `ANDROID_SDK_ROOT`, then `%LOCALAPPDATA%/Android/Sdk` on Windows.
For a nonstandard installation, point `ANDROID_HOME` at the SDK configured in Fusion.
The current publisher uses NDK r28c `28.2.13676358` and prefers SDK CMake `3.22.1`. If unavailable,
it searches other SDK versions, VS-bundled CMake/Ninja, and PATH for compatible CMake >= 3.22.
Missing packages can be installed through SDK Command-line Tools / sdkmanager, subject to network
access and accepted SDK licenses. Missing installation tools or failed setup produce explicit errors.

CMake builds the shared C++20 core with Release optimization, native Android API 21, and 16 KB
ELF alignment. Every publish configures/builds all ABIs incrementally; caches live under
`Android_Gradle/_NativeBuild`. Shared-header changes use the same publishing entry point.
The APK build's JDK, compileSdk, targetSdk, and minSdk remain part of Fusion's application setup.

### Package contents

- `app/src/main/java`: the extension adapter and owned helpers only.
- `app/src/main/jniLibs/<ABI>/libfindtheway.so` and `libc++_shared.so` for arm64-v8a,
  armeabi-v7a, x86, and x86_64.
- `app/libs/FindTheWay-JNI-rules.jar`: JNI keep rules under `META-INF/proguard`, discovered by R8
  without replacing the application's global rules.
- Owned JNI/CMake sources, recursively collected shared headers, iconv dependencies, corresponding
  source, and licenses for inspection/relinking.
- A per-file SHA-256 manifest.

Fusion retains its Material dependencies, generated resources, signing, obfuscation, and AAB
configuration. Native extensions sharing libc++ must use compatible runtimes.

Java collection includes explicit extension entry points plus owned `_DeLib` / `_3rdLib` helpers.
SDK Runtime, Banks, Objects, and built-in extension sources belong to the installed Fusion and
must never be included merely because they exist in the development checkout. The publisher copies
owned native/CMake directories; new compilation units must still be declared in CMakeLists.txt.

Pinned four-ABI `libiconv.a` / `iconv.h` files live in `_3rdLib/iconv-android`. Publishing checks
SHA-256, tolerating CRLF checkout only for headers while validating binaries byte-for-byte.
Corresponding source is downloaded, verified, cached, and packaged with license/relinking material.
No host Linux libraries or private Fusion SDK sources are published.

`FusionRoot` overrides the default Fusion install location. Without an installed target directory,
the ToInstall package is still produced. Failures return nonzero and are logged to
`Android_Gradle/FindTheWay-package.log`; standalone batch runs pause on failure, while VS calls do not.
Maintain this publisher whenever sources, dependencies, or build configuration change, and verify
the actual resulting ZIP/APK rather than only manually copied files.

## Verification

Host checks require Linux, JDK 17+, GCC 14+, CMake 3.22+, and Python 3, with `JAVA_HOME` and tools configured:

```sh
python3 Extensions/Android_Gradle/tests/run_host_tests.py --build-dir /tmp/findtheway-host --sanitize
python3 -m unittest discover -s Extensions/Android_Gradle/tests -p test_publisher.py -v
```

Use `--core /path/to/proposed/Extensions/_DeLib` to test isolated shared-header proposals.
Checks compare C++/JNI paths, ranges, ZOC, AOE types/directions, and isometric coordinates; include
200 weighted-map Dijkstra comparisons, serialization failures, cache/force-find modes, Unicode,
reentrant/exception callbacks, and repeated creation/release. ACE tests use explicit Runtime API
fixtures; backdrop tests compare image origins after per-frame hotspot application, including
internal obstacles and outer boundaries. These fixtures do not render a device screen.
ASan/UBSan checks run separately; LSan was disabled in the recorded ptrace-limited environment.

Four-ABI APK builds passed with JDK 21, Gradle 8.14.3, AGP 8.11.1, compileSdk 35, NDK r28c, and
CMake 3.22.1 in the recorded environment. The original Runtime Gradle files were restored byte-for-
byte and the extension was supplied through prebuilt jniLibs. The APK contained both required
libraries for all ABIs, and Material3 resource linking passed. Other installed Runtime dependency
versions may require a newer Build API; the author's core 1.17 setup required API 36.
Private Runtime/SDK/test-application sources are not committed to the public repository.

## Future iOS work

An Objective-C++ `.mm` adapter can use the same shared C++ headers without JNI. iOS requires its
own Runtime ACE, object, event, build, and packaging checks. Android success does not establish
an iOS port, and no second algorithm implementation is needed.
