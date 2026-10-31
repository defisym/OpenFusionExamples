# Android FFMpeg and SDL Mixer

Both extensions use JNI and **one SDL Mixer X audio device**, with the same SDL2,
Mixer X, AudioCodecs and SoundTouch revisions selected for Windows. FFmpeg's
release/source hash comes from the Windows `FFmpegVersion.cmake`. Fusion object,
event, file and rendering access stays in Java. The extensions do not replace
`RuntimeAndroid.zip`, the Activity, manifest or Gradle exporter templates.

## Build and install

Python 3.12+, NDK r28c and CMake 3.22+ are required. From the repository root:

```sh
# Linux/WSL/macOS with the corresponding host NDK: build FFmpeg's Android SDK.
python3 Extensions/FFMpeg/Android/build_sdk.py \
  --ndk /path/to/android-ndk-r28c \
  --output /path/to/ffmpeg-android-sdk

# These publishers also work on Windows with its Windows NDK and prepared SDK.
python3 Extensions/_Script/SDL_Mixer.py
python3 Extensions/_Script/FFMpeg.py --ffmpeg-sdk /path/to/ffmpeg-android-sdk
```

`ANDROID_HOME`/`ANDROID_SDK_ROOT` use FTW's SDK discovery. Supply `--ndk` and
`--cmake` together to use explicit tools. On Windows use `python` or `py -3`;
`SDL_Mixer_Android.bat` and `FFMpeg.bat` in `Extensions/_Script` are the entry points.
FFmpeg source compilation needs bash/make and the Linux/macOS NDK; ordinary Windows
Python cannot run that source build. Prepare the SDK under WSL using a **Linux**
NDK, with the output on a Windows-accessible path, then set `OFE_FFMPEG_ROOT` to
its Windows path before building the Visual Studio projects. The SDL/AudioCodecs
and JNI builds themselves use the Windows NDK normally.

Both publishers default to the same audio source/build cache under
`Extensions/SDL_Mixer/Android/out`. Custom caches must agree: SDL uses `--sources`
and `--build-root`; FFmpeg uses `--audio-sources` and `--audio-build-root`.
The FFmpeg SDK defaults to `Extensions/_3rdLib/FFmpeg-sdk-Android`, separate from
Windows' `FFmpeg-sdk`. Missing/stale SDKs are rebuilt on supported hosts; version,
source SHA and ABI validation prevents silently using another Android version.

The outputs are:

- `Extensions/SDL_Mixer/ToInstall/Files/Data/Runtime/Android/SDL_Mixer.zip`
- `Extensions/FFMpeg/ToInstall/Files/Data/Runtime/Android/FFMpeg.zip`

Copy the ZIPs **unextracted** into Fusion's `Data/Runtime/Android` directory, then
export the application normally. Each ZIP is standalone. When both are installed,
use ZIPs from the **same revision/build caches**: the shared Java sources, SDL
bindings, SDL2/Mixer X/audio libraries and libc++ overlap intentionally and must
match. Rebuild/reinstall both when updating shared dependencies. Runtime ABI 2
adds PCM streaming; an older `libofe_audio.so` cannot serve the new FFmpeg binary.

`Run_Only Unicode` publishes Android ZIPs after the Windows builds. To build the
Windows runtimes alone, set `/p:SDLMixerPackageAndroid=false` and
`/p:FFMpegPackageAndroid=false`. Existing SDL_Mixer Windows archive packaging is
unchanged. First-time source/codec builds can take several minutes.

For the repository's development Gradle app, pass
`-POFE_SDL_MIXER_SOURCES=/path/to/prepared/audio/sources` and
`-POFE_FFMPEG_ROOT=/path/to/ffmpeg-android-sdk`, after preparing the audio SDK
with its publisher/builder. For a custom audio build root, also pass
`-POFE_SDL_MIXER_BUILD_ROOT=/path/to/audio-builds`. Export ZIPs already contain their
native libraries, so normal Fusion exports do not rebuild dependencies.

## Reading files

Use the same path string in either extension's existing play/open/cache action.
No new Fusion object is needed for file access. The adapters use
`CRunApp.openHFile()` for Fusion embedded files and OBB/platform resolution, plus
Android's native file, assets, URL and ContentResolver APIs. Sources needing a
seekable native filename are materialized in private cache storage and retained
until the decoder finishes using them. AES keys use Windows' UTF-16/Win32 derivation.

