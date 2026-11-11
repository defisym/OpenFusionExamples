# Experimental DX11 capture of Fusion's child client area

Select `NEOPIC_CAPTURE_BACKEND` in `Macro.h` and **Rebuild the whole project**.
The default is `0`. Existing ACE IDs and saved object properties are unchanged.

| Value | Source | Behavior |
| --- | --- | --- |
| `0` | Existing DC of `rhHEditWin` | Original baseline |
| `1` | Frame secondary Surface | Synchronous RGB normalization pass, with conservative final-composition checks |
| `2` | Windows Graphics Capture | Synchronous copy of an already available WGC frame, cropped to `rhHEditWin`'s client area |

## Required contract and current limits

A successful Capture Frame Area action must finish its GPU operation and load the
result through `LoadFromPointer(..., L"_TempCapture", ..., TakeOwnership)` **before
returning**. Show or Save may follow in the same event. No screenshot request is
deferred to another Handle. The action waits on a DX11 event query after one
asynchronous Flush; CPU bitmap conversion is not used to force completion. This
wait can cost time and is not a zero-latency/real-time deadline guarantee.

The output area is the child client area of the same `rhHEditWin` used by the DC
baseline, not the whole top-level window. The image's **logical size** is the
runtime viewport (`rh3WindowSx/rh3WindowSy`), not the physical client pixel size.
Putting a physical-pixel image into a scaled Fusion scene caused the oversized
mode 2 result reported in Windows testing. WGC must capture the root window because
of its API, then crops to that child's physical client rectangle.

**WGC cannot guarantee the exact current Fusion Display frame.** Waiting for the
GPU copy does not make an older compositor image fresh. Its producer is inherently
asynchronous, so mode 2 remains an experiment rather than a solution to a strict
current-display-frame requirement. If no frame is already available, the action
fails immediately with `E_PENDING`; it does not wait for future frame arrival.

Secondary is also not a documented final-window buffer. Mode 1 rejects a missing
or non-RTT source, a logical size different from the runtime viewport, nonzero
origin, an aspect ratio mismatch, Frame effects, or nonidentity Frame transforms whose final composition
cannot be established. These are explicit failures, not a fallback to DC and not
proof of support for every rendering configuration. A default scene may have no
secondary Surface at all. The author confirmed both experimental backends now
produce the correct image in the tested Fusion 296 / DX11 application; other
rendering configurations still require validation.

## Changes after the first Windows test

The first mode 1 implementation allocated its output from a native texture's
`GetDesc()` size and directly copied its storage. That did not establish the
mapping from the Fusion Surface's logical image to its underlying allocation.
The user reported fragmented images. The exact cause is not established yet.

The subsequent Surface BlitEx experiment produced a dark/black result in Windows
testing. Both backends now share an explicit GPU normalization pass, modeled on
the existing FFMpeg rendering approach and reusing D3DUtilities/ShaderCompiler.h:

- Retain the native source before activating/clearing a new destination RTT.
- Copy only the valid source region into a shader-readable texture, excluding
  allocation padding. WGC uses the child-client crop; Secondary uses its logical
  Surface region, with the existing conservative composition checks.
- Render to a viewport of the Fusion **logical** dimensions. Point/linear
  filtering follows the application's resize-filter option.
- Write RGB with **alpha = 1**, with blending disabled, to avoid inheriting zero
  or partial capture alpha and darkening the visible scene.
- Preserve/restore the entire pipeline with a DX11.1 context-state object. A
  missing DX11.1 interface fails explicitly instead of corrupting Fusion state.

BGRA/RGBA 8-bit RGB channel conversion is handled by SRV sampling and the target
RTV. Unsupported YUV/HDR formats are rejected rather than interpreted as RGB.
The author confirmed the corrected output for both backends in their application.
An empty secondary's RGB cannot be repaired by an opaque output pass. Native/source
crop/output dimensions are logged for comparison when the benchmark is disabled.

### Yellow capture border

The yellow border is Windows' WGC capture indicator, not a Fusion window border
or part of the generated RTT. This prototype keeps a WGC session warm so a frame
may already exist when a synchronous action is called, hence the indicator can
remain visible while the object is alive. Disabling it requires supported OS
APIs and borderless-capture access; the prototype does **not** silently disable
it or request additional consent. WGC therefore does not provide an invisible,
exact-frame-synchronous replacement for the DC baseline.

