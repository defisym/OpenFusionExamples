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

Compare behavior against the maintained Windows and Android helpers, including
qualifiers, event stamps, slot zero, negation and layer sentinels. The explicit
replace API clears empty selections; the desktop vector SelectObjects overload
may be a no-op. Filter predicates must remain pure. No scope/action-state guard
is implemented yet.

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
