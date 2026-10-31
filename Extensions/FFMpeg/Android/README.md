# FFmpeg Android JNI port

Fusion Java ACE dispatch → thin JNI binding → the original C++ `FFMpeg` class.
Android compiles `Extensions/_3rdLib/FFMpeg.h`, `FFMpeg.cpp` and the existing
`FFMpegAdapter` factory. There is no separate Android demux/decode/seek engine.
Windows keeps its existing public C++ entry points. Platform-independent fixes to
decoder/filter/resampler drain, fractional-second seek, buffer stride/size and
partial audio copies apply to both platforms. The Windows seek/preview policy now
lives in the shared class and is called by both Fusion bindings.

`FFMpegAdapterAndroid` adds MediaCodec decoder selection, initialization and Surface
buffer release beside `FFMpegAdapterD3D11`. `CopyAdapterAndroid` implements native
RGBA copies and GLES external-image copies through the `CopyAdapter` factory.
Windows headers and D3D implementations remain behind `_WIN32`; Android APIs stay
behind `__ANDROID__`. Actual Fusion runtime access remains in Java (`ITexture`,
`GLRenderer`, application storage/files and events). See
[the combined build/file guide](../../Android_Gradle/Media-JNI.md).

## Rendering and audio

- Hardware: FFmpeg's native NDK MediaCodec decoder outputs to a Java `Surface` backed by
  `SurfaceTexture`. Its external OES image is copied on the Fusion GL thread into
  an RGBA 2D render target, using the producer's transform matrix. Fusion draws
  that target through its normal renderer, including runtime ink effects/shaders.
- Software: swscale converts the decoded frame with its actual source strides to
  tightly packed RGBA. JNI copies it into a reusable direct buffer; the Android
  copy adapter uploads it to a regular 2D texture. Premultiplied output follows
  the application's `texPremultiply` setting. Source alpha is retained.
- Hardware initialization failure falls back to the software decoder. Requested
  hardware state and actual hardware state are reported separately. A decode
  error after initialization stops the instance and logs the FFmpeg error.
- The external-image copy flushes Fusion's pending draws first and restores the
  GL program, framebuffer, viewport, texture unit/bindings, buffer/attribute state,
  color mask, blend, depth, scissor, cull and stencil enables. It never draws an
  overlay View or takes over Fusion's output surface.
- The original C++ `audio_fillData` decodes/resamples to stereo S16/44.1 kHz.
  Mixer X invokes it through an independent looping-chunk/effect channel in
  **the same device as SDL_Mixer**, matching Windows SMI. Shared transport handles
  only conversion to the actual device rate. FFmpeg's existing libavfilter
  `atempo` chain preserves pitch; it no longer uses a separate Android tempo path.
- The C++ core owns audio and monotonic fallback clocks, pause, seek invalidation,
  decoder/filter/resampler drain and completion timing. The audio clock describes
  samples submitted through the callback, rather than measured speaker latency.

Android API 21+ and a current GLES 2+ Fusion renderer are required. Tempo works
through the shared libavfilter chain on API 21+. Open/seek/video decode and input materialization remain
synchronous on the Fusion run thread; slow network/probe work can block it.

## Fusion compatibility

IDs retain Windows order: 8 conditions, 20 actions and 17 expressions. Object
properties use the Win32 version-1 payload layout (including its alignment).
Hardware device name is `mediacodec`; setting copy-to-texture requests hardware
for the next open. All displayed Android video uses a Fusion texture, including
software uploads. The Windows shared-D3D-device property has no Android equivalent.
The backend uses the per-instance Surface/MediaCodec lifetime instead.

Actions cover open/open-at-position, close/play/pause, volume/loop, seek/seek flags,
thread count, accurate seek, cache/erase, device selection, stretch, tempo,
force-no-audio, codec override and reset display. Decode-thread count is capped at
16; configuration changes apply on the next open. Seek flags pass the low FFmpeg
flag bits and retain the high `NoGoto` / `NoRevert` bits. Accurate seeks discard
preroll without publishing discarded hardware frames. Opening at/previewing zero
rewinds the decoder so beginning audio is not consumed by the preview.

