---
name: android-runtime-parity
description: Maintain Android Fusion object selection and creation helpers, checking the current Windows implementation for changes on every Android extension or runtime update.
---

# Android runtime helper parity

Use this skill whenever updating Android extensions, Android exporter/runtime
integration, or the shared object selection/creation helpers. Also use it when
changing their Windows counterparts so Android does not silently fall behind.

## Implementation boundary

Keep Android runtime access in Java. These helpers manipulate Java objects,
selection chains and event/action state directly; do not introduce JNI just to
reuse the Windows helper classes. Pure computational plugin algorithms can have
a separate native core when justified by that plugin's requirements.

Windows defines the shared helper capabilities and filtering behavior. Preserve
Android-specific runtime representations and creation entry points. Port the
behavior through Android's actual runtime structures, rather than copying native
pointers, layouts or call tables.

## Required check on every update

1. Inspect the **current checked-out Windows implementation**, including local
   changes, before finishing any Android extension/runtime update:

   - `Extensions/_3rdLib/ObjectBase.h`
   - `Extensions/_3rdLib/ObjectSelection.h`
   - `Extensions/_3rdLib/ObjectCreation.h`

   Compare it with:

   - `Extensions/Android_Gradle/app/src/main/java/_3rdLib/ObjectBaseClass.java`
   - `Extensions/Android_Gradle/app/src/main/java/_3rdLib/ObjectSelectionClass.java`
   - `Extensions/Android_Gradle/app/src/main/java/_3rdLib/ObjectCreationClass.java`

2. Read the last reviewed Windows revision in
   `Extensions/Android_Gradle/tests/object_helpers/README.md`. From the repository
   root, use `git diff <reviewed-revision> --` followed by the three Windows paths
   above to include committed and uncommitted differences. Use `git log` for
   context when needed. If the revision is unavailable or helper dependencies
   changed, directly compare the current implementations; do not assume parity
   merely because history is missing. This check does not require fetching or
   switching branches.

3. If Windows gained or changed portable filtering behavior or helper APIs,
   synchronize Android in the same update. Check iteration modes, qualifiers,
   negation, destroyed objects, selection/event stamps, scope/action restoration,
   object lookup and creation OI conversion. Inspect called helpers too, not just
   method signatures. Do not defer a portable addition just because the current
   Android plugin does not use it yet; later plugins may depend on it.

4. Verify assumptions against the supplied Android runtime, especially
   `Services/ObjectSelection.java`, `Events/CEventProgram.java`,
   `RunLoop/CRun.java` and `Actions/ACT_CREATE.java`. Distinguish OI handles from
   OI list indices, accept object slot zero, and handle signed qualifiers and
   empty SOLs. `Services/ObjectSelection` is exporter-provided; maintain the
   extension helpers in `_3rdLib` unless the task specifically requires an
   exporter change.

5. Preserve existing Android call compatibility. Document intentional platform
   differences and runtime-specific fixes in the parity README. Do not copy a
   Windows bug solely to make the code look identical. If a feature has no
   Android runtime equivalent, record the concrete limitation.

6. When helper behavior changes, add focused regressions and run:

   ```sh
   python3 Extensions/Android_Gradle/tests/object_helpers/run.py
   git diff --check
   ```

   These are Java 8 compatibility and fixture-based behavior checks. Report full
   Android build/device validation separately when available. Do not present
   fixture results as a complete runtime integration test.

7. After completing the comparison, update the reviewed Windows revision in the
   parity README if its reference files changed. If reviewed Windows changes are
   still uncommitted, retain the prior committed revision and record that pending
   comparison instead of claiming HEAD includes them. In the completion report,
   briefly state whether Windows helpers changed, what was synchronized, any
   retained platform differences and the validation performed.
