# FindTheWay precompiled iOS core

This is the first implementation milestone, not a completed Runtime extension.
The static library exposes a C ABI for map lifetime, cell costs, basic last-path
search/copy, map serialization and grid conversion. Events, named paths, range/ZOC/AOE
queries, and Runtime ACE integration remain to be implemented.
Shared algorithms and headers are unchanged. The existing placeholder extension
has not been wired to this partial API, so it must not be distributed as working.

Use current master shared headers containing FindTheWayFusion.h (research baseline:
29edeed0ffb6929c6a1ca34423002c50ec4ddc0b). Older checkouts fail configuration explicitly.
The public header compiles as C or C++11; only the implementation requires C++20.
The core's own MapData/size types never cross the ABI. Handles are uint64_t rather
than pointers cast to long, avoiding Windows LLP64 versus Apple LP64 assumptions.
All operations on a map belong to its owning Fusion thread; the registry mutex
protects handles, not concurrent access to a map. Each call retains a shared owner.

The preferred per-plugin entry point is now `iOS/FindTheWay/build.py` at the
repository root. The lower-level command below remains available. Both use
CMake with Ninja/Make and Apple command-line packaging tools.

## Build on Apple Silicon

Requires Xcode selected through xcode-select, Apple SDKs, Python 3, and CMake >=3.22.
Select a minimum OS supported by the actual shared-core libc++ features; the old
exported project's iOS 12 setting is not evidence that the core supports iOS 12.

```sh
python3 build_xcframework.py \
  --build-dir /tmp/ftw-apple \
  --output /tmp/FindTheWayCore.xcframework \
  --minimum-ios "$FTW_MINIMUM_IOS"
```

Run from this directory. Supply --core-include when validating an isolated shared
header proposal. Only iphoneos arm64 and iphonesimulator arm64 are built. The
script preserves existing output and publishes only after both builds/package
creation succeed. If rebuilding, choose a new output path. The Apple build is
not yet verified in this Linux execution environment.

Add the XCFramework to the generated App target's Link Binary With Libraries,
select Do Not Embed, include FindTheWayCore.h, and link Apple libc++ and iconv.
Follow the existing static turbojpeg XCFramework integration. Keep existing
Runtime language and ownership settings. Packaging/install/export automation
and the C ABI's remaining operations are subsequent milestones.

## Host verification

```sh
cmake -S . -B /tmp/ftw-host -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/ftw-host
ctest --test-dir /tmp/ftw-host --output-on-failure
```

The C caller test checks orthogonal/diagonal paths, walls and gaps, buffer sizing,
map round trips, traditional/isometric coordinates, costs, failed-output preservation, invalid cells,
invalid layers, overflow-sized dimensions, stale handles, and idempotent release.
It uses actual shared core code, not an algorithm mock. Linux GCC 14 host build and
this test passed against isolated headers read from the recorded master commit.
The expanded C caller test also passed with ASan/UBSan; leak detection was disabled
in this execution environment. This is not Apple build, signing, or device evidence.

## Runtime helper synchronization

Implement extension-owned iOS helpers against both Windows and current Android
helpers; do not edit the private SDK helper or assume its API is equivalent.
Reference sources:

- Windows: _3rdLib/ObjectSelection.h and ObjectCreation.h.
- Android: _3rdLib/ObjectSelectionClass.java and ObjectCreationClass.java.
- iOS: SDK ObjectSelection.h/.m, CEvents.m creation actions, and CRun APIs.

Initial iOS helper implementation now lives in `../Helpers`; see its README for
implemented methods, Runtime verification, and pending Android synchronization.
The following table records the full parity target, not completed validation:

| Area | Semantics to preserve and verify on both mobile platforms |
| --- | --- |
| Selection | OI handles versus OI-list indices, qualifier expansion, event-count validity, empty SOL clearing, negated filtering and Runtime return convention |
| Iteration | All/selected/fallback modes, destroyed-object policy, nested events, selection/action-state restoration where Windows requires it |
| Creation | Position parameter decoding, layer validation including -1, name-to-OI lookup, failed creation, newly created object selection |
| Rendering/physics | Hidden layers, sprite flags, physics attractors/bodies, normal-object registration; inspect actual Runtime paths |

Both the iOS SDK and Windows vector SelectObjects overload return early for an
empty list. Explicit replacement/clear APIs must distinguish that desktop no-op
contract; do not silently change existing callers. Current master
9ff0b78a has corrected Android slot-zero handling and the layer sentinel. Preserve
that update and continue paired scenario checks. Do not index
layers[-1] when the valid default-layer sentinel is used. The iOS CEvents.m path
also performs physics setup beyond f_CreateObject and evt_AddCurrentObject;
a helper calling only those two methods would be incomplete.

Shared helper semantics and paired regression scenarios should be maintained
together in the Android/iOS documentation. Platform field access stays in each
adapter. New helper changes must not silently alter existing Android behavior.
