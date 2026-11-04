---
name: fusion-ios-porting
description: Port Clickteam Fusion extensions to iOS using a shared C++ core, a Runtime adapter, Apple command-line builds, and verified extension packaging. Keep Android and macOS application procedures separate.
---

# Port Fusion extensions to iOS

## Scope and constraints

Apply the [common porting rules](../fusion-extension-porting/SKILL.md), including
Git checkout normalization and artifact hash verification, before this workflow.

Use for iOS Runtime integration, native builds, exported Xcode projects, and .ext
publishing. Derive paths, commands, SDK requirements and artifact names from the
target extension's current project. Do not make FindTheWay settings mandatory
for other plugins. Write this skill, references and operational docs in English.
Preserve shared C++ interfaces; submit shared behavioral changes for author review.
Keep private SDK/exporter material outside the public repository.

If encryption is involved, also apply
[shared encryption](../fusion-shared-encryption/SKILL.md): that runtime is an
author-approved shared service, not a rule to dynamically split every helper.

## Workflow

1. Inspect current Windows ACE definitions, mobile adapters, the installed exporter
   and a real exported project. Inventory IDs, property layout, parameter decoding,
   events, selection, creation and unsupported operations before implementation.
2. Treat the Windows C++ implementation as the behavioral oracle for portable
   features. Add direct comparisons against its actual classes for the target
   plugin's data, flags, numeric/string semantics and caching; do not use Android
   output as the specification. Adapt only platform-specific I/O/object mechanics using
   the actual iOS SDK. Reuse computational algorithms in shared C++. Keep Runtime objects, selection,
   creation and event dispatch in an owned adapter. Compare helpers with current
   Windows and Android versions; preserve each Runtime's actual representations.
   Check qualifiers, OI handle/index distinctions, slot zero, empty selections,
   destroyed objects, nested events, scope/action restoration, layer sentinels and
   post-creation physics/visibility. Do not assume the bundled mobile helper has
   the same capabilities as the maintained Windows helper.
   Use the reusable Runtime helpers described below; keep FTW or other plugin
   semantics in the consuming adapter.
3. For a precompiled core, define a C-compatible boundary with opaque ownership,
   fixed-width values, explicit buffer/string lengths and exception translation.
   Avoid STL and C++ layout exposure. Retain owners across callback reentrancy;
   snapshot iteration results and define destruction/replacement behavior.
4. Prefer Extensions/iOS/<Plugin> for a new plugin build project; honor an
   established owned project layout without retaining a legacy SDK application. Use CMake and
   Ninja/Make for scripted compilation; use Apple CLI tools for SDK selection and
   XCFramework creation. Full Xcode supplies iOS SDKs; standalone Command Line
   Tools are insufficient. Do not require opening Xcode GUI to build the core.
5. Build separate iOS device and simulator platform slices. Derive architectures
   from the target project and author requirements. The FTW reference uses arm64
   only; do not make that a universal rule or add Intel/Rosetta/32-bit slices
   without a target requirement. Use a static
   XCFramework when it matches the exported project's linking model. Prebuilding
   isolates language settings, not the deployment availability of libc++ APIs.
6. Stage binaries with source/configuration and per-file hashes, normalize Git
   text line endings for source fingerprints and transferred artifact text
   (public headers, XML metadata and integration notes). Windows Git checkout
   can convert these to CRLF. Keep static archive/.ext hashes byte-exact.
   Publish only after all builds/verification succeed; preserve previous outputs
   on failure. Keep Build directories ignored and transfer only owned artifacts.
7. Publish the extension as .ext, including required native and adapter material.
   Native-only manifests are valid inputs to a Windows packager: verify compiled
   dependencies, then package the current Runtime adapter there. Do not force a
   Mac rebuild for adapter sources shipped separately from the compiled core.
   If an adapter is compiled into the transferred library, its changes do require
   a rebuild; record that boundary instead of applying a blanket exemption.
   Validate its actual encoding and export round trip; do not rename an arbitrary
   ZIP to .ext. Fusion may export files without adding their Xcode references.
   Verify library/source references, include paths, system dependencies and final
   linkage; prefer an idempotent integration step if project references are missing.
   Check exported Scheme container/target references after project renaming,
   effective architecture settings at project and dependency levels, required
   versus weak linkage of static libraries, and consistent deployment targets.
   If template defaults need conversion, provide an idempotent, target-aware CLI
   with a dry-run mode; preserve unrelated flags, signing and SDK extensions.
8. Prefer a one-way handoff: Mac compiles native artifacts; Windows stages the
   adapter and creates .ext. Do not require returning a Windows-made .ext to Mac.
   Retain useful official packaging utilities and usage guides; remove obsolete
   application scaffolding selectively. A GUI utility is not evidence of a CLI.
   Reference architecture is guidance, not proof that every mechanism is optimal.
   Hook Windows publishing into the target extension's existing release entry
   point before its final archive is produced. Install to the actual Fusion
   iPhone runtime directory when requested; fail clearly for a missing directory.
   Distinguish successful packaging from complete Runtime feature parity.
   Put plugin-specific generated-output rules in that plugin's .gitignore.
