# FindTheWay iOS

## Native artifacts and Windows publishing

The Runtime implements the Windows C++ condition/action/expression set, except
condition 27, which accepts a Windows-only `cSurface*`. On iOS use picture import
(condition 12) or an Active's fixed value/frame (condition 26). This is source
coverage, not a claim that Fusion export or device tests have already passed.

`Artifacts/Current` contains the author-built **ABI 2** core, built with Xcode
27.2 (27B52) for iOS 17.0 device/simulator arm64. Source fingerprints and archive
hashes match this revision; Windows can package these artifacts directly.
ABI 2 adds unit/ZOC-aware searches, areas, line ranges, AOE, map display, caching
and map-update callbacks. No shared algorithm header has been changed; the iOS
C boundary calls the same C++ classes as Windows.

Rebuild only after native sources, ABI or producer configuration change.
Requires full Xcode selected with xcode-select, Python 3.10+, CMake 3.22+, and
optionally Ninja. Both iOS device and simulator use arm64; no Intel/Rosetta slice.

```sh
set -o pipefail
python3 Extensions/iOS/FindTheWay/build.py --minimum-ios 17.0 \
  2>&1 | tee /tmp/findtheway-ios-build.log
```

Commit/transfer `Extensions/iOS/FindTheWay/Artifacts/Current` to Windows. Build
FindTheWay **Run_Only Unicode / Win32** in Visual Studio once. Python 3.10+ is
required there. RunOnly verifies the transferred core, creates `FindTheWay.ext`,
installs it into Fusion and includes it in the combined ToInstall release archive.
No Wine, GUI automation or Mac-generated .ext is required.

For a non-default Fusion installation, set the `FusionRoot` environment variable
before starting Visual Studio, or pass `/p:FusionRoot=...` to MSBuild. The default
matches the existing Steam publisher; missing iPhone runtime directories fail
explicitly. Standalone packaging/install is also available:

```bat
python Extensions\_Script\FindTheWayIOS.py --artifacts Extensions\iOS\FindTheWay\Artifacts\Current --install
```

Then export the application as an iOS Xcode project from Fusion and build on Mac.
Set the App minimum iOS version to at least the core build minimum (17.0 here).
The .ext contains the owned Runtime sources/helpers, C header, both platform
archives, XCFramework and generated PBX source references. Mach-O autolink selects
the correct archive plus libc++/iconv. It relies on the supplied exporter's
`LIBRARY_SEARCH_PATHS = $(SRCROOT)`. The Runtime requires C++11, while shared C++20
is precompiled. Adapter-only edits do not require a native rebuild; native source,
ABI or producer configuration edits do.

## Windows behavior and iOS resources

Portable behavior follows `Extensions/FindTheWay/Main.cpp`, `Func.h` and the
shared C++ algorithms: named/last paths, ignore flags, force/diagonal searches,
unit/ZOC lists, area rings and move/attack boundaries, line range, AOE filtering,
caching, serialization and map display. Runtime integration includes qualifiers,
selection/negation, object creation by parameter/name, collision/map/area/ZOC
iteration, map-update events and grid backdrops. Immediate events preserve the
outer selection and action state; object snapshots use slot plus creation ID.

Picture imports call Fusion's `loadResourceData:`. The supplied SDK normalizes
Windows editor/export paths and backslashes, searches the sandbox and bundle,
opens embedded data elements, and handles security-scoped/file URLs. No writable
path is synthesized and no directory is created to read an image. iOS filenames
retain their actual case. File formats are those decoded by UIImage.

Images are read from CPU pixels, including when the Runtime must reload purged
image data. Sampling follows Windows: dimensions use integer division by grid
size, cells sample at grid centers plus offsets, red is terrain cost, green is
dynamic cost. Picture sampling is orthogonal before applying isometric geometry.
The reader handles RGBA8888/JPEG, RGB888, RGBA4444, RGBA5551 and RGB565 with their
row pitch and premultiplication. Display-color getters do not scale costs by
alpha. Use opaque, lossless 32-bit map images for exact 8-bit costs; color already
lost to export quantization or fully transparent premultiplication cannot be
recovered. Signed offsets are normalized in the iOS C boundary to avoid Windows
LLP64 versus arm64 LP64 arithmetic differences.

The independently implemented .ext codec uses observed SDK framing and standard
ZIP encryption, verifies decryption/CRC and writes six PBX insertion blocks.
Official maker and guide remain in `../Tools`; architectural references live in
the English iOS skill. The generic CLI packages an owned Runtime pair:

```bat
python Extensions\iOS\Tools\make_ext.py --name Probe --project-zip RuntimeIPhone.zip --output Probe.ext
```

## Validation

```sh
cmake -S Extensions/iOS/FindTheWay -B /tmp/ftw-host -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/ftw-host
ctest --test-dir /tmp/ftw-host --output-on-failure
python3 -m unittest discover -s Extensions/iOS/tests -p 'test_*.py'
python3 Extensions/iOS/tests/run_runtime_fixture.py --core-library /tmp/ftw-host/core/libFindTheWayCore.a
```

The direct C++ oracle compares paths, 32 ignore flag combinations, area layers,
line ranges, ZOC, AOE, stash behavior and display output. Runtime fixture checks
use the actual adapter/core and Foundation (or GNUstep on Linux), with owned
models for SDK APIs. They cover selection, qualifiers, creation, callbacks,
image formats/pitch/alpha, resource-loader delegation and nested iteration.
These models do not replace actual Fusion or iOS execution.

On Apple Silicon, check all Runtime sources against a real exported project's
private SDK declarations before the final Xcode build:

```sh
python3 Extensions/iOS/Helpers/check_helpers.py --runtime-project /path/to/Proj
```

Final Fusion export, Xcode linking and simulator/device behavior remain external
validation steps. Keep the PR draft until those checks pass.