| Input | Example | Requirement |
| --- | --- | --- |
| Fusion embedded binary | The original embedded file name/path, including a Windows path exported by Fusion | Include the file in the Fusion application; native `openHFile` resolves it |
| APK asset | `asset://music/tone.ogg`, `file:///android_asset/music/tone.ogg` | Include the file as an Android asset |
| Asset/`mmf` fallback | `tone.wav` | Matching asset, with an `mmf/` fallback |
| Ordinary local file | `/storage/emulated/0/Android/data/<package>/files/movie.mp4` | A real path the application can read |
| File URI | `file:///.../movie%20name.mp4` | Same permissions as the corresponding local path |
| Android document/media URI | `content://...` from a file picker or MediaStore | A valid read grant, or applicable media permission |
| HTTP/HTTPS | `https://example.org/movie.mp4` | Internet permission; Android's cleartext policy applies to HTTP |

On modern Android, `/sdcard/...` is not unrestricted desktop file access. Prefer a
Storage Access Framework document URI and retain its read grant when playback must
survive application restarts. URI permission acquisition/persistence belongs to the
file picker/exported application. The media extensions consume the supplied grant;
they do not request broad storage permissions or display an unsolicited picker.
If reading media through MediaStore, configure the appropriate Android media
permissions in the application. App-private and app-specific external files need
no broad storage permission. An arbitrary Windows filesystem path cannot refer to
a device file unless Fusion resolves it as an embedded binary.

Both extensions report open failures through their existing behavior/logs. Inspect
`OFE.FFMpeg`/`OFE.SDL_Mixer` in logcat for missing files, denied URI grants, bad AES
keys or unavailable decoders. File materialization, open and seek remain synchronous;
large files/downloads need enough private cache space and can block the run thread.
MIDI soundfonts/instrument configuration remain external data, as with Mixer X on
Windows; compiling the MIDI decoders does not bundle instrument banks.

## Playback and lifecycle

SDL Mixer keeps streams, channel settings, binary cache and source leases in
Fusion's native `CExtStorage` (SMIX). Destroying the last frame instance only
unbinds it. Playback and maintenance continue in frames without an instance;
Restart Frame preserves state, Restart Application clears it, and application exit
releases it. Subapps have independent state and share only the output device.

FFmpeg decoders/textures are per object, as on Windows. Its materialized-file cache
is application-owned native storage (FFMG). Closing an instance releases that
reader; cached files survive until erase, application restart or exit. Active
readers remain valid across cache erase. Shared lifecycle adaptation observes
existing runtime state (including the file cleared by `endApplication()`) and Activity callbacks because CExtStorage has no lifecycle
hooks; no fake global object or second global-state registry is introduced.

FFmpeg PCM uses independent Mixer X chunk/effect channels, following Windows SMI;
SDL Mixer uses Mixer X music streams. FFmpeg's JNI calls the original C++
`FFMpeg` class and its `FFMpegAdapter`/`CopyAdapter` factories, adding Android
MediaCodec/GLES adaptors beside the Windows implementations. Seek flushes shared
decoder/filter/resampler state. The original libavfilter atempo chain preserves
FFmpeg pitch; SoundTouch remains available to SDL Mixer. Native audio callbacks
drive the shared C++ media clock. Muted/no-audio video uses a monotonic clock. Focus loss freezes
shared audio; application suspension and user pause remain separate.

Hardware rendering uses MediaCodec -> SurfaceTexture -> an OES-to-2D copy on the
Fusion GL thread. Software rendering uses FFmpeg RGBA -> JNI -> GLES texture
upload. Fusion draws both through its native ITexture/renderer, ink effects and
shaders. Hardware initialization failure falls back to software. Android has no
Windows cSurface/native-address ABI: FFmpeg surface-pointer expressions return 0;
SDL raw-address expressions return an empty string/0. Handled-memory SDL input
uses bounded opaque names and remains supported.

See each extension's `Android/README.md` for dependency/build recipes and validation.
Full licensed Fusion exports and physical-device/route behavior still need user
validation; independent APK/fixture checks are not a full Fusion export test.

## Deferred Encrypter work

As agreed on 2026-11-01, further Encrypter Android work is paused until the
refactoring is complete. The current media decryption compatibility layer does
not constitute a port of the standalone Encrypter extension. Direct cross-platform
verification with the Windows Encrypter remains outstanding. Record and resume
that work via [Encrypter's Android status](../Encrypter/ReadMe.md#android-status--deferred-pending-refactoring);
do not expand the current media port into Encrypter work before then.
