# Android porting workflow

## 1. Discover the extension's build contract

Locate the target extension's ACE declarations, Java adapter, native sources, shared helpers,
publishing script, native build configuration, tests, and package/install destinations. Record
their paths before editing. Use existing build documentation when it agrees with the executable
scripts; resolve discrepancies rather than copying settings from an unrelated port.

The development SDK and installed `Data/Runtime/Android/RuntimeAndroid.zip` may be different
versions. Compare signatures, image-handle widths, and resource-generation tasks. For example,
an SDK may widen image handles from `short` to `int`; narrowing casts are not a general repair.

Create an ACE matrix covering IDs, parameter order, accessor APIs, results, events, and unsupported
features. Successful compilation of the switch statement alone does not establish compatibility.
For unavailable desktop surface/texture operations, consume parameters and return the agreed
failure or empty result. Define the resource state after failure; do not pretend the operation worked.

## 2. Port shared C++ dependencies

- Add required includes and verify headers without the Windows PCH.
- Replace Windows-specific storage types with semantic project types. Separate the current data
  model from Fusion's numeric event representation.
- Use `(std::min)(a, b)` and `(std::max)(a, b)` to avoid Win32 macro expansion.
- Implement platform differences in shared helpers. Consider `std::filesystem` for non-Windows
  path operations and iconv for required encoding conversions; Windows can retain its native APIs.
- Treat `CP_ACP` as a platform policy, not a universal encoding. Make the non-Windows fallback
  explicit and configurable; a UTF-8 default does not detect the system's active encoding.
- Return owned string/path values rather than shared static buffers. A compatibility conversion
  to `char*` must not outlive its owner or expose a dangling pointer from a temporary.
- Android's NDK has no public iconv API. If needed, supply target ABI headers/libraries and never
  link host Linux libraries into an Android target.
- Keep JNI-local CMake and tests in the Android build area. Porting does not require a wholesale
  reorganization of historical header-only code.

List shared C++ behavior changes for author review. Keep them out of the main branch until authorized.

## 3. Implement JNI and Runtime adapters

- Keep one algorithm implementation in C++. Java handles Runtime object selection/creation,
  parameters, events, and result marshalling.
- Give native objects explicit ownership and validated handles. An opaque `jlong` registry can
  retain ownership for each call so nested events cannot destroy an object still being used.
  Make Java cleanup idempotent and release resources during rebuild and destruction.
- Do not share `JNIEnv*` across threads or retain expired local references. Long-lived Java objects
  require paired global-reference cleanup, thread attachment, and class-loader awareness.
- Handle Java UTF-16 explicitly, including supplementary characters and embedded NUL. Windows
  and Android use different `wchar_t` widths. `NewStringUTF` is not a general Unicode conversion API.
- Do not let C++ exceptions cross the JNI ABI. Preserve pending Java callback exceptions rather
  than swallowing them and continuing unsafe JNI calls.
- Check declarations, export/registration names, parameter widths, and return types. Batch
  marshalling can reduce JNI overhead without moving algorithms into Java.
- Keep native classes and name-resolved callbacks stable under R8. One additive packaging method
  is `app/libs/<extension>-JNI-rules.jar` containing `META-INF/proguard/<extension>.pro`.

## 4. Choose and discover native tools

Derive versions from the target extension and installed Runtime, not from a reference extension.
Record the following separately:

| Setting | How to choose it |
| --- | --- |
| Publisher Python version | Requirements of the publisher's language features and libraries |
| JDK/JBR | Compatibility with the Runtime's Gradle and Android Gradle Plugin versions |
| Gradle / AGP | The installed Runtime's supported configuration |
| NDK | C++ language/library requirements, ABI compatibility, and applicable page-size requirements |
| CMake / Ninja | The project's minimum version and available compatible installations |
| Native Android API | Required native APIs and supported devices; distinct from compileSdk |
| Supported ABIs | The extension's distribution targets and Fusion export settings |

For SDK discovery, check explicit configuration and environment variables such as `ANDROID_HOME`
and `ANDROID_SDK_ROOT`, then documented platform defaults such as `%LOCALAPPDATA%/Android/Sdk`.
Search installed SDK tools, VS-bundled tools, and PATH before requiring a new installation.
VS may put CMake and Ninja in sibling directories rather than a shared bin directory.

If dependencies are missing, use sdkmanager where available. Command-line Tools themselves may
be absent. Check license/setup requirements and verify tools after installation; an attempted
installation is not evidence of a usable toolchain. Report actionable failures.

