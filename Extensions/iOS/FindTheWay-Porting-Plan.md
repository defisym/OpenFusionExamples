# FindTheWay iOS port: investigation and proposal

Implementation milestone: `FindTheWayCore/` now contains a static C++20 library with
C-compatible map, path, serialization and coordinate APIs, C caller tests, and an Apple Silicon-only
XCFramework build script. Linux host checks passed against isolated current-master
headers. Owned Runtime helpers have an initial implementation under `Helpers/`.
Apple builds, full ACE integration, and Android helper synchronization remain
pending; see `FindTheWayCore/README.md` for exact scope and helper findings.

DarkEdif follow-up: upstream documentation and generated adapters confirm that
an encoded `.ext` can carry precompiled XCFrameworks plus thin Runtime sources.
Xcode references may still require adding after export. See `DarkEdif-Reference.md`;
this supersedes the earlier uncertainty about whether that delivery model exists.
Our own encoder/export round trip remains unverified.

Build layout: root `iOS/FindTheWay/` owns the CMake entry project and Mac CLI
producer. Windows RunOnly defaults to its `Artifacts/Current` handoff directory.
Native-only manifests are distinguished from full .ext release manifests.

## Baseline and evidence

Research date: 2026-10-30. The local checkout is `234676b7`; its Android code is obsolete.
The implementation baseline inspected through GitHub is master commit
`29edeed0ffb6929c6a1ca34423002c50ec4ddc0b`. Do not implement against the local Java-only snapshot.
Git fetch failed because the execution environment could not connect to its proxy;
remote files were inspected through the GitHub connector without changing local source files.

Inspected remote sources include the Android porting skill, its skill-authoring conventions,
`Android_Gradle/FindTheWay-JNI.md`, JNI implementation and CMake configuration,
`_DeLib/FindTheWay.h`, `FindTheWayFusion.h`, and `GeneralDefinition-Portability.md`.
The iOS adapter on remote master remains a placeholder. Local inspection covered
`iOS/SDK.xcodeproj`, `CExtLoad.m`, Runtime headers, the December 2014 SDK guide,
and the separately installed Proj/ProjFin exporter projects.

## Findings

- Android now invokes the shared header-only C++ core. Reuse `FindTheWay.h`, `AOE.h`,
  their dependency closure, and `FindTheWayFusion.h`. The latter preserves Fusion's legacy
  numeric cost interface over the current MapData model. Do not fork the algorithms or use
  the historical Java algorithm as the iOS implementation.
- The documented public interface is 29 conditions, 21 actions, and 20 expressions.
  Inventory IDs, parameters, return types, properties, and event behavior from current
  Windows definitions and the Android adapter before implementation. The local old Android
  code lacks map iteration and is not a compatibility baseline.
- `CRunFindTheWay.h/.m` declare the Runtime entry points but implement only one placeholder
  condition, action, and expression. Properties and resource lifetime are unimplemented.
- `CExtLoad.m` already imports and instantiates FindTheWay. The local Xcode project already
  lists its source in the Sources phase. Registration alone does not establish functionality.
- The local SDK application configurations force Objective-C++ input even for `.m` files;
  C++ settings are still C++11-era (`c++0x`), with a recorded iOS deployment target of 12.0.
  The shared core requires C++20. These settings are observed legacy defaults, not a decision
  about the port's supported Xcode or minimum iOS version.
- Existing Runtime extensions use manual reference counting. ARC settings found on another
  target must not be applied indiscriminately to the Runtime or new adapter.
- The SDK guide describes exporting a RuntimeIPhone Xcode project from Fusion, registering
  the extension, using Application.cci for iterative testing, and creating an extension `.ext`
  with iOSExtMaker from the project ZIP. It specifies one `.h` and `.m` source pair.
  Multi-file shared-header delivery and `.mm` handling therefore need an actual packaging
  round trip; neither can be assumed from successful Xcode development builds.
- Installed exporter Runtime sources are local SDK material; some were merged locally and
  excluded from Git by setup scripts. Do not commit or package the entire SDK checkout.

