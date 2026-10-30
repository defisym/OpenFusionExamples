# Android object helper parity

The production helpers live in `app/src/main/java/_3rdLib`. Their Windows references
are `Extensions/_3rdLib/ObjectBase.h`, `ObjectSelection.h`, and `ObjectCreation.h`.

Last reviewed Windows revision: `234676b7ed306f19a35ab6d69519a0ba6aa52598`.
The three Windows reference files had no local modifications at that review.
For every Android extension/runtime update, follow the repository's
[`android-runtime-parity` skill](../../../../.agents/skills/android-runtime-parity/SKILL.md)
to check the current Windows helpers and synchronize portable changes.

Android also ships `Services/ObjectSelection.java` in its exporter runtime. That
is the original selection helper, attributed to Anders Riggelsen, with lowercase
methods. `_3rdLib/ObjectSelectionClass.java` is a separately maintained extension
of that implementation. Existing extensions such as `CRunFindTheWay` use the
`_3rdLib` class. The exporter class is not modified by this parity update.

## Synchronized helpers

- Shared `ObjectBaseClass`: OI list/object lookup, name lookup, creation OI handle
  conversion, OIL existence checks and OIL iteration. An OI list index is not an
  OI handle; qualifier pairs contain the list index in the second element.
- `GetFirstObject`, `Selected`, `ObjectIsQualifier`, `ObjectIsSelected`.
  `ObjectIsSelected` tests the current event stamp, even for an empty SOL;
  `Selected` additionally requires a selected object.
- `ForEachFlag_Default`, `ForEachFlag_ForceAll`, `ForEachFlag_SelectedOnly`.
  ForceAll takes precedence. The existing boolean overload remains available.
  Default traversal uses the current SOL, or all objects if the type has not
  been filtered in this event. Traversal excludes destroyed objects.
- `FilterObjects(..., selectDestroyed)`, defaulting to `true` as on Windows.
  Destroyed-object exclusion occurs before the predicate and negation.
  Qualifier filtering visits every member, and the condition result remains
  `hasSelected ^ negate` for Fusion's negated-condition handling.
- `IterateObjectWithIdentifier`, `SaveSelectedObject`, `Scope`, `SaveScope`,
  `RestoreScope`, `KeepScopeCall`. Action state comes from Android's
  `CEventProgram`; saved SOLs are stamped with the current event on restore.
  `SaveScope`/`RestoreScope` handle selections, while action state is restored
  explicitly via `Scope.RestoreActionState` or automatically by `KeepScopeCall`.

## Runtime differences retained or corrected

- Android qualifier arrays and direct `CRun.f_CreateObject` calls are retained;
  Windows uses byte offsets and a native action call table. Backdrop creation
  (`OCCreateBackdrop`/`GetBackdropOI`) remains Windows-specific.
- Creation name lookup now uses the current frame's OIL as on Windows, rather
  than the application's entire OI bank. Android's existing case-insensitive
  name comparison remains; leading U+FFFF markers are skipped as on Windows.
  Windows's `StrEqu` is case-sensitive.
- Slot zero is valid in both runtime lists and creation results. Android
  traversal and creation now include it. `Selected` accepts zero even though
  the current Windows helper tests `> 0`.
- An empty `SelectObjects` clears the SOL, consistent with the Android runtime's
  restore logic. Windows currently treats empty input as a no-op. Scope restore
  must preserve explicitly empty selections.
- Missing links terminate iteration instead of hanging. Scope restoration
  ignores removed OILs and objects whose slots were deleted or reused.
  `KeepScopeCall` also restores state when the callback throws.
- Creation skips layer-array access for the `-1` sentinel, as `ACT_CREATE` does.
  The runtime resolves that sentinel from the supplied HFII; callers creating
  by name with HFII `-1` must supply an explicit layer. Creation parameters are
  local so a nested creation cannot overwrite its caller's parameters.
- Existing position decoding, layer clamping and sprite hiding remain Android
  implementations. This does not add the optional Box2D post-creation setup
  performed by Android's `ACT_CREATE`; the helper already called `f_CreateObject`
  directly, and physics parity is outside this selection-helper update.

## Regression checks

Run from the repository root with a JDK 9+ (compiles for Java 8):

```sh
python3 Extensions/Android_Gradle/tests/object_helpers/run.py
```

The runner compiles the actual three helper source files against small runtime
fixtures, then checks signed/unsigned qualifiers, sparse OI handles, slot zero,
iteration flags, destroyed objects, negated filters, nested scopes, empty SOLs,
exception restoration, removed/reused objects, nested creation and layer bounds.
It does not require Android SDK dependencies and does not replace a full Android
build or device/runtime integration test.
