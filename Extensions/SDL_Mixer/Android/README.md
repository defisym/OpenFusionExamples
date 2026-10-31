# SDL Mixer Android JNI port

The Android extension is a thin JNI binding over **SDL Mixer X**, with the same
7 conditions, 17 actions and 14 expressions as the Windows object. Fusion remains
the host: the port does not start SDLActivity or initialize an SDL video window.

## Architecture and sharing

```text
Fusion CRunSDL_Mixer -> Java SDLMixer -> libofe_sdl_mixer.so
                                           |       |
                                    libofe_audio.so SoundTouch (static)
                                           |
                                  libSDL2_mixer_ext.so
                                           |
                                      libSDL2.so
                                           |
                               SDL's Android audio driver
```

`libofe_audio.so` owns reference-counted Mixer X device access, focus and application
suspension. Its version-2 C ABI is in `cpp/AudioRuntime.h`; FFmpeg dynamically links the same library and queries its actual output
specification. PCM chunk/effect streams follow Windows SMI, with per-stream
queues, pause/volume, output-rate conversion and SoundTouch tempo. Do not compile another copy of this singleton into either plugin.
No AudioTrack or alternate mixer is implemented by this extension. SDL's stock
Android backend and Java audio bindings handle platform output. The default is
SDL2's stock `android` driver (JNI-backed AudioTrack), selected with default hint
priority so explicit SDL hints/environment settings can override it. The older
API-23 emulator image's default OpenSL ES fast-track path crashed its system
mediaserver; the stock `android` path passed the same playback checks.

The native mixer owns the Windows object's channel policy: exclusive replacement
with optional crossfade, overlapping streams in a mixing channel, per-channel and
master volume, panning/position/distance, AB loops and repeated-note score pitch.
Paused outgoing exclusive streams are halted before replacement so they cannot
remain indefinitely paused while waiting for a fade to finish. Both channel types
use Mixer X's **music-stream** API; they do not compete for SDL
chunk-channel indices. Fusion instances acquire application data through the native
`CRun.getStorage/addStorage` API (`SDLMixerStorage extends CExtStorage`, ID SMIX).
Destroying an instance only detaches it: playback, settings, binary caches and
source leases remain in Fusion's application storage, including frames without
an SDL object. Sub-applications have independent channel/cache state while sharing
the Mixer X output device. Music objects are halted/freed before the application's
last device reference is released; one application's exit cannot close another's output.

The supplied `CExtStorage` has no lifecycle/update callback. A 50 ms main-Looper
maintenance task handles AB loops, completed sources and cleanup even without
instances. It reads existing application state; there is no parallel global
registry and no artificial global-object marker. Restart detection reads
`CRunApp.playerNames` identity: `initGlobal()` allocates a new array on every
Restart Application (also for subapps sharing values), whereas frame changes and
Restart Frame preserve it. Ancestor restarts/exit invalidate descendants.
Activity lifecycle callbacks handle background/resume/destruction; SL_QUIT or
restart is cleaned up on the next maintenance pass, or synchronously before the
next instance acquires storage. Storage is removed using `delStorage` when a
current CRun exists, otherwise from the owning application's native storage list.
No exporter runtime classes or global/player values are replaced or modified.

JNI handles are registry IDs, never native addresses. Audio callbacks do not call
Fusion or JNI. Completed streams are reclaimed by instance updates and application maintenance.
SoundTouch processors and scratch buffers are private to each pitched stream,
with state retained across callbacks. User pause is independent from application
suspension and audio focus. Each application suspends its own streams, so another
application can continue rendering; resuming preserves user pause. The current focus policy pauses on transient loss or
ducking and resumes on gain; it does not change individual stream pause flags.

`MediaSource.Resolver` supplies application-owned common file/asset/content-URI/HTTP(S) materialization and
Win32-compatible AES/CBC key derivation. Both Fusion adapters use `FusionFiles`/`CRunApp.openHFile` for embedded
binaries, OBB and platform file resolution. FFmpeg now shares Mixer X output.
See [the combined build/file guide](../../Android_Gradle/Media-JNI.md) for URI
grants, scoped storage, installation and standalone/shared dependency handling. Extracted files remain leased while Mixer X streams read
them. Binary cache updates retain old readers, and release skips referenced data.

## Upstream Android compatibility

The manifest pins the same revisions as the Windows dependencies:

| Component | Revision | Notes |
| --- | --- | --- |
| SDL2 2.32.10 | `5d249570393f7a37e037abf22cd6012a4cc56a71` | Stock Android JNI/Java bindings and audio drivers |
| Mixer X 2.8.0 | `72139aa7332062de541de50708742fe4ee2ae05c` | CMake supports Android; multi-music APIs have no Windows-only restriction |
| AudioCodecs | `bb11ca3cc1d7597b23a530d519b3e636dceb7b23` | Same static codec suite and LGPL/non-GPL selection as Windows |
| SoundTouch 2.3.3 | `e83424d5928ab8513d2d082779c275765dee31b9` | Native core, statically linked into the JNI extension |

