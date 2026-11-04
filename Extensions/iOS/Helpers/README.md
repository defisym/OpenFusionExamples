# iOS Runtime object helpers

These C++11 Objective-C++ headers provide general Fusion Runtime functionality
for any iOS extension. They have no FindTheWay or native-core dependency:

- `ObjectBase.h`: `FusionIOS::ObjectBase`, shared OI/object lookup, name
  comparison, existence checks and OI-list-index to creation-handle conversion.
- `ObjectSelection.h`: `FusionIOS::ObjectSelection`, qualifiers, selection,
  filtering, counts, type/identifier checks, iteration and selection/action scopes.
- `ObjectCreation.h`: `FusionIOS::ObjectCreation`, creation parameters,
  SDK position resolution, name-based OI lookup and post-creation setup.

Selection and creation are independent classes deriving from ObjectBase. Their
names and portable operations follow Windows `Extensions/_3rdLib/ObjectBase.h`,
`ObjectSelection.h`, `ObjectCreation.h` and Android's corresponding `_3rdLib`
classes. The Windows references were checked at `9ff0b78a`; they are unchanged
in this port. Use the current Windows implementation for future parity reviews.

The filenames match Windows. Package the three headers in the export root's
`Helpers/` directory, separate from the SDK's own `Classes/Services/ObjectSelection.h`.
Runtime sources include `../../Helpers/ObjectSelection.h` or ObjectCreation.h;
do not add this directory to the SDK's global recursive header search paths.
All implementations are inline: use one consistent revision, with no helper
compilation unit or shared library.
Multiple extensions can include them without duplicate strong definitions. The
extension packager generates extension-specific PBX IDs for shared headers.

```objc
#include "ObjectSelection.h"
#include "ObjectCreation.h"

FusionIOS::ObjectSelection selection(run);
selection.ForEach(oiList, [&](CObject* object) {
    // Use the current selection, or all objects if this event has not filtered it.
});

FusionIOS::ObjectCreation creation(run);
creation.OCCreateObject([&](FusionIOS::ObjectCreation*,
                          FusionIOS::ObjectCreation::CreateParams& params) {
    params.oi = creation.GetCreationOI(@"Marker");
    params.x = x;
    params.y = y;
    params.layer = 0;
});
```

## Selection and event scopes

`SelectAll`, `SelectNone`, `SelectOneObject`, `SelectObjects`, `FilterObjects`,
`GetFirstObject`, `Selected`, `ObjectIsSelected`, `ObjectIsOfType`,
`GetNumberOfSelected`, `GetNumberOfObject`, `IterateObjectWithIdentifier` and
`SaveSelectedObject` are available without plugin-specific predicates.
`ForEachFlag_ForceAll` takes precedence over `ForEachFlag_SelectedOnly`.
`Selected` requires a nonempty current selection; `ObjectIsSelected` also accepts
an explicitly empty selection. Slot zero is valid. Qualifier pairs use the OI
list index, not the creation OI handle.

`Snapshot`/`Replace` use slots plus creation IDs. Iteration skips removed/reused
objects and does not visit later creations. An empty replacement or SelectObjects
clears the SOL, as on Android; Windows's vector overload currently treats empty
input as a no-op. Filter predicates must not dispatch events or mutate lists.
Destroyed objects are excluded before predicate/negation when requested.

`SaveScope`/`RestoreScope` save selections; `Scope::RestoreActionState` restores
action-loop state explicitly. `KeepScopeCall(true, callback)` restores both even
when an Objective-C exception escapes the callback. Apple Objective-C++ also
supports C++ exception unwinding. Empty selections, stale event stamps and
generation IDs are preserved; the current Runtime event counter is not rolled
back. Qualifiers must come from valid Runtime parameters because the SDK exposes
no qualifier-array length. All calls execute on Fusion's owning thread.

## Creation and platform differences

`ResolvePosition` decodes a copied SDK CreateDuplicateParam through read_Position;
the caller applies repeat flags to its action. `Create` accepts resolved parameters
and `OCCreateObject` initializes a local parameter set for each callback, including
nested creation. Creation selects the result, registers physics and applies
hidden-layer visibility. Layers are clamped to [-1, nLayers-1]; by-name creation
with HFII -1 requires an explicit layer. Names skip leading U+FFFF markers and
retain mobile case-insensitive comparison; Windows's StrEqu is case-sensitive.
Grid backdrops use a separate SDK API in the consuming extension.

The Runtime and its objects remain borrowed. Do not retain these helpers after
the Runtime is destroyed, share them across threads, modify the exporter helper,
or expose Runtime objects through a plugin's native-core ABI.

## Validation

The standalone fixture compiles two translation units using these headers and
Foundation, without FindTheWay sources, headers or a native library:

```sh
python3 Extensions/iOS/tests/run_runtime_fixture.py --helpers-only
```

It covers sparse OI handles, qualifiers, slot zero, counts, selection modes,
destroyed objects, filters, scopes/exceptions, slot reuse, nested creation and
layer sentinels. Linux uses GNUstep; its legacy Objective-C runtime cannot run
the C++ exception check, which is enabled on Apple only. These owned API models
do not replace tests with the actual iOS Runtime.

Check the headers against an exported project's SDK on Apple Silicon:

```sh
python3 Extensions/iOS/Helpers/check_helpers.py --runtime-project /path/to/Proj
```

Use `--runtime-source /path/to/extension/Runtime` and repeatable `--include`
arguments to check a consuming extension too. Follow with actual export, link
and Runtime checks, including parent/repeat creation, physics and hidden layers.