WGC initialization previously ran in CreateRunObject. A reported VS exception at
`check_hresult(CreateForWindow(...))` was caught, but session startup failed and
no image was captured. Startup is now attempted from Handle only after the child
and root are visible; an explicit action may retry. CreateForWindow failures are
returned directly and logged with the HRESULT, child/root HWNDs and visibility.
Other WinRT failures are caught and logged with their initialization stage.
A debugger can still break on first-chance exceptions inside WinRT before a catch;
continuing and reading the failure log distinguishes this from an unhandled exit.

## Build

Use VS2022, Win32, the existing SDK/dependencies and Fusion 296 / DX11.
WGC needs Windows 10 1903+ and a current Windows SDK with C++/WinRT headers
(10.0.19041+ is suitable). Its implementation uses native wchar_t in a separate
translation unit; the Fusion sources retain `/Zc:wchar_t-`.

From an x86 VS2022 Developer PowerShell in the repository root:

```powershell
./Extensions/NeoPic/tests/CompileCapture.ps1 -RunGpuSmokeTest
msbuild Extensions/NeoPic/NeoPic.vcxproj /t:Rebuild /p:Configuration="Release Unicode" /p:Platform=Win32
```

The first command syntax-checks all three backend configurations with the
benchmark disabled and enabled (0 and 100 iterations), including a standalone
capture-header translation unit. It also compiles/runs the portable benchmark
statistics tests with MSVC and a native WARP
pixel test with the production normalization pass: an 8x8 padded RGBA texture
contains only a 4x4 image with zero alpha, which must become a correctly oriented
2x2 opaque BGRA image inside a padded target. The test verifies restored viewport
and topology too. It needs no Fusion runtime or capture permission. The test's
staging readback is outside production capture code.

The public Windows PR workflow uses `-NativeOnly -RunGpuSmokeTest`: it compiles
the independent WGC translation unit under all three backend macro values, with
the benchmark disabled/enabled, and tests the production pixel pass and benchmark
statistics. It does NOT select Fusion Main/Runtime/header compilation,
since the proprietary SDK headers are intentionally absent from the repository.
The default local command above requires your Inc/ SDK headers and fails if they
are missing. Neither scope proves an MFX link or a successful WGC/Fusion capture. After that,
set the desired macro in Macro.h and Rebuild. Close Fusion, back up the installed
NeoPic MFX, then install `Extensions/NeoPic/Obj/Unicode/Release/NeoPic.mfx` into
Fusion's `Extensions/Unicode`. For exported EXEs also build Run_Only Unicode and
install that MFX into `Data/Runtime/Unicode`. No new MFX is included in this PR.

## Minimal Windows retest

1. Keep the same application, window, resolution and effects that reproduced the
   issue. Capture mode 0 once as a baseline. Test mode 1 and mode 2 separately.
2. Enable HWA on a normal NeoPic; keep its previous image hidden while capturing
   to avoid recursively including it. Add visible markers near each edge of the
   middle child area, plus different layer labels and an animated counter.
3. On one keypress, call Capture Frame Area followed immediately by the existing
   Save-current-image action with an absolute PNG path. A successful request now
   publishes `_TempCapture` during that same action. On failure the old image is
   preserved; do not mistake a file of the old image for a successful screenshot.
4. Observe VS native debug output or DebugView. Success ends with
   `ready (synchronous GPU completion)`. Failure includes an HRESULT. For mode 1
   send the `secondary logical=... viewport=... rhHEditWin-client=...` line. For
   mode 2 send any `WGC CreateForWindow failed ...`/other stage line and the
   `native-source=... format=... client=... native-target=... format=...` line.
5. Compare saved PNG content to the middle child area: no title/menu/outer frame,
   no missing edges/layers, no fragmented pixels. Record image dimensions too.
   When showing the image inside the same scene, global Frame effects may apply
   again; use the PNG to distinguish a capture problem from a second effect pass.
6. Let the original scene continue animating. The owned snapshot must remain
   unchanged and support the usual NeoPic fade. Note that an event executes
   before its subsequent Display: it cannot capture changes that have not yet
   been rendered. WGC additionally has compositor/capture latency.

Return the logs, baseline/experimental PNGs, Windows version, GPU/driver and
actual window/DPI/effect settings. For a debugger exception, the HRESULT is the
error code (e.g. 0x80070057); the KernelBase instruction address is not the HRESULT.

## Benchmark inside the capture action

In `Macro.h`, select one backend and change the benchmark definitions, then
**Rebuild** and install the matching MFX as above:

```cpp
#define NEOPIC_CAPTURE_BACKEND 0 // Repeat separately with 1 and 2.
#define NEOPIC_CAPTURE_BENCHMARK 1 // Default is 0: normal single capture.
#define NEOPIC_CAPTURE_BENCHMARK_ITERATIONS 100
#define NEOPIC_CAPTURE_BENCHMARK_WARMUP 10
```