The two Windows frame-pointer expressions (4 and 5) consume their parameters and
return **0**: Android Fusion has no `cSurface*` ABI. They must not return a GL texture
name or truncated native address. A compatible Android frame-sharing protocol
would need corresponding changes to the consuming extension.

Inputs support readable paths, file URIs, APK assets, content URIs, HTTP(S) and
FFmpeg streaming URLs. Fusion embedded binary names and OBB/platform paths use
`CRunApp.openHFile()`. The application-owned file resolver lives in Fusion native
`CExtStorage` (FFMG), resets at Restart Application and releases on exit; instances
only release their own readers. AES/CBC and UTF-16/Win32 key derivation match
Windows. Sources needing a native seekable filename are materialized in private
cache; cache erase preserves active readers. Android scoped-storage/URI permissions
still apply: see the combined guide for paths and grants. HTTPS uses Android TLS;
no separate FFmpeg TLS library is bundled.
Finish/open-failure events execute on Fusion's calling thread, with state committed
before callbacks so actions may close or reopen the object reentrantly.
A GL context recreation closes the old codec/surface and reopens at the saved
position. `close()` is idempotent and JNI handles are registry IDs, not pointers.

## Build and publish

Like FTW, the publisher creates an extension ZIP with Java adapters, prebuilt JNI
libraries for all four Android ABIs, native sources and R8 rules. It does not replace
Fusion's Gradle templates, application manifest, runtime classes or installed
`RuntimeAndroid.zip`. `CExtLoad` registration is only for the repository's development
project; the exporter discovers `CRunFFMpeg` from the extension package normally.

The source build needs Python **3.12+**, NDK r28c, CMake 3.22+, `bash` and GNU `make`.
The FFmpeg source builder runs on Linux/WSL or macOS with the matching host NDK.
Windows users build the SDK under Linux/WSL first, then supply its output with
`OFE_FFMPEG_ROOT` or `--ffmpeg-sdk`. Source building with Windows batch compiler
wrappers is not supported. The ZIP publisher and JNI CMake build work with ordinary
Windows Python, a Windows NDK and the SDK/NDK discovery used by FTW.

```sh
# Pinned FFmpeg 9.0 LGPL shared SDK, MediaCodec/JNI enabled, four ABIs.
python3 Extensions/FFMpeg/Android/build_sdk.py \
  --ndk /path/to/android-ndk-r28c --output /path/to/ffmpeg-android-sdk

python3 Extensions/_Script/FFMpeg.py --ffmpeg-sdk /path/to/ffmpeg-android-sdk
```

`ANDROID_HOME` / `ANDROID_SDK_ROOT` select SDK discovery. `--ndk` and `--cmake`
together bypass that discovery. `OFE_FFMPEG_ROOT` provides the publisher's default
SDK path. Both the publisher and Android CMake validate the SDK version/source hash and
requested ABIs against the Windows selection in
`Extensions/_3rdLib/FFmpeg-build/FFmpegVersion.cmake`. Android has no separate
version constant: that shared file selects the release and corresponding source
SHA-256; the Windows prebuilt archive retains its own archive hash. Missing or
stale SDKs trigger a source build on supported hosts. The default Android SDK
output is `Extensions/_3rdLib/FFmpeg-sdk-Android`, separate from Windows
`FFmpeg-sdk`. Set `OFE_FFMPEG_AUTO_DEPLOY=OFF` to reject missing/stale SDKs without
building during CMake configuration. A source reproduction package includes a
snapshot of the same version file next to `build_sdk.py`. Use an SDK built by `build_sdk.py`;
its manifest, matching source archive, configuration and license files are required.
Source downloads are SHA-256 verified, with no prebuilt third-party binaries.
The SDK enables decoding/demuxing and NDK zlib (including PNG/APNG); encoders,
muxers, external codec libraries and devices are excluded. The existing C++ tempo
path enables only abuffer, abuffersink, atempo, aformat and aresample filters.

