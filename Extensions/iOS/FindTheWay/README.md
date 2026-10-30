# FindTheWay iOS

## Windows build and Fusion export

The checked-in `Artifacts/Current` contains the author-built iOS 17.0 arm64 device
and simulator core. Pull the branch, select **Run_Only Unicode / Win32** in the
FindTheWay VS solution, and build. Python 3.10+ is required. RunOnly verifies the
core, creates FindTheWay.ext, installs it into Fusion, and includes it in the
combined ToInstall release archive. No Wine or GUI automation is required.

The default Fusion directory matches the existing publisher's Steam installation.
For another installation, set the `FusionRoot` environment variable before starting
Visual Studio, or pass `/p:FusionRoot=...` to MSBuild for the iOS target. Missing
iPhone runtime directories fail explicitly. The standalone command is:

```bat
python Extensions\_Script\FindTheWayIOS.py --artifacts Extensions\iOS\FindTheWay\Artifacts\Current --install
```

Then export the Fusion application as an iOS Xcode project. The extension contains
its Runtime pair, C header, both platform-specific static libraries, XCFramework
and generated project metadata. The adapter's Mach-O autolink directives select
the device/simulator library and system libc++/iconv. This uses the supplied
exporter's existing `LIBRARY_SEARCH_PATHS = $(SRCROOT)`; a different exporter must
preserve an equivalent root library search path. No global C++20 setting is needed.

The package codec is independently implemented from observed SDK framing and
standard ZIP encryption. The official GUI maker and SDK guide remain in ../Tools.
The generic CLI can package an owned Runtime pair from a project ZIP:

```bat
python Extensions\iOS\Tools\make_ext.py --name Probe --project-zip RuntimeIPhone.zip --output Probe.ext
```

## Implemented Runtime features

- Conditions 0/1: create/import maps; 4/5: find and check named/last paths;
  6/28: path/map iteration events; 7: map availability; 8: cell cost checks.
- Actions 0/4: set/clear terrain or dynamic costs; 2/20: path/map iteration;
  19: grid dimensions and offsets.
- Expressions 0-9 and 12-19: iteration coordinates/index, paths, coordinate
  conversion, cell costs, map serialization, numeric constants and flags.

Other ACEs return failure/empty results and log a warning once per ID. Object
selection/creation, collision/image maps, ranges, ZOC/AOE, map-update events and
map display strings are not connected yet. Shared helpers are retained for later
integration. This is a functional map/path build, not complete Windows parity.

Iteration snapshots survive nested events and map replacement; destroying the
extension stops iteration safely. Expression parameters are consumed before
handling missing/unsupported results. Offsets currently follow the C ABI's
nonnegative range. Runtime-only changes do not require rebuilding the core;
changes to native sources or producer configuration do.

## Rebuild the native core on Apple Silicon

Requires full Xcode selected with xcode-select, Python 3.10+, CMake 3.22+, and
optionally Ninja. The author built the core with Xcode 27.1 (27A9275), iOS 17.0.

```sh
set -o pipefail
python3 Extensions/iOS/FindTheWay/build.py --minimum-ios 17.0 \
  2>&1 | tee /tmp/findtheway-ios-build.log
```

Build cache is in Build; transfer/commit Artifacts/Current to Windows. The normal
native manifest is sufficient; Windows creates the current .ext itself.

## Validation

Host core/ABI tests, standard-library codec/publisher tests and the actual Runtime
adapter against owned API fixtures plus Foundation pass. The fixture tests cover
named paths, serialization, coordinates, nested events, replacement and teardown.
Cross-target LLVM links resolve both real Mac archives through autolink. These
checks do not replace Fusion's actual exporter or an Apple final-App/device test.

```sh
cmake -S Extensions/iOS/FindTheWay -B /tmp/ftw-host -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/ftw-host
ctest --test-dir /tmp/ftw-host --output-on-failure
python3 -m unittest discover -s Extensions/iOS/tests -p 'test_*.py'
python3 Extensions/iOS/tests/run_runtime_fixture.py --core-library /tmp/ftw-host/core/libFindTheWayCore.a
```

The last command uses Foundation on macOS or GNUstep Base and Clang on Linux.