9. Image maps need CPU-readable pixels, including reloading purged Runtime data.
   Preserve sampling coordinates, red/green channels, row pitch and image format;
   distinguish logical colors from premultiplied display colors. Delegate paths
   to the Runtime resource loader for sandbox, bundle, embedded and security-scoped
   files. Do not invent writable paths to read images or dereference desktop
   surface pointers on iOS. Document irrecoverable export quantization/alpha loss.
   For background drawing, inspect both sprite positioning and visibility bounds:
   Windows surface origins may be top-left coordinates while iOS sprite positions
   are image-hotspot anchors. Convert per selected frame and keep the backdrop's
   stored hotspot consistent; setting it to zero does not disable the sprite's
   own image-hotspot subtraction. Test different animation-frame hotspots.
10. Validate in layers: host/core and ABI tests; actual Apple compile/link;
   helpers against private Runtime declarations; .ext install/export; simulator
   and device behavior. User-supplied Apple build logs establish that build only,
   not full ACE coverage or device correctness. Keep concise usage documentation;
   avoid committing investigation transcripts, temporary plans or redundant reports.

## Reusable Runtime helpers and exported-project patches

When lookup/selection/creation infrastructure is needed, use the maintained
`Helpers/ObjectBase.h`, `ObjectSelection.h` and `ObjectCreation.h` as separate
generic classes, with common OI access in ObjectBase. Do not add a plugin core
dependency merely to obtain these helpers or import unused plugin semantics. Compare the current Windows
`Extensions/_3rdLib/Object*.h` and Android `_3rdLib` counterparts, including APIs
not yet used by the consuming extension. Do not rename them for that extension.

The exporter already has its own ObjectSelection.h. Package the maintained
headers under export-root `Helpers/`, outside `Classes/`; use explicit relative
includes from Runtime sources. Verify the paths in the actual package, and avoid
global recursive search paths or header-map entries that could shadow the SDK
file. Inline implementations need no helper compilation unit or native library;
validate multiple translation units and package one consistent revision. Generate
extension-specific PBX IDs for any shared files that do require project entries.

For adapter/helper fixes, provide an overlay patch for an existing Xcode export
when useful. Preserve the export-relative directory layout, include all changed
sources/headers and validate quoted include resolution. This permits testing on
Mac without another Windows/Fusion export. If an old helper compilation unit is
removed, update the project's source references or supply an empty compatibility
unit in the overlay so old references do not cause missing-file/duplicate-symbol
errors. Keep temporary compatibility units and patch archives out of production
commits. State which files to overwrite and any required project edits.

## Runtime diagnosis and completion evidence

Distinguish the selected build SDK, deployment target and simulator OS. Lowering
the minimum OS or choosing an older simulator does not change the build SDK.
If UIKit rejects startup for missing UIScene lifecycle adoption, investigate the
exporter's application lifecycle separately from the plugin. A plist declaration
alone does not implement scene/window/event handling. Describe an older-SDK test
as a temporary compatibility result, not a lifecycle migration or an official
vendor workaround unless verified from a vendor source.

For missing hover previews, inspect the Runtime's touch-to-mouse mapping and
the example's event triggers before changing object creation or algorithms.
Check tap/drag behavior; distinguish input-trigger differences from incorrect
object counts, coordinates or selections.

Report implementation, host/model tests, Apple compilation, packaging/export,
simulator behavior and device coverage separately. Revalidate the latest changed
adapter/helpers on Apple: an earlier successful native build or app launch does
not validate a later refactor. Generic helper tests must not link a plugin core.
Document host-only test limitations, including GNUstep versus Apple's exception
runtime. When implementation is complete, identify the remaining concrete review
and platform checks; do not imply exhaustive ACE coverage from one working demo.

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

## Legacy SDK .ext format and independent CLI

The repository's make_ext.py is independently implemented; it does not wrap or
copy DarkEdif. Compatibility checks against non-placeholder SDK packages establish
an involutive byte framing around a traditionally encrypted ZIP, with project.txt
holding six NUL-terminated PBX insertion blocks (source build files/references,
group entries, Sources entries, header references, header group entries). Generate
owned IDs and validate ZIP decryption/CRC plus payload contents before publishing.
Do not put private SDK sources into regression fixtures or package them wholesale.

A C/Objective-C Runtime pair can emit Mach-O .linker_option directives selecting
separate device/simulator archives by TargetConditionals. Use this only after
verifying the target exporter's library search paths and actual links. The supplied
project searches SRCROOT, allowing both uniquely named archives at ZIP root.
This can avoid manual framework-reference edits; it is not a universal exporter
assumption. Retain the XCFramework as the native handoff representation.

Wine is optional validation material, not a publishing dependency. If it cannot
run a legacy 32-bit maker reliably in the executor, report that limitation; do not
claim its GUI or command-line interface passed. A verified independent format CLI
is preferable to fragile coordinate clicking when it meets the target workflow.
