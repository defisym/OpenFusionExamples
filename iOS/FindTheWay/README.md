# FindTheWay iOS project

## Build after pulling the repository on an Apple Silicon Mac

Requirements: Python 3.10+, CMake 3.22+, full Xcode selected with xcode-select,
and optionally Ninja. Use the shared C++ baseline containing FindTheWayFusion.h;
the old local Java-only checkout is insufficient. No Xcode GUI is needed.

From the repository root:

```sh
python3 iOS/FindTheWay/build.py --minimum-ios "$FTW_MINIMUM_IOS"
```

For the first compile probe, set `FTW_MINIMUM_IOS=17.0`; this is a test target,
not a claim that Apple compilation or this minimum has passed. Choose a final
minimum based on actual Apple libc++ availability; iOS 12 in the
sample Runtime project has not been established as supported by the C++20 core.

The command independently configures iphoneos arm64 and iphonesimulator arm64
using CMake and Ninja (Make fallback), then uses xcodebuild -create-xcframework.
Build files live in iOS/FindTheWay/Build. The generated CMake project builds the
existing core target; no algorithms or DarkEdif code are duplicated here.

## Transfer to Windows

Successful builds replace iOS/FindTheWay/Artifacts/Current with rollback on
publication failure. The directory contains the static XCFramework, integration
instructions and a source/file-hash manifest. Source changes during a build fail
publication. Source fingerprints normalize Git CRLF/LF differences; binary hashes
remain byte-exact.

Artifacts/Current is deliberately not ignored by Git so it can be transferred by
committing the outputs, or copied to the Windows checkout using a file-transfer
method. Scripts do not commit, push, upload, or request credentials automatically.
Only transfer generated artifacts, not Build or private SDK source. Pull the same
source commit on Windows before consuming the artifacts.

Windows RunOnly now defaults FindTheWayIOSArtifacts to this directory. A custom
transfer directory can still be selected with that MSBuild property. The verifier
rejects stale sources or changed files before updating ToInstall.

## Current implementation boundary

The default output has manifest kind "native": it contains a built core, not a
complete Fusion plugin. Windows can verify it, but refuses to publish it as a
working .ext. Full ACE integration and the .ext encoder/export round trip remain
unfinished. Once a real matching .ext exists, a release bundle can be prepared:

```sh
python3 iOS/FindTheWay/build.py --minimum-ios "$FTW_MINIMUM_IOS" \
  --extension-package /path/to/FindTheWay.ext
```

This copies the provided extension and records kind "release". It does not generate
.ext or prove Runtime validation. Windows RunOnly then verifies and stages the
release bundle before current master's Android publisher archives ToInstall.
The final goal remains a self-contained .ext; the separate support ZIP is
provisional until the exporter/encoder integration is complete.

## Verification performed

The CMake entry project and core C caller tests passed on Linux against the isolated
current shared-header baseline. Python tests check command orchestration and
artifact/source validation using fixtures. Apple compilation has not run in this
Linux environment; command-fixture tests are not Apple build evidence.