The existing guarded defaults allow project-wide compiler definitions instead.
`NEOPIC_CAPTURE_BENCHMARK=0` (the default) compiles out the benchmark loop,
timer, statistics and benchmark-only capture state. The action captures once,
even if `ITERATIONS` remains 100 and `WARMUP` remains 10. Set the master switch to
1 to enable measurement; `ITERATIONS=0` also keeps the ordinary action.
Rebuild after changing the switch. No ACE IDs or MFA changes are needed.

1. Use the same application, logical viewport, client size/DPI, resize filter,
   effects, GPU, NeoPic HWA/library settings and Release build for all three runs.
   Keep the NeoPic's displayed screenshot hidden to avoid recursion. Start with
   100 iterations, since the action deliberately blocks events and Display.
2. Let the scene render first; for WGC, allow roughly a second for its session to
   warm up. Press a key to call **Capture Frame Area once**. The loop runs inside
   that action; no Fusion fast loop or Always event is needed.
3. Copy the `NeoPic capture benchmark ...` summary from VS debug output or
   DebugView. Repeat for backends 0, 1 and 2. Compare only runs with matching
   `viewport`, `output`, HWA/library and filter settings, and `status=ok`.
4. The last successful image remains available as `_TempCapture`. Show/save it
   after the action if desired; writing a file is outside the benchmark.

Each measured iteration includes Capture, allocation/normalization, synchronous
GPU completion, and the ordinary `LoadFromPointer(..., L"_TempCapture", ...)`
handoff (including replacement of the previous snapshot). For fair output sizes,
the **benchmark-only** DC path scales to the same logical viewport as modes 1/2;
it also flushes GDI and, with NeoPic HWA under DX11, waits for the handoff's GPU
upload. The normal non-benchmark DC action retains its original output sizing.
This compares the complete action path, not just a GPU copy command.

Normal per-capture debug messages are suppressed during the loop. The summary
reports warmup successes/failures, first-call wall time, measured successes and
failures, measured batch wall time, successful-sample mean/min/median/P95/max,
and completed copies per second. Warmup and summary output are excluded from the
measured batch; `first_ms` is the first call of this batch, not necessarily a cold
process start. P95 uses nearest rank. Failed attempts have separate timing and
never enter successful percentiles. If every warmup attempt fails, measurement
is skipped (`status=no_ready_source`); this is a failed run, not a speed result.

**There is no new Fusion Display between iterations.** In benchmark mode WGC
retains the latest valid frame until the loop finishes, refreshing it if another
frame is already queued. It can therefore repeatedly copy the same image rather
than report mostly fast `E_PENDING` failures. A successful benchmark measures
snapshot processing throughput, not distinct captured frames per second, source
freshness or display FPS. Timing uses QPC wall time, including CPU work and GPU
waiting, rather than GPU timestamps. CaptureCompletion's existing 1 ms polling
interval can affect these synchronous wall times. No speedup is assumed before
comparing actual logs from the same machine.

## Regression tests and validation boundaries

```sh
c++ -std=c++20 -Wall -Wextra -Werror Extensions/NeoPic/tests/CaptureCropTest.cpp -o capture-crop-test
./capture-crop-test
c++ -std=c++20 -Wall -Wextra -Werror -IExtensions/NeoPic/tests/completion-mocks Extensions/NeoPic/tests/CaptureCompletionTest.cpp -o capture-completion-test
./capture-completion-test
c++ -std=c++20 -Wall -Wextra -Werror Extensions/NeoPic/tests/CaptureBenchmarkTest.cpp -o capture-benchmark-test
./capture-benchmark-test
```

Crop tests cover client mapping, scaling and negative monitor coordinates.
Completion tests exercise the production helper against small mock interfaces:
query allocation, immediate-context checks, ordered End/Flush, incomplete/complete
results, synchronous waiting, teardown and errors. Mocks are never on the plugin
include path and do not emulate a real GPU. Benchmark tests use an injected clock
and capture callback to check warmup exclusion, failed-sample separation,
percentiles, unavailable sources and input validation.

The Linux development environment cannot compile/link MSVC or run Fusion/WGC.
Public Windows checks cover the independent components and WARP as described
above, not the proprietary Fusion adapter or an MFX link. The author confirmed
correct images for both experimental backends in their Windows application.
Portable tests also run with GCC, AddressSanitizer and UBSan (leak detection
disabled under ptrace). The benchmark itself still needs application performance
measurements; no speed improvement is claimed.
