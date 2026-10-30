# iOS Runtime object helpers

`FTWObjectHelpers.h/.mm` are extension-owned Objective-C++ helpers. They use
Runtime declarations locally and do not modify or redistribute SDK source.
They require C++11 and manual-reference-counted Runtime settings. They are not
linked into the placeholder extension yet and have not been compiled on Apple.

Selection implements qualifier expansion, event-count-aware all/selected/fallback
iteration, explicit SOL replacement/clearing, and negated filtering. ForEach takes
an initial snapshot of object slots and creation IDs, then resolves each object
before invoking a callback. Objects created later are not visited; removed or
replaced slots are skipped. Callbacks may cause nested events, but must keep the
run header and helper owner alive. Filter predicates must not generate events or
mutate the Runtime. This is an explicit restriction, not a general event sandbox.
Qualifiers must come from valid Runtime event parameters: the SDK does not expose
the qualifier array length for arbitrary-ID validation.

Creation accepts resolved parameters, clamps layers to [-1, nLayers-1], accepts
object slot zero, adds the new object to event selection, registers physics,
and applies hidden-layer visibility. Name lookup uses the current frame OIL and skips leading U+FFFF markers.
Creation by name without an HFII requires an explicit layer. Position
parameter decoding, backdrop creation, scope save/restore and action-state guards
remain to be implemented before full FindTheWay ACE integration.

## Cross-platform parity

Windows ObjectSelection.h/ObjectCreation.h define the reference behavior; inspect
current Android ObjectSelectionClass.java/ObjectCreationClass.java together with
the mobile Runtime implementation before changing either side.

| Scenario | iOS helper behavior | Android synchronization work |
| --- | --- | --- |
| Empty explicit replacement | Clears affected SOLs | Check explicit clear versus SelectObjects no-op callers |
| Desktop empty SelectObjects overload | Caller must preserve desktop no-op explicitly | Preserve existing caller contract |
| Unfiltered OI | Iteration falls back to all objects | Check event-count validity and selected-only mode |
| Qualifier | Expands distinct concrete OI-list indices | Use identical qualifier scenarios |
| Negated condition | Retains predicate XOR negate, returns nonempty XOR negate | Preserve Runtime condition-negation convention |
| Destroyed objects | ForEach excludes; filtering defaults to including | Match Windows flags rather than one global policy |
| Nested iteration | Snapshot slot+creation ID, resolve before callback | Compare mutation/replacement scenarios |
| Creation result zero | Treated as success | Current master accepts slot zero; retain paired checks |
| Default layer -1 | No layers[-1] access | Current master guards sentinel; keep paired checks |
| Physics creation | Attractor/body/normal-object hooks follow Runtime creation | Android helper retains its pre-existing direct-create behavior; physics remains a platform difference |

The empty-input difference requires care: the Windows vector SelectObjects
overload also returns without clearing. Earlier investigation notes described
the SDK early return as a missing feature; it is not by itself a Windows parity
bug. The owned helper deliberately names explicit clearing/replacement separately.
Master 9ff0b78a already contains an Android helper parity update. This PR preserves
those sources; it does not claim new Android changes or validation. The table
tracks pending paired checks rather than claiming completed synchronization.

## Apple verification

Run on an Apple Silicon Mac with the supplied exported project's Runtime sources:

```sh
python3 check_helpers.py --runtime-project /path/to/Proj
```

This syntax-checks the actual implementation against private SDK declarations,
without compiling or publishing the SDK. It must be followed by Runtime tests:
normal/qualifier objects, stale/current/empty selection, negation, destroyed
objects, nested events deleting/replacing objects, successful slot-zero creation,
failed creation, layer -1, hidden layers, and physics-enabled objects.
