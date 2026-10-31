---
name: android-runtime-parity
description: Maintain Android Fusion runtime parity by checking Windows selection/creation helpers, using native application storage and file access, and synchronizing shared media dependencies.
---

# Android runtime helper parity

Use this skill whenever updating Android extensions, Android exporter/runtime
integration, or the shared object selection/creation helpers. Also use it when
changing their Windows counterparts so Android does not silently fall behind.

## Implementation boundary

Treat **C++ as the first-class implementation** for existing C++ extensions.
Reuse their algorithms, playback state, queues, caches, synchronization and public
abstractions across platforms. Use conditional compilation to isolate necessary
platform dependencies; avoid a separate Android business-logic implementation.
JNI is a boundary to that shared C++ implementation, not a reason to replace it.

Keep actual Fusion Android runtime access in Java. Selection chains, event/action
state, `CRunApp` storage/files and renderer calls manipulate Java runtime objects;
do not import Windows pointers/layouts or add JNI solely to imitate those APIs.
Use Java/Android APIs where required by the platform, with thin JNI marshalling
around the reused C++ core. A platform-specific adaptor is appropriate; a duplicate
portable engine is not.

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

## Application-owned extension state

For every port, inspect Windows `CreateRunObject`, `DestroyRunObject`, `StartApp`
and `EndApp`, including `GetExtUserData`/`SetExtUserData` ownership. A Fusion
**global object flag does not keep the Java extension instance or its JNI handle
alive**: Android `killRunLoop` saves global values and then destroys frame objects.
`saveGlobalObjectsData`/`loadGlobalObjectsData` restore values, not private extension
fields or native resources.

If Windows retains shared state until EndApp, make the Android state application
owned, keyed by the actual `CRunApp`, independently of instance reference counts.
Destroying the last frame instance must only detach it. Preserve playback, channel
settings, caches, source leases and periodic maintenance through frames with no
instances. Release on application exit, reset on Restart Application, and isolate
sub-applications while allowing intentionally shared platform devices. Keep user
pause separate from application suspension; handle backgrounding and exit even
when no extension instance remains.

Use Fusion's **native extension global storage** first: subclass
`Extensions.CExtStorage`, acquire via `CRun.getStorage(id)`, publish via
`addStorage(data, id)`, and remove via `delStorage(id)`. These are already scoped
to `rhApp.extensionStorage`; do not build a parallel static application registry
or manufacture global objects/storage markers. Reuse the extension identifier
and reject ID/type collisions. Consult the supplied runtime and SDK documentation
before inventing a global-state mechanism.

The supplied `CExtStorage` has no cleanup/update callbacks and is not cleared by
`CRunApp.killGlobalData()`. Add only the lifecycle adaptation actually needed for
native resource release and maintenance; do not replace exporter runtime classes.
Observe existing fields read-only when callbacks are unavailable, with documented
semantics and tests for transitions consumed between observation ticks. SDL Mixer
observes `CRunApp.playerNames` identity: `initGlobal()` replaces that array on each
Restart Application, including subapps sharing values, while ordinary frame changes
and Restart Frame preserve it. Also observe `CRunApp.file == null` after
`endApplication()`: subapp destruction can call it without leaving SL_QUIT set.
Do not write `adGO`, global values or player data
to create an artificial lifecycle token.

Add regressions for last-instance destruction and recreation, a frame without the
object, frame restart versus application restart, exit without a surviving
instance, parent/sub-application isolation and cleanup, source lifetime, and pause
restoration. Distinguish fixture coverage from a full Fusion Runtime APK test.

## Shared dependency version selection

Check Windows dependency deployment recipes before choosing Android versions.
Use the same authoritative version/revision selection, with platform-specific
SDK paths, toolchains, features and archive hashes. In particular Android FFmpeg
must read `Extensions/_3rdLib/FFmpeg-build/FFmpegVersion.cmake`, used by Windows;
do not repeat a release constant in the Android builder. Validate cached SDK
version, corresponding-source hash and requested ABIs before using/publishing
it. Version updates must invalidate stale SDKs rather than silently reuse them.

For media ports, prefer `CRunApp.openHFile()` for Fusion embedded files and
OBB/platform resolution. Keep ContentResolver URI grants and Android scoped-storage
semantics intact; do not invent filesystem paths for document URIs or assume a
Windows path exists on-device. Share source materialization/AES code, retain files
until native readers close, and scope caches through native application storage.

When two extensions share an audio backend, ship one shared native runtime and
matching Java/vendor bindings. Do not replace a process-wide music callback when
the other extension needs Mixer X music streams. FFmpeg's PCM chunk/effect channels
can coexist with SDL_Mixer's music streams, following Windows SMI. Verify device
ownership, independent pause/closure, seek queue invalidation, audio-clock progress
and DSP tail drain together; ensure the overlapping files in both export ZIPs match.

## Counterexample: a working Android rewrite is not a shared-core port

In the initial FFMpeg Android implementation, the existing
`Extensions/_3rdLib/FFMpeg.h`, `FFMpegAdapter` factory and Windows `CopyAdapter`
architecture were left outside the Android build. A new
`Extensions/FFMpeg/Android/cpp/Player` implemented its own demux/decode scheduling,
queues, seek, draining and clocks. Java/JNI/rendering wrappers and extensive
playback tests made that backend functional, but the portable logic existed in
parallel and could diverge when the Windows implementation changed. Matching ACE
IDs, FFmpeg versions and observable test cases did not satisfy the requested
architecture. This was an implementation mistake, not a necessary Android limit.

Do not repeat that approach or describe it as completed architectural parity.
Windows CNG/D3D/Fusion dependencies justify isolating those dependencies; they do
not justify rewriting the surrounding portable playback core. Moving the duplicate
engine to a directory named "shared" also does not fix this if Windows continues
executing the old implementation.

Before implementation and before declaring a port complete:

1. Inventory the existing C++ core, adaptor interfaces/factories, policies and
   dependencies. Explain which original modules Android will actually compile and
   call, and which specific platform APIs require replacement.
2. Reuse/refactor the existing core. Add platform adaptors through its established
   interfaces and factories; guard platform headers/implementations with macros.
   Preserve Windows call compatibility. Share portable fixes and regression tests.
3. Review the real call chains and build source lists for both platforms. Check
   for duplicate decode loops, state machines, seek/EOF handling, clocks and
   channel/cache policies hidden behind different filenames or languages.
4. Validate shared C++ behavior as well as platform integration. Passing Android
   unit tests or an independent APK does not establish reuse or Windows parity.
   Report any independently reimplemented behavior explicitly, with a concrete
   platform reason; do not claim a stronger result than the code demonstrates.

For FFMpeg, the intended direction is a common C++ playback core using the existing
`FFMpegAdapter`/`CopyAdapter` design, with MediaCodec/GLES platform implementations
and native Fusion Java bindings. Encrypter remains deferred until its agreed
refactoring/resumption point; this counterexample does not authorize its port.
