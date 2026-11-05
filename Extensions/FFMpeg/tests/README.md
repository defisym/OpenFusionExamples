# Shared C++ playback regressions

These tests exercise `Extensions/_3rdLib/FFMpeg.h` directly, without JNI,
Android or a Mixer X audio device. They belong to the shared playback core.

```sh
python3 Extensions/FFMpeg/tests/run.py \
  --ffmpeg-include /path/to/ffmpeg/include --ffmpeg-lib /path/to/ffmpeg/lib \
  --sdl-include /path/to/SDL2/include/SDL2 --sdl-lib /path/to/SDL2/lib
```

The runner requires a C++20 compiler with ASan/UBSan and an `ffmpeg` CLI with
MPEG-4/AAC encoding. It generates video with delayed frames, delayed audio and
Unicode filenames. LeakSanitizer is disabled for managed ptrace environments.
`PCMFragmentTest` compares a large PCM request with four-byte fragments using
SDL's real additive mixer and rejects an all-silence result. `SharedCoreTest`
covers frame count, audio tail, fractional seek, rewind/preview, callback reentry,
state-only pause, delayed audio and invalid-open cleanup. Memory-source checks
cover read/seek/size/EOF, an independent preview context, complete audio/video
playback to EOF and preservation of caller-owned data after player destruction.
`DecoderProtocolTest` uses linker wrappers around the actual FFmpeg calls to
verify Send First ordering, the queue-wait early return without touching the codec,
one-tick deferral and recovery after source EAGAIN,
and propagation of IO EIO and codec EPERM without confusing them with EOF or
queue waiting. This test requires a linker supporting GNU `--wrap`.

## Review boundaries

The core commits precede Android integration and are organized by the problem
they solve. Shared behavior fixes affect Windows too; they are not all required
merely to compile on Android.

| Intent | Change and compatibility boundary |
| --- | --- |
| Dependency cleanup | Include the existing arithmetic/string/conversion helpers directly and remove a stale probe comment. Do not introduce FFmpeg-specific copies or a header for an unused audio constant. |
| Portable compilation | Standard-width types, standard exception constructor, qualified lock types and guarded Windows headers. Keep Windows call sites and error codes. |
| AVIO ownership | A non-owning MemView tracks the caller's media data and read cursor. Allocate/free the separate AVIO working buffer through FFmpeg, including partial construction. Use avformat's custom-IO probing and bounded memory seeking/size queries. Narrow paths delegate to the existing wide-path constructor. |
| Pixel conversion | Cache conversion by actual frame properties and allocate/describe the actual output stride. BGRA remains the default; callers may select RGBA. |
| Seek timeline | Preserve fractional seconds and normalize frame timestamps against the source timeline. |
| PCM copy bounds | Pass only the available decoded bytes to the mixer. The regression fails before this one-line fix and passes after it. |
| Decoder protocol | Preserve the original Send First branches, queue-wait early return, pending-packet retry and packet argument contract. Source EAGAIN defers the tick; queue waiting and native EOF are distinct from real errors. Frame-derived audio timestamps remain in the audio-pipeline commit. The separate forwardFrame seek loop retains its previous packet-release behavior; its send-EAGAIN retry policy is not changed or claimed fixed here. |
| Audio pipeline | Drain filters and resampling, size PCM storage dynamically, reset DSP state on seek and return actual bytes produced. |
| Playback clocks | Use monotonic time and submitted-audio progress; preserve delayed starts, pause and tempo continuity. Preserve the state-only pause option. |
| Shared seek/preview policy | Move the Windows wrapper policy into the core, support unclocked previews/still images and report completion. Invoke frame callbacks outside the audio lock so controls can reenter. |
| Hardware lifecycle | Extend the existing adaptor interface for decoder selection/context ownership. Preserve explicit decoder overrides, allow delayed pixel-format discovery and retry software when hardware initialization fails. |

Only the final integration commit adds MediaCodec/GLES adaptors, JNI/Java,
Android SDK deployment, packaging, platform tests and platform-specific crypto
compatibility. The D3D implementation remains behind Windows guards.

Each core stage from the portable-compilation commit onward was independently
compiled and exercised on a Linux host with FFmpeg 7.1.5 and SDL2. These checks do not replace Windows MSBuild or
Android device/export testing. Platform integration results are recorded in
[`Android/README.md`](../Android/README.md).