## Proposed architecture

The selected design is a precompiled static `FindTheWayCore.xcframework` exposing a C ABI,
with a thin Runtime adapter. Build the core independently with C++20 and Apple libc++;
the exported application's Runtime source compilation can retain its existing C++11 setting.
The public header must compile as C, Objective-C, and C++11: use opaque handles, fixed-width
integers, explicit string/buffer lengths, function-pointer callbacks, and explicit release functions.
Do not expose STL, C++ classes, templates, exceptions, or allocation ownership across the ABI.
A new iOS-owned bridge can wrap existing shared interfaces without modifying shared headers.

Use `FindTheWayFusion` inside the core for existing numeric ACEs and retain current serialization.
Keep object selection, creation, parameters, collision queries, image access, backdrops and event
dispatch in the Runtime adapter. The supplied project already compiles `.m` as Objective-C++;
a C-compatible bridge header avoids requiring newer C++ features in that compilation.

Own state through RAII and retain a local shared owner during calls that can generate events.
Snapshot iteration coordinates before synchronous callbacks, preserving nesting and callback
order. Define behavior when an event destroys or replaces the map or extension. Avoid ownership
cycles between C++ callbacks and Objective-C owners; clear callbacks during teardown.
Translate C++ failures at the adapter boundary into the corresponding Fusion failure/result.

Convert NSString UTF-16 to/from the core's wide strings with explicit lengths and defined
surrogate handling. Test supplementary characters and embedded NUL. Do not reinterpret
NSString storage as wchar_t or assume Windows wchar_t width. Check narrowing of core
coordinates/sizes into Runtime integer values.

## Build and packaging investigation

1. Create a minimal application using the actual installed Fusion iOS exporter. Record exporter,
   Xcode, Apple SDK, and scheme versions. Preserve the supplied projects as references.
2. On macOS, first build the unmodified exported project to distinguish Runtime/toolchain
   failures from extension failures. Independently compile the C ABI bridge, FindTheWayFusion,
   and AOE into a static library using C++20. Then link the adapter against the XCFramework.
3. Resolve iconv against the Apple target SDK and verify encoding aliases/signatures. Check
   std::format and filesystem availability against the selected minimum OS. Precompilation
   isolates language settings but does not remove OS/runtime availability requirements.
4. Validate the extension installation/export round trip early. Determine whether `.ext` can
   transport the owned precompiled library and C header, or whether a separate post-export
   integration tool must stage them and update the generated project. Do not assume that
   successful manual Xcode integration establishes automatic Fusion packaging.
5. Follow the concrete exported-project configuration below. Build simulator and device variants,
   first with unsigned simulator validation and then device build/run. Record actual successful
   commands once macOS/Xcode is available.

### Configuration selected from the supplied blank projects

Inspected `/workspace/sdk/ios-projects/Proj/Proj.xcodeproj/project.pbxproj` and the corresponding
`ProjFin` project, their shared scheme XML, and both turbojpeg XCFramework Info.plist files.
Both projects use an application target plus a Box2D static-library target. Their application
Frameworks phases already link `turbojpeg.xcframework` and `libBox2D.a`. Turbojpeg is a static
library XCFramework with separate iOS device and simulator entries. The application target has
only Resources, Sources and Frameworks phases; the CopyFiles phase belongs to Box2D and copies
headers, not embedded dynamic frameworks. This provides a concrete static-library precedent.

Proposed integration layout (new extension-owned files, not yet created):

```text
Libraries/FindTheWayCore.xcframework/
Classes/Extensions/FindTheWayCore.h
Classes/Extensions/CRunFindTheWay.h
Classes/Extensions/CRunFindTheWay.m
```

