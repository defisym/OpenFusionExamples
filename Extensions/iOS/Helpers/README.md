# iOS Runtime object helpers

`FTWObjectHelpers.h/.mm` are owned Objective-C++ helpers, packaged and used by
FindTheWay's Runtime adapter. They require C++11 and manual reference counting.
They do not redistribute or modify private SDK code.

Selection supports qualifiers, event-count-aware all/selected/fallback traversal,
SOL replacement/clearing, and the Windows filtering/negation convention. A foreach
snapshot uses object slots and creation IDs; removed/replaced objects are skipped,
and objects created later are not visited. Explicit replacement with an empty
list clears the SOL. Callers implementing the desktop vector overload's empty
no-op must retain that distinction.

`ftw_ios_generate_event` preserves selection and action-loop state across nested
immediate events. Generation checks prevent a reused slot from selecting a new
object during restoration. Predicate filters must not dispatch events or mutate
object lists. All calls execute on Fusion's owning thread.

Creation looks up the current frame's OI handles by name, skips U+FFFF name markers,
clamps layers to [-1, nLayers-1], accepts slot zero, selects newly created objects,
registers physics and applies hidden-layer visibility. The adapter decodes copied
position/create parameters using SDK `read_Position`, preserving direction,
parent/repeat behavior, HFII, OI and layer sentinels. Grid backdrops use the SDK's
separate backdrop API.

Compare portable semantics with the maintained Windows helpers; use the actual
iOS Runtime's types and calling conventions for platform details. Qualifier IDs
must originate from valid Runtime event parameters; the SDK does not expose the
qualifier-array length for validating arbitrary external IDs.

Check both helpers and complete adapter against an exported project's SDK:

```sh
python3 Extensions/iOS/Helpers/check_helpers.py --runtime-project /path/to/Proj
```

Run on Apple Silicon with selected full Xcode. This is a declaration/syntax check;
it must be followed by actual export, link and Runtime tests, including physics,
hidden layers, parent positions, qualifiers, empty selections and nested events.