The output is `FFMpeg/ToInstall/Files/Data/Runtime/Android/FFMpeg.zip`.
Copy it into Fusion's `Data/Runtime/Android` extension directory for exporting.
`Extensions/_Script/FFMpeg.bat` is the Windows publishing entry point.
`Run_Only Unicode` invokes it after the Windows build; set
`/p:FFMpegPackageAndroid=false` to skip it. The development Gradle project can build
the JNI target with `-POFE_FFMPEG_ROOT=/path/to/ffmpeg-android-sdk`.
`--abi` selects a development subset; omit it for release packages. Packaging checks
ELF machine IDs, dynamic dependencies and every 64-bit load segment's 16 KB
alignment (NDK 32-bit libraries retain their supported 4 KB alignment),
and includes all six FFmpeg libraries (including avfilter), SDL2, Mixer X, the shared audio library
and NDK libc++ per ABI. The matching SDL Java bindings accompany the JNI bridge.
Both publishers use the same default audio build caches; custom FFmpeg audio
caches use `--audio-sources` and `--audio-build-root`. Rebuild/install both ZIPs
together after changing shared dependencies.
The source archive, LGPL notices and build configurations accompany the package.
Failed builds leave the prior ZIP intact.

## Validation

```sh
python3 Extensions/Android_Gradle/tests/ffmpeg/run.py \
  --ffmpeg-sdk /path/to/host-ffmpeg-sdk --audio-root /path/to/audio-builds/host --android-jar /path/to/android.jar
python3 Extensions/Android_Gradle/tests/ffmpeg/compile_android.py \
  --android-jar /path/to/android.jar
python3 Extensions/Android_Gradle/tests/object_helpers/run.py
python3 Extensions/Android_Gradle/tests/ffmpeg/sdk_version_test.py
```

Host tests exercise the actual JNI decoder and compare every RGBA frame against
FFmpeg CLI output, including B-frame EOF, static/alpha images, Unicode filenames, buffer bounds, accurate
seek/rewind, errors, clock and
handle lifetime. Direct C++ checks compile the original shared class and cover video/audio/filter/resampler
drain, fractional seek, memory AVIO, invalid-open cleanup and buffer bounds under
ASan/UBSan (LeakSanitizer disabled for ptrace environments). The Java compile check
uses real Android APIs and narrow Fusion boundary fixtures; it is not an APK build.
Native four-ABI compilation/ELF checks do not establish behavior on physical devices. Device checks should cover hardware and software color/orientation,
alpha, ink effects, audio sync/tempo, loop/seek and app pause/context recreation.

Verified in the workspace: FFmpeg 9.0 SDK and JNI Release builds for
arm64-v8a, armeabi-v7a, x86 and x86_64; ZIP ABI/dependency/alignment checks; Java 8
Android API/Fusion fixture compilation and ACE regression; host JNI frame/reference
and ASan/UBSan audio checks. Real Mixer X/JNI coexistence checks cover the
audio device clock, pause/seek, pitch-preserving tempo, DSP drain and independent
closure while SDL music remains active. The host uses FFmpeg 7.1.5 development libraries.
The current Windows selection/creation/base helpers were unchanged from their
reviewed revision; Android's 56 helper regressions passed without helper changes.
Shared Windows/Android version selection and stale version/source/ABI rejection
were verified. Android CMake automatic deployment also rebuilt/restored a missing
x86_64 ABI using the shared 9.0 selection and an existing source cache.
Full Fusion APK, Windows MSBuild and physical-device playback were not run here.

The independent API-23 x86_64 APK probe passed actual JNI/Mixer X playback alongside
SDL music, all six tested audio codecs, file/file-URI/assets/content-URI/HTTP and
AES inputs, native Fusion-storage fixtures, software GLES upload/reference checks,
and MediaCodec Surface/SurfaceTexture/OES color/orientation, GL-state and
playback/drain checks while using the shared C++ callback clock. Delayed-audio fixtures
also verify that audio start offsets relative to video survive decoding.
The software emulator's platform codec exercises that path; this does not establish
physical hardware acceleration or replace a full licensed Fusion Runtime export.