The Mixer X maintainer explicitly confirms the Android CMake build in
[issue 59](https://github.com/WohlSoft/SDL-Mixer-X/issues/59#issuecomment-1279765831).
The missing Android.mk is intentional. The upstream master source archive inspected
on 2026-10-31 matched the pinned Mixer X tree; this port does not require an upgrade.
SoundTouch's download uses a GitHub mirror retaining the exact upstream commit;
its Codeberg origin and archive SHA-256 are recorded in `dependencies.json`.

SDL's full Android build dependencies remain compiled because its stock JNI method
registration references joystick/haptic and video symbols. Runtime initialization
is restricted to AUDIO and TIMER. The matching upstream `org.libsdl.app` Java
classes are packaged unchanged, but no SDL Activity is declared in the manifest.
Native registration depends on those exact class/method names; R8 rules retain them.
The development CMake wrapper supplies an SDL2 package discovery shim and avoids
exporting an SDL2main application dependency. No upstream checkout is modified.

Actual playback checks exposed two upstream issues:

- The pinned crossfade helper combines FadeOut's `1` success convention with
  FadeIn's `0` success convention incorrectly. The wrapper calls those same two
  primitives directly and checks the incoming stream's start result.
- PCM WAV seeking aligns offsets to a whole second instead of a sample frame.
  `cpp/MixerXFixes.cmake` compiles an explicitly marked corrected source copy with
  that one-line alignment change. The original source tree remains untouched;
  both the original archive and correction recipe accompany binary packages.

On the API-23 x86_64 software emulator, Clang's automatically vectorized
SoundTouch coefficient setup faulted when its allocation ended at a page boundary.
The Android x86_64 build scalarizes that one setup translation unit; explicit SSE
playback kernels remain enabled. This is a reproducible emulator compatibility
workaround, not a claim of a physical-CPU defect.

The host GCC 14 build additionally needs `alloca=__builtin_alloca` for upstream
stb_vorbis; Android Clang does not need that definition.

## Compatibility boundaries

- Android API 21+; SDL2/Mixer X/SoundTouch builds cover all four export ABIs.
- The codec profile matches Windows' LGPL/non-GPL selection: built-in WAV/AIFF,
  MP3, Ogg Vorbis, FLAC, QOA and PXTone; static AudioCodecs adds Opus, WavPack,
  ModPlug/libXMP trackers, GME, EDMIDI and FluidLite alongside Timidity MIDI.
  Windows-only native MIDI is disabled. Soundfonts/instrument data are not bundled.
  The builder stages SDL headers/library, builds AudioCodecs, then links Mixer X.
  For API-21 32-bit targets, WavPack's FILE seek probe and Opusfile's generated
  stdio configuration avoid API-24-only large-file stdio; Mixer X uses their
  64-bit SDL RW callback interfaces. The original source archives stay untouched.
- Raw desktop pointer expressions 6 and 8 consume parameters and return an empty
  string/zero. Handled-memory expression 7 works through bounded, versioned opaque
  memory names; it never truncates an address into a Fusion integer.
- Negative/oversized channel IDs and invalid memory ranges are rejected. Channels
  are bounded to 0..4095. Failed exclusive opens stop/fade the prior stream, as
  Windows does.
- Two Windows condition mistakes are corrected here: All Channels Paused tests
  all active streams (true when none are active); mixing-channel No Output tests
  playing state and pause state separately. Stored channel effects/master volume
  apply to newly opened streams. These intentional differences are regression tested.
- The master volume remains scoped to this extension's managed streams. It does
  not silently become an application-wide gain for other Mixer X clients.
- File opening, decryption and cache loading are synchronous. SoundTouch pitch
  adds processing latency; a short pitched stream's DSP tail is not drained after
  the underlying Mixer X stream ends. Device hearing/latency quality needs separate
  physical-device validation.

## Build and publish

Python 3.12+, NDK r28c and CMake 3.22+ are required. The dependency build is CMake
only and works with the appropriate host NDK; Windows uses Ninja from SDK discovery.
All archives are pinned and SHA-256 checked before extraction.

```sh
python3 Extensions/_Script/SDL_Mixer.py
# Explicit tools and reusable dependency/build caches:
python3 Extensions/_Script/SDL_Mixer.py \
  --ndk /path/to/android-ndk-r28c --cmake /path/to/cmake \
  --sources /path/to/source-cache --build-root /path/to/native-builds
```

SDK discovery follows FTW (`ANDROID_HOME` / `ANDROID_SDK_ROOT`). The publisher emits
`SDL_Mixer/ToInstall/Files/Data/Runtime/Android/SDL_Mixer.zip`. `--abi` selects a
subset for development; omit it for release. The ZIP contains seven owned Java
sources, the matching SDL Java bindings, five native libraries per ABI, JNI/R8
rules, dependency archives, correction/build recipes and license notices.
It checks ELF machines, dynamic dependency closure and 64-bit 16 KB ELF alignment.
NDK 32-bit libraries retain their supported 4 KB alignment. Failed publishing
leaves the previous ZIP intact. Fusion runtime classes, manifests and Gradle
application templates are not replaced.

On Windows, `Extensions/_Script/SDL_Mixer_Android.bat` is the Android entry point.
`Run_Only Unicode` invokes it after the existing Windows build. Set
`/p:SDLMixerPackageAndroid=false` to build the Windows runtime alone. The existing
`SDL_Mixer.bat` release-archive workflow is retained.

For the repository's development Gradle application, prepare sources first:

```sh
python3 Extensions/SDL_Mixer/Android/build_android.py --prepare-only --sources /path/to/source-cache
# Add to the normal application Gradle build:
# -POFE_SDL_MIXER_SOURCES=/path/to/source-cache
```

## Verification

```sh
# Build the same sources for host playback/JNI checks:
python3 Extensions/SDL_Mixer/Android/build_android.py --host \
  --sources /path/to/source-cache --build-root /path/to/builds
python3 Extensions/Android_Gradle/tests/sdl_mixer/run.py \
  --build /path/to/builds/host --sdl-source /path/to/source-cache/SDL2-5d249570393f7a37e037abf22cd6012a4cc56a71 \
  --android-jar /path/to/android.jar
python3 Extensions/Android_Gradle/tests/sdl_mixer/compile_android.py --android-jar /path/to/android.jar
python3 Extensions/Android_Gradle/tests/object_helpers/run.py
```

The native PCM test captures actual Mixer X output and checks tempo duration,
preserved pitch, pause, seek reset and DSP tail under ASan/UBSan.
Host playback uses the real Mixer X decoders, effects and mixing callback with SDL's
dummy device; it does not establish Android output behavior. Tests cover six codecs,
crossfade cleanup, fractional seek/AB, user/app pause, shared client lifetime, cache
updates/readers, panning and SoundTouch output frequency. The production JNI methods
run under `-Xcheck:jni`; all 7/17/14 Fusion ACEs have dispatch/parameter tests. Set
`OFE_MIXER_SANITIZE=ON` on the host CMake build to instrument the owned C++ playback
checks with ASan/UBSan (disable LeakSanitizer under ptrace).

`tests/sdl_mixer/device/build_probe.py` builds a separate ordinary-Activity APK with
the production Java/JNI bridge. Its worker-thread probe exercises platform JNI
setup, audio callbacks, shared playback, seek/AB, suspension, AES and memory views.
Its storage probe additionally compiles narrow CRun/CRunApp/CExtStorage fixtures
with the production global-storage class and JNI bridge, checking empty-frame
AB maintenance, retained AES source leases, cache/settings, restart, subapps and
EndApp cleanup. This exercises real Android scheduling and audio; Fusion lifecycle
transitions in that probe are fixture driven.
It does not include the licensed Fusion runtime or replace a full Fusion export test.

Verified in the 2026-10-31 workspace: four-ABI Release builds and ELF/package checks;
real host Mixer X playback under ASan/UBSan; production JNI calls under `-Xcheck:jni`;
Java 8 Android compilation and all 7/17/14 ACE dispatch tests; unchanged Windows
object helpers with all 56 Android helper regressions passing. FFmpeg's existing
JNI/frame/audio tests also passed after extracting the common media-source helper.

The standalone APK passed on the Android 6.0/API-23 x86_64 software emulator using
production defaults, an ordinary Activity and initialization on a worker without
an Android Looper. It verified callback advancement for WAV/MP3/Ogg/FLAC/Opus/WavPack, shared
clients, crossfade, fractional seek/AB, application and user pause, AES, Unicode
handled-memory input, and device reopening after the last direct client closed.
The native-storage probe also passed with real Android scheduling/JNI: empty-frame
AB maintenance and AES leases, retained cache/settings, restart reset, independent
subapps, per-application suspension and EndApp cleanup without frame objects. No host
sound was emitted by that headless emulator. Physical-device audio quality,
Bluetooth/route changes, modern audio-focus restrictions, Windows MSBuild and a
full licensed Fusion Runtime APK have not been validated here.

The combined probe also verifies readable paths/file URIs, APK asset forms,
ContentResolver input, HTTP and Fusion embedded-file opener fixtures, with AES.
Real FFmpeg output shares the Mixer X device; GLES checks exercise software uploads
and the native MediaCodec Surface/OES path. See the combined guide for build/install
commands, native storage lifetime and Android file grants.
