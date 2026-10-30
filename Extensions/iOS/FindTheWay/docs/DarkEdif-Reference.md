# DarkEdif iOS precompiled-extension reference

Inspected Phi's upstream SortaCore/MMF2Exts at commit
ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2. This is an implementation reference,
not a dependency or a decision to port FindTheWay onto the DarkEdif framework.

## Evidence

- `DarkEdif/Lib/FusionSDK.props` selects StaticLibrary for iOS, libc++, and
  iphoneos/iphonesimulator builds. ARM64 device builds trigger a separate simulator
  build. After Build, DarkEdifPostBuildTool receives the output directory, target
  architecture/platform/SDK and extension metadata to package the result.
- `FusionSDKConfig.Template.ini` configures a remote Mac, PSCP/PLink paths and
  arguments, the remote build folder, and selected iOS architectures. Apple builds
  occur on Mac even when Visual Studio is the initiating interface.
- `DarkEdif/- MFAs and documentation/DarkEdif MultiTargeting.md`, under "My
  extension's files are missing from built Fusion iOS projects!", states that the
  tool packages compiled iOS files into an encoded EXT in Data/Runtime/iPhone.
  Fusion exports the extension files, but the Xcode project may not reference
  them. Its instructions explicitly add the CRun extension files from Extensions
  and the XCFramework from Frameworks to the generated Xcode project.
- `DarkEdif/Phi Object/CRunPhi_Object.hpp` declares extern-C entry points and an
  opaque C++ instance pointer. Its generated `.mm` class forwards lifecycle and
  ACE calls to those functions and provides Runtime callbacks in the opposite
  direction. This is direct evidence for a precompiled C++ implementation plus
  a thin source adapter compiled with the exported application.

Links (pinned):

- https://github.com/SortaCore/MMF2Exts/blob/ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2/DarkEdif/Lib/FusionSDK.props
- https://github.com/SortaCore/MMF2Exts/blob/ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2/DarkEdif/-%20MFAs%20and%20documentation/DarkEdif%20MultiTargeting.md
- https://github.com/SortaCore/MMF2Exts/blob/ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2/DarkEdif/Phi%20Object/CRunPhi_Object.hpp
- https://github.com/SortaCore/MMF2Exts/blob/ac3f3f9dd5d54e9fc2c980b33450c79f23ad58b2/FusionSDKConfig.Template.ini

## Implications for FindTheWay

Use the same overall delivery model: independently compile the C++20 core on Mac,
produce separate arm64 device/simulator slices in a static XCFramework, package
that with the C-compatible header and thin Runtime sources in the iOS extension,
then let the final exported application compile the adapter and link the core.
Visual Studio RunOnly can orchestrate the Mac build or consume its verified outputs;
no manually maintained second Xcode application project is required for the core.

DarkEdif establishes that the .ext/export path can carry an XCFramework and multiple
adapter sources. Earlier uncertainty about whether that is possible is resolved.
What remains unverified is our exact exporter version's handling of package entries
and Xcode references, and how to reproduce the encoded .ext using our publisher.
The inspected tree exposes the PostBuildTool executable rather than its encoder
source; no byte-level .ext encoder has been implemented or tested here.

The final target should be a self-contained .ext carrying the required extension
files. The existing separate support ZIP/importer is provisional infrastructure,
not evidence that a permanently separate XCFramework download is necessary.
Add an idempotent post-export Xcode integration step if our exporter also omits
project references, following the manual steps documented by DarkEdif. Preserve
unrelated project settings and existing references.

Android's precompiled-library delivery also avoids replacing Fusion's Gradle and
manifest configuration. Retain that constraint. For iOS, the corresponding task
is linking references in the generated Xcode project, not replacing its global
language standard or signing configuration. Do not copy DarkEdif's old Intel,
32-bit, SDK-version, or VS remote-tool defaults into this arm64-only port.
