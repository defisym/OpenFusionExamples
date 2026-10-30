# FindTheWay iOS build

Requires Apple Silicon macOS, full Xcode selected with xcode-select, Python 3.10+,
CMake 3.22+, and optionally Ninja. CMake builds the shared C++20 core using Ninja
or Make; xcodebuild packages separate device/simulator arm64 slices. No GUI is needed.

From the repository root:

```sh
set -o pipefail
python3 Extensions/iOS/FindTheWay/build.py --minimum-ios 17.0 \
  2>&1 | tee /tmp/findtheway-ios-build.log
```

Build cache: `Build/`. Transfer output: `Artifacts/Current/`. The latter is tracked
so the Mac can commit its XCFramework and manifest for the Windows checkout.
Scripts do not commit or push automatically. Pull matching sources on Windows;
source hashes normalize CRLF/LF while binary hashes remain byte-exact.

The author successfully built both arm64 slices and packaged the XCFramework with
Xcode 27.1 (27A9275), targeting iOS 17.0. This verifies the core build, not the full
Runtime adapter, exported application, or device behavior.

The C ABI currently covers maps, cell costs, basic paths, serialization and grid
conversion. Full ACE dispatch, remaining queries and the .ext export round trip
are unfinished. A normal build emits manifest kind "native" and is deliberately
rejected by Windows full-plugin publishing. Once a real matching .ext is available:

```sh
python3 Extensions/iOS/FindTheWay/build.py --minimum-ios 17.0 \
  --extension-package /path/to/FindTheWay.ext
```

Windows RunOnly verifies artifacts from this directory by default. Override
`FindTheWayIOSArtifacts` for another transfer directory or explicitly use
`FindTheWayPackageIOS=false` until full-plugin publishing is ready. Verified release
bundles stage FindTheWay.ext and the Xcode support ZIP under the Windows plugin's
ToInstall tree before the existing Android publisher creates the combined archive.
Generated release-output ignores belong to `Extensions/FindTheWay/.gitignore`.

For manual Xcode integration, add the static XCFramework to Link Binary With
Libraries with Do Not Embed, expose FindTheWayCore.h, and link Apple libc++/iconv.
This still requires verification with the final .ext/export flow.

Host regression checks (GCC 14+ and the current shared headers):

```sh
cmake -S Extensions/iOS/FindTheWay -B /tmp/ftw-host -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/ftw-host
ctest --test-dir /tmp/ftw-host --output-on-failure
python3 -m unittest discover -s Extensions/iOS/tests -p 'test_*.py'
```
