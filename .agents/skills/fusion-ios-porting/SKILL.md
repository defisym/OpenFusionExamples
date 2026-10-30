---
name: fusion-ios-porting
description: Port Clickteam Fusion extensions to iOS using a shared C++ core, a Runtime adapter, Apple command-line builds, and verified extension packaging. Keep Android and macOS application procedures separate.
---

# Port Fusion extensions to iOS

## Scope and constraints

Use for iOS Runtime integration, native builds, exported Xcode projects, and .ext
publishing. Derive paths, commands, SDK requirements and artifact names from the
target extension's current project. Do not make FindTheWay settings mandatory
for other plugins. Write this skill, references and operational docs in English.
Preserve shared C++ interfaces; submit shared behavioral changes for author review.
Keep private SDK/exporter material outside the public repository.

## Workflow

1. Inspect current Windows ACE definitions, mobile adapters, the installed exporter
   and a real exported project. Inventory IDs, property layout, parameter decoding,
   events, selection, creation and unsupported operations before implementation.
2. Reuse computational algorithms in shared C++. Keep Runtime objects, selection,
   creation and event dispatch in an owned adapter. Compare helpers with current
   Windows and Android versions; preserve each Runtime's actual representations.
   Check qualifiers, OI handle/index distinctions, slot zero, empty selections,
   destroyed objects, nested events, scope/action restoration, layer sentinels and
   post-creation physics/visibility. Do not assume the bundled mobile helper has
   the same capabilities as the maintained Windows helper.
3. For a precompiled core, define a C-compatible boundary with opaque ownership,
   fixed-width values, explicit buffer/string lengths and exception translation.
   Avoid STL and C++ layout exposure. Retain owners across callback reentrancy;
   snapshot iteration results and define destruction/replacement behavior.
4. Keep each plugin's build project under Extensions/iOS/<Plugin>. Use CMake and
   Ninja/Make for scripted compilation; use Apple CLI tools for SDK selection and
   XCFramework creation. Full Xcode supplies iOS SDKs; standalone Command Line
   Tools are insufficient. Do not require opening Xcode GUI to build the core.
5. Build separate iOS device and simulator platform slices. The current task
   supports arm64 only; do not add Intel/Rosetta/32-bit targets. Use a static
   XCFramework when it matches the exported project's linking model. Prebuilding
   isolates language settings, not the deployment availability of libc++ APIs.
6. Stage binaries with source/configuration and per-file hashes, normalize Git
   text line endings for source fingerprints, and keep binary hashes byte-exact.
   Publish only after all builds/verification succeed; preserve previous outputs
   on failure. Keep Build directories ignored and transfer only owned artifacts.
7. Publish the extension as .ext, including required native and adapter material.
   Validate its actual encoding and export round trip; do not rename an arbitrary
   ZIP to .ext. Fusion may export files without adding their Xcode references.
   Verify library/source references, include paths, system dependencies and final
   linkage; prefer an idempotent integration step if project references are missing.
8. Prefer a one-way handoff: Mac compiles native artifacts; Windows stages the
   adapter and creates .ext. Do not require returning a Windows-made .ext to Mac.
   Retain useful official packaging utilities and usage guides; remove obsolete
   application scaffolding selectively. A GUI utility is not evidence of a CLI.
   Reference architecture is guidance, not proof that every mechanism is optimal.
   Hook Windows publishing into the target extension's existing release entry
   point before its final archive is produced. Distinguish native-only artifacts
   from a complete plugin; never report an unfinished adapter as release-ready.
   Put plugin-specific generated-output rules in that plugin's .gitignore.
9. Validate in layers: host/core and ABI tests; actual Apple compile/link;
   helpers against private Runtime declarations; .ext install/export; simulator
   and device behavior. User-supplied Apple build logs establish that build only,
   not full ACE coverage or device correctness. Keep concise usage documentation;
   avoid committing investigation transcripts, temporary plans or redundant reports.

## Architectural reference: Phi's DarkEdif

Reference ideas only; do not copy implementation or generated adapter code.
Upstream: https://github.com/SortaCore/MMF2Exts (inspected revision
ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2).

- DarkEdif/Lib/FusionSDK.props selects static iOS libraries, device/simulator
  builds and a post-build packaging tool. FusionSDKConfig.Template.ini describes
  VS-initiated remote Mac builds and artifact transfer.
- DarkEdif/- MFAs and documentation/DarkEdif MultiTargeting.md documents encoded
  .ext delivery containing compiled iOS files. It instructs users to add exported
  CRun sources and the XCFramework to Xcode when references are not generated.
- DarkEdif/Phi Object/CRunPhi_Object.hpp and .mm demonstrate a thin Runtime class
  forwarding to precompiled implementation through extern-C calls and an opaque
  instance pointer, with callbacks back into the Runtime.
- The inspected tree exposes DarkEdifPostBuildTool.exe rather than its encoder
  source. These references establish a delivery model, not a verified .ext codec
  for a different publisher. Do not inherit its old SDK, VS, Intel or 32-bit defaults.
- Android's prebuild-before-packaging model also avoids replacing Fusion's Gradle
  and manifest configuration. Treat that as a design lesson, not an iOS build recipe.