| Component | Proposed configuration |
| --- | --- |
| Core producer | Independent static-library build, C++20, libc++, C++ exceptions enabled internally |
| Core architectures | iOS arm64 and iOS Simulator arm64 only; Apple Silicon development hosts |
| Container | `xcodebuild -create-xcframework` from distinct device/simulator static archives and the C header |
| App library reference | Relative project reference to Libraries/FindTheWayCore.xcframework |
| App build phase | Add XCFramework to Link Binary With Libraries, following turbojpeg |
| Embedding | Do Not Embed; no new dynamic-framework copy/sign phase or runpath needed |
| Public header | Explicit adapter header path; no _DeLib/_3rdLib headers required by the App |
| System dependencies | Link Apple SDK libc++ and iconv as required; preserve inherited linker flags |
| Runtime language | Preserve current c++0x and Objective-C++ input mode; C ABI header requires no C++20 |
| Runtime memory management | Preserve Runtime ownership convention; do not enable ARC globally |
| Bitcode | Disabled in supplied project; do not introduce a bitcode requirement |
| Signing | Preserve application signing settings; configure team/profile on the user's Mac |
| Minimum OS | Supplied App target says 12.0; actual supported minimum remains pending Apple core availability checks |

Intel Macs, x86_64 simulator builds, and Rosetta-based validation are outside the supported scope.
Do not copy turbojpeg's armv7/i386/x86_64 architecture list into the new library. Device arm64
and simulator arm64 are different platform slices and must not be combined into one fat archive.
Confirm selected slices at link time. System libc++ remains supplied by iOS; do not ship Android
libc++_shared.so. A pure C API referenced directly from adapter code should not require new
-all_load or -force_load flags; investigate actual link failures before adding such flags.

The existing shared scheme is named RuntimeIPhone but references RuntimeIPhone.xcodeproj and
RuntimeIPhone target/product names, whereas these extracted projects are named Proj/ProjFin.
Do not assume the shared scheme is runnable. Validate with `xcodebuild -list` on macOS, then use
or repair a scheme against the actual target. No scheme was changed during this inspection.

A dynamic framework remains technically possible, but would add an application Embed Frameworks
phase with signing and runtime search-path configuration. The supplied projects provide a direct
static XCFramework precedent, so static is the selected initial delivery format.

Do not reuse Gradle, NDK ABI lists, ELF alignment, JNI handles, R8 rules, Android publishing
hooks, or libc++_shared.so packaging for iOS. Reusable Android experience is limited to interface
parity, ownership, Unicode, callback reentrancy, coordinate analysis, package ownership, and
layered verification. Read the actual iOS sprite/backdrop paths before applying hotspot fixes.

## Implementation milestones and acceptance

1. Establish the current master baseline in an isolated checkout and record the complete ACE
   matrix, serialized property layout, supported features, and mobile exclusions.
2. Pass the Apple static-core/C-ABI compile/link probe and minimal installation/export round trip. This is the first
   milestone because packaging constraints can change the source layout.
3. Implement lifecycle, property reading, map creation/costs/serialization, pathfinding,
   coordinate conversion, and scalar/string expressions. Compare results with shared-core fixtures.
4. Implement ranges, ZOC, AOE, caches, path/map iteration, and nested events. Test map replacement,
   destruction during callbacks, repeated creation/release, invalid data, and Unicode names.
5. Implement Runtime selection/qualifiers, creation, collision generation, grid images and
   backdrops. Check scrolling, layers, per-frame hotspots, internal obstacles and outer borders
   on a real Runtime application. For unsupported features, consume parameters, return explicit
   failure, and preserve the established map-reset behavior where required by compatibility.
6. Repeat installation/export/build from the produced package; inspect ownership and dependencies,
   run simulator/device scenarios, then document reproducible commands and evidence in English.

Linux/Android test results provide reference behavior but do not establish Apple compilation,
exporter packaging, or device rendering. This investigation ran no Apple builds: the current
execution environment is Linux and has no xcodebuild or Apple SDK.

## Shared-core review boundary

No shared C++ modifications are part of this investigation. Prefer an iOS adapter and build
configuration using the existing shared interfaces. If Apple compilation demonstrates a required
shared-header fix, prepare a separate minimal patch with compiler evidence, Windows/Android impact,
and relevant regression results for author review before applying it to the shared implementation.
Keep any future iOS skill separate from the Android skill and write it and its references in English.
