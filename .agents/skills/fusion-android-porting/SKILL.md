---
name: fusion-android-porting
description: Port Clickteam Fusion C++ extensions to Android, or diagnose Android Java ACE dispatch, JNI, native-library packaging, Runtime compatibility, and image-coordinate issues. Use for shared C++ cores with Java adapters to Fusion's Android Runtime. Includes single-entry publishing and Visual Studio RunOnly integration. Do not use for iOS-only, HTML5-only, or Windows-only tasks.
---

# Port Fusion extensions to Android

## Scope

Use this skill only for Fusion Android/JNI implementation, builds, publishing, or troubleshooting.
For a multi-platform task, apply it to the Android portion; still assess shared C++ changes for their
effect on other platforms. Keep iOS/Objective-C++ and HTML5 workflows in separate skills.
Write this skill and its supporting repository documentation in English.

Keep algorithms in shared C++; use Java for Fusion Runtime integration and JNI marshalling.
Compile native libraries during extension development/publishing, then let Fusion use its own
default build templates when exporting an APK.

## Establish the target

1. Identify the extension, its Windows ACE definitions, existing mobile code, and the installed
   Fusion Runtime and development SDK versions.
2. Inspect repository instructions, branch state, and local changes. Preserve unrelated work.
   A development SDK is build/reference material, not extension-owned package content.
3. Locate the target extension's actual publishing entry point, CMake/native configuration, Java
   sources, package output, VS configuration, tests, and build documentation. Derive commands and
   names from these files. Do not require another extension's documentation or scripts.
4. Separate shared C++ changes from Android and build changes. Submit shared C++ changes for
   author review; do not merge the main branch without authorization. Continue routine work
   within existing authorization without repeatedly requesting approval.
5. Use private SDK material locally for inspection and builds; do not publish its sources,
   templates, or binaries in the public repository.

## Implementation sequence

1. **Inventory ACEs.** Match condition/action/expression IDs, parameter order and types, return
   values, and generated events. Preserve Java switch dispatch IDs. Unsupported mobile features
   must still consume their parameters and return the documented failure or empty result.
2. **Port shared dependencies.** Find assumptions about Windows PCHs, types, macros, strings, and
   paths. Adapt common helpers using standard facilities and narrow platform guards; keep callers
   using those helpers instead of duplicating implementations.
3. **Define the JNI boundary.** Keep algorithms and current data models in C++; handle Runtime
   objects, parameters, and events in Java. Keep legacy numeric/file-format compatibility in the
   Fusion adapter. Read [workflow.md](references/workflow.md) for lifetime and encoding details.
4. **Prebuild native libraries.** Build Release JNI libraries for every supported ABI, include
   required runtime dependencies, and stage them under `app/src/main/jniLibs/<ABI>/`. Fusion users
   should not need to compile the extension's C++ during ordinary APK export.
5. **Constrain package ownership.** Include explicit extension Java entry points and owned helpers.
   Exclude development SDK Runtime, Banks, Objects, and built-in extensions. Do not replace
   `app/build.gradle`, `settings.gradle`, application ProGuard files, or `RuntimeAndroid.zip`.
6. **Keep one publishing entry point.** A batch launcher and Python publisher can discover tools,
   prepare dependencies, build all ABIs incrementally, validate, package, install, and archive.
   Propagate failures and preserve logs. Do not pause for input when invoked by VS. In this
   repository, automatic Android publishing belongs to RunOnly; Release/Debug remain Windows builds.
7. **Validate in layers.** Check the core, JNI/ACEs, package contents, a real Runtime APK build,
   then device rendering and interaction. Report evidence at the layer actually exercised.

## Coordinate and image checks

- Distinguish grid indices, cell centers, image hotspots, image top-left positions, and frame,
  window, and layer coordinates.
- Inspect the actual Runtime sprite, paste, and redraw paths to establish where hotspots apply.
- Do not assume animation frames share a hotspot. If Windows uses a common image origin while
  Android applies each frame's hotspot, use
  `target anchor = reference anchor + target hotspot - reference hotspot + application offset`.
- Compare resulting image origins after hotspot application, not just raw Java arguments.
  Cover internal obstacles, outer boundaries, scrolling, target layers, and differing hotspots.
  Never use a DEBUG switch to change coordinate semantics.

## Delivery checks

- Package only owned Java, target ABI libraries, extension resources, and required license/source
  material. Test packaging from a full SDK checkout to catch accidental SDK inclusion.
- Inspect every supported ABI in the ZIP and final APK, including shared dependencies such as
  `libc++_shared.so`; check compatibility with other native extensions.
- Preserve JNI classes and callback names through R8. Use an additive consumer-rule mechanism,
  such as a JAR containing `META-INF/proguard/<extension>.pro`, rather than replacing global rules.
- Stage cleanly and replace output packages only after successful builds and validation.
- Maintain and execute the publishing entry point whenever sources, dependencies, or configuration
  change. A successful manual copy is not verification of the publisher.
- Document the target extension's reproducible commands, versions, artifact paths, evidence, and
  remaining validation. Keep commits reviewable and update the appropriate PR.

Read [workflow.md](references/workflow.md) for the reusable implementation procedure and
[troubleshooting.md](references/troubleshooting.md) for diagnosis. Their optional repository
examples illustrate the procedure; they are not prerequisites or fixed defaults for a new port.