Configure and incrementally build each ABI in Release mode with an explicit Android toolchain,
native API, optimization settings, and required ELF alignment. Stop publishing on failure rather
than substituting an old `.so`. If using shared STL, include a compatible `libc++_shared.so` and
check coexistence with other extensions. Do not assume arbitrary NDK STL versions are interchangeable.

## 5. Build the extension package

```text
Developer's VS RunOnly / publishing entry point
  -> verify dependencies and corresponding source
  -> compile shared C++ and JNI for each supported ABI
  -> collect owned Java, jniLibs, resources, consumer rules, and license material
  -> write manifest, create ZIP, install, and archive
Fusion APK export
  -> combine its Runtime with extension ZIPs
  -> use default Gradle to compile Java, link resources, package libraries, and sign
```

Define Java ownership explicitly: the extension's entry classes and its own helper namespaces.
When following imports, stop at SDK-owned Runtime, Banks, Objects, and built-in extension classes.
The existence of a local `.java` file is not permission to redistribute it or replace the installed Runtime.

Typical layout:

```text
app/src/main/java/Extensions/CRun<Extension>.java
app/src/main/java/<owned-helper-package>/...
app/src/main/jniLibs/<ABI>/lib<extension>.so
app/src/main/jniLibs/<ABI>/libc++_shared.so  # when using shared STL
app/libs/<extension>-JNI-rules.jar
<required licenses, corresponding source, relinking material, and manifest>
```

Owned native/CMake sources and shared headers may accompany the package for inspection or
relinking. Exclude SDK content and build caches. Account for Git line-ending conversion in text
hashes: pin LF where needed and permit only the intended text normalization. Validate archives
and libraries byte-for-byte. Pin downloaded dependencies, verify hashes, and satisfy their licenses.

Clean staging first, validate all ABI outputs, then atomically replace the output ZIP. Do not
override default `app/build.gradle`, `settings.gradle`, application ProGuard configuration, or the
Runtime archive: that would make one extension own the application's dependencies, generated
resources, signing, obfuscation, and AAB configuration.

In this repository, invoke automatic Android publishing from RunOnly, leaving Release for the
Windows editor and Debug for Windows development. Handle VS fast up-to-date checks so Java,
shared-header, or dependency changes still reach the publisher. Quote paths, propagate exit codes,
avoid interactive pauses in VS, and capture subprocess output in the publisher log.

## 6. Validate and document the new port

1. Compare core results with an independent oracle or established implementation, using domain-
   appropriate cases rather than merely asserting the program did not crash.
2. Exercise JNI encoding, ownership, exceptions, reentrant callbacks, and all ACE contracts.
3. Package from a full SDK checkout and prove SDK sources stay excluded. Verify failure behavior
   for missing libraries, invalid hashes, and native compilation errors.
4. Inspect the actual ZIP and APK, including every supported ABI and all runtime dependencies.
5. Build against the installed Fusion Runtime's default templates without undocumented local flags.
6. Test device objects, collisions, hotspots, borders, scrolling, and layers. Fixtures validate
   boundary calls; they cannot establish correct rendering on a device.
7. Write extension-specific build instructions from the commands actually used. Record versions,
   output locations, evidence, and remaining work. Attribute author-reported device checks accurately.

## Optional repository example

The FindTheWay port illustrates these steps but is not a prerequisite for another port. Useful
implementation examples include `Extensions/_Script/FindTheWay.py`, the native sources and tests
under `Extensions/Android_Gradle/`, and `Extensions/_DeLib/GeneralDefinition-Portability.md`.
Adapt their structure to the target extension; do not reuse FTW artifact names, ACE IDs, algorithms,
dependency lists, exact tool versions, or commands without checking applicability.

## External references

- [DarkEDIF build configuration](https://github.com/SortaCore/MMF2Exts/blob/master/DarkEdif/Lib/FusionSDK.props): native development builds and post-build packaging.
- [DarkEDIF Java wrapper](https://github.com/SortaCore/MMF2Exts/blob/master/DarkEdif/Bluewing%20Client/CRunBluewing_Client.java): loading with `System.loadLibrary`.
- [Historical official Android SDK](https://github.com/ClickteamLLC/android): native templates and packaging history. Its Ant, old ABI, and assets/mmf instructions must not be copied into a modern Runtime unverified.
- [Gradle Java compatibility](https://docs.gradle.org/current/userguide/compatibility.html): match the chosen JDK to the actual Gradle version.
