# FindTheWay iOS publishing from Visual Studio RunOnly

The Run_Only Unicode configuration imports FindTheWay/Publish.targets. Its iOS
staging target runs before PackageFindTheWayAndroid, so the current master Python
publisher includes iOS outputs when archiving the entire ToInstall tree. It also
runs after Build when invoked without the Android publishing target. Debug and
Release editor builds and design-time builds do not package iOS. The target has
no Inputs/Outputs shortcut and runs even when the linker has no new work, provided
MSBuild actually executes Build.

The local development snapshot predates the Android Python publisher and its VS
target. That older batch publisher was not replaced. Apply the new import/targets
together with current master's existing Android RunOnly integration to obtain the
combined release archive; do not treat the old local checkout as a verified full
release pipeline.

## Artifact preparation on Mac

The per-extension CMake project and scripted producer are in root
`Extensions/iOS/FindTheWay/`; see its README. CMake uses Ninja/Make, followed by
`xcodebuild -create-xcframework`; no Xcode GUI is required. The default producer
output is a native-only manifest until a real matching .ext is supplied.


Windows stages release artifacts; Apple compilation and Runtime validation happen
on an Apple Silicon Mac. The complete iOS ACE implementation, .ext packaging and
post-export integration are not ready yet. The publisher deliberately requires an
actual prepared .ext rather than creating a fake package from the placeholder.

After completing those steps, prepare this artifact directory:

```text
FindTheWay/
  FindTheWay.ext
  FindTheWayCore.xcframework/
    Info.plist
    ... device arm64 static library and headers ...
    ... simulator arm64 static library and headers ...
  Integration.md
```

Integration.md must explain how this release's exported Xcode project receives the
core, helpers, public header and linker dependencies. Record the Xcode/exporter and
minimum OS versions and the actual build/Runtime verification. The XCFramework
must contain exactly one arm64 device slice and one arm64 simulator slice. Intel
architectures are rejected. No private SDK source belongs in this directory.

With the same source checkout used for the build, record the release hashes:

```sh
python3 Extensions/_Script/FindTheWayIOS.py \
  --artifacts /path/to/FindTheWay --record-manifest
```

Transfer the complete directory, including manifest.json, to Windows. Source and
file hashes detect a different checkout, stale binaries, corruption, and extra
payload files. Metadata and archive-format checks are not substitutes for Apple
link/build/device verification; manifest creation does not perform that validation.
No new iOS plugin has been produced or validated by the current Linux work.

## RunOnly integration

```bat
msbuild Extensions\FindTheWay\FindTheWay.vcxproj /t:Build ^
  /p:Configuration="Run_Only Unicode" /p:Platform=Win32 ^
  /p:FindTheWayIOSArtifacts="D:\Artifacts\FindTheWay"
```

Defaults:

- FindTheWayPackageIOS=true. Missing or stale iOS artifacts fail the build.
- FindTheWayIOSArtifacts=Extensions/iOS/FindTheWay/Artifacts/Current (repository root).
- FindTheWayPython=python; override with a Python 3.10+ executable path if needed.

Explicitly build without staging iOS using `/p:FindTheWayPackageIOS=false`.
The existing FindTheWayPackageAndroid property remains independent. Skipping a
platform does not delete its artifacts from a previous ToInstall tree; use a clean
release staging tree when defining the contents of a new release.

Verified iOS files are staged to:

- ToInstall/Files/Data/Runtime/iPhone/FindTheWay.ext
- ToInstall/Files/Tools/FindTheWay-iOS.zip (XCFramework, integration instructions,
  and manifest for the Mac/Xcode step)

The existing current-master publisher then creates the dated and latest combined
release ZIP. The iOS target does not install into the live Fusion directory; users
install from ToInstall. It does not change signing, the installed exporter SDK,
Android templates, or Windows editor outputs.

## Checks performed here

Five standard-library tests exercise staging, unchanged outputs after stale-source
rejection, file tampering, Intel metadata rejection and SDK payload exclusion.
MSBuild project/target XML parses. These are fixture-based publisher tests, not an
MSBuild execution, real .ext export, Apple binary build, or device run.

```sh
python3 -m unittest discover -s Extensions/iOS/tests -p test_publisher.py -v
```
