# Android porting troubleshooting

Read the newest build attempt first. Publisher and Gradle daemon logs may append multiple runs;
an old error at the beginning is not the current failure. Establish whether extension publishing
failed or Fusion's subsequent APK build failed.

## Diagnose the failing layer

| Symptom or evidence | Inspect | Response |
| --- | --- | --- |
| VS MSB3073 / nonzero batch exit | Latest publisher exception and subprocess output | Fix the dependency/native build failure; do not suppress the exit code. |
| VS succeeds but Fusion produces no APK | Installed extension ZIP, Fusion JDK/SDK, Gradle log | Confirm the publisher installed into the Fusion instance actually being used. |
| A blank application also fails | Shared Fusion toolchain and Runtime | Diagnose the common failure before changing extension algorithms. |
| `Unsupported class file major version 69` | Java home/version and Gradle version in the log | Java 25 was incompatible with Gradle 8.14.3 in the observed case; an independent JDK/JBR 21 resolved it. Check the compatibility table for other versions. |
| `checkDebugAarMetadata` requires a newer API | compileSdk and resolved AndroidX versions | Raise Fusion's Build API to satisfy the actual dependency; core 1.17 required API 36 in the observed case. |
| Missing `Theme.Material3.DayNight.NoActionBar` | Material dependency and extension Gradle overrides | Restore the Runtime's default build configuration instead of maintaining an obsolete replacement. |
| Many incompatible Runtime Java methods/fields | SDK sources accidentally included in extension ZIPs | Restrict package ownership. Do not patch mixed SDK versions one error or narrowing cast at a time. |
| Missing SDK CMake and sdkmanager | VS, other SDK installations, PATH CMake/Ninja | Reuse compatible tools; provision missing tools only when necessary. |
| Windows iconv header hash mismatch | Missing file versus content mismatch; LF/CRLF | Permit only intended header normalization; retain byte-exact validation for binaries. |
| Device `UnsatisfiedLinkError` | Actual APK ABI, library names, dependencies, JNI symbols | Check jniLibs, `System.loadLibrary`, libc++, registration/export names, and R8 rules. |
| Half-cell shift only with nonzero hotspots | Anchors, hotspots, final image origins, drawing paths | Compute final positions; do not hide the issue with a DEBUG coordinate switch. |
| Main image aligns but obstacle borders shift | Different frame hotspots and common reference origin | Translate border anchors by their hotspot difference from the reference frame. |

## Locate logs and inspect versions

- Find the target publisher's log from its script or build output; do not assume a different
  extension's filename. Check the last error, native build results, package creation, and installation.
  A created ZIP does not prove the correct Fusion instance received it.
- Gradle daemon logs normally live under
  `%USERPROFILE%/.gradle/daemon/<version>/daemon-<pid>.out.log`, or under `GRADLE_USER_HOME` when set.
  Match timestamps and project paths to the failing export.
- Read the temporary project path from the actual error. Do not assume every Fusion release
  preserves temporary projects or provides an export/keep-project option.
- If no relevant Gradle log exists, check whether Java/Gradle started before changing C++ code.

An IDEA update can replace the bundled JBR's major version without changing its path. Keeping
Fusion's compatible JDK/JBR in an independent directory isolates it from IDE updates. Use the
Runtime's actual Gradle compatibility requirements; JDK 21 is a tested example, not a permanent rule.
Keep Java runtime, compileSdk, targetSdk, minSdk, and native Android API requirements distinct.

## Coordinate case study: compare final image origins

Let `C` be a cell center, `H0` the reference frame hotspot, `Hi` a border frame hotspot, and `D`
the application-specific offset. When Windows draws surfaces at a shared origin, the expected
image top-left is:

```text
P = C - H0 + D
```

For an Android drawing path that subtracts each image's hotspot, `P = A - Hi`. Pass:

```text
A = C + Hi - H0 + D
```

The main frame has `Hi == H0` and can use `C` directly. Different border hotspots require the
delta. For a 32-pixel cell, a main hotspot of `(16,16)` and border hotspot of `(0,0)` produce a
`(16,16)` border displacement if both receive the same unadjusted anchor. Adding the same
hotspot compensation to every frame is also incorrect.

Cover these cases:

- All-zero, equal centered, centered-main/zero-border, and asymmetric mixed frame hotspots.
- Internal obstacles and outer map boundaries, including when right/bottom offsets apply.
- Scrolling, layer visibility/offsets, immediate paste, and subsequent redraw paths.
- Raw anchors, final image origins, dirty rectangles, and collision sampling as separate quantities.

An earlier regression check accepted equal raw anchors and missed the rendered border mismatch.
The corrected check compared origins after per-image hotspot application against the Windows
reference. Device screenshots supplied the visual comparison; no shared C++ coordinate change
was needed. Re-establish these coordinate contracts for each new extension and Runtime version.
