# NeoPic frame capture

Select **Display Settings → Frame Capture → Capture Backend** in the object's
properties. One MFX contains all three backends; there are no backend-selection
or benchmark macros. A normal Capture Frame Area action performs one capture and
publishes `_TempCapture` synchronously.

| Property value | Backend | Source |
| --- | --- | --- |
| 0 (default) | Window DC | Existing `rhHEditWin` DC |
| 1 | Secondary Surface (DX11) | The Frame's valid secondary Surface region |
| 2 | Windows Graphics Capture | Root window cropped to the same child client area |

The selection belongs to each object, including library objects, and is saved in
the MFA/exported runtime data. Unknown stored values use DC. There is no silent
fallback after a selected GPU backend fails.

## Structure and ownership

`FrameCapture` owns the common capture-and-publish transaction. `DcFrameCapture`,
`SecondaryFrameCapture` and `WgcFrameCapture` supply their respective images;
`Dx11FrameCapture` shares native normalization and GPU completion handling.
The action calls `CaptureAndLoad` once. `Update` only prepares an asynchronous
source when needed; it never defers an action's capture request.

The DC backend reuses its internal bitmap and clones it before publication. GPU
backends transfer an owned RTT directly. Both reach
`LoadFromPointer(..., L"_TempCapture", ..., TakeOwnership)` without an additional
clone. Save or Show may follow in the same event. On a capture failure the old
image remains unchanged. Native queries submit with one Flush and wait for GPU
completion before releasing borrowed frames; this is not a latency deadline.

Native WGC code remains in its own `/Zc:wchar_t` translation unit. Fusion sources
keep their existing `/Zc:wchar_t-` ABI. The production source/header dependency
graph does not include the optional benchmark subclass or statistics runner.

## Output sizing and source limits

DC retains its normal client-sized output. GPU output uses the Fusion logical
viewport (`rh3WindowSx/rh3WindowSy`), preventing a physical-pixel screenshot from
being displayed oversized in a scaled scene. Client size, valid source image size,
native allocation size and destination size are separate quantities.

The full valid source rectangle is stretched to the destination size, following
the DC path's independent source/destination rectangles. Source dimensions need
not equal the viewport and aspect ratios need not match. For example, a 1920x1080
logical capture remains that size as a window is dragged through 1281x720,
1365x768 or other dimensions. Native allocation padding is excluded. The shared
GPU pass uses point/linear sampling according to the resize-filter option, writes
opaque alpha, supports the listed 8-bit RGBA/BGRA layouts and rejects unsupported
YUV/HDR formats. DX11.1 context-state swapping preserves Fusion's pipeline.

Secondary is a conditional runtime render target, not a documented final-window
buffer. Missing/non-RTT sources, nonzero origins, unsupported Frame effects and
nonidentity Frame transforms remain explicit failures. An accepted Surface and a
completed GPU operation do not prove that its pixels contain every final effect
or border. Diagnostic output includes app/frame always-secondary flags, source
and old-transition Surface identities, layer-effect-data presence, dimensions
and the failure stage. These flags are never changed to force a capture. The old
secondary belongs to frame transitions and is not substituted as a previous
Display buffer. A black image is not automatically classified as failure.

WGC's producer is asynchronous: a GPU-complete snapshot can still be older than
the current compositor frame. With no ready image, capture returns `E_PENDING`
instead of waiting for a future Display. A warm WGC session shows Windows' yellow
capture indicator; this prototype requests no borderless permission. Startup
waits until the child/root are visible and reports the initialization stage and
HRESULT on failure. Handled first-chance WinRT exceptions can still interrupt a
VS debugger before its catch runs.

The author confirmed the rendering and resize changes in their application.
This does not establish final-composition equivalence for every Fusion project.

## Edit-data compatibility

`Main.h` includes the versioned layout in `EditData.h`. V1 is frozen. V2 adds a
four-byte `CaptureBackend` and **64 reserved bytes**. Future fixed-size fields
must consume that reserve, reducing its length by their actual size/alignment;
the Win32 payload-budget assertion detects unintended growth.

`UpdateEditStructure` uses the same `UpdateEditData` helper as Fini. It copies all
V1 settings, defaults the new selector to DC, clears the reserve and updates the
allocation limit. The shared helper preserves the other SDK header fields and
sets the new size/version. New objects also initialize the selector and reserve
explicitly: Fusion's raw data allocation does not run C++ constructors.

## Optional derived benchmark

Production captures once and contains no timer, loop count or benchmark state.
To run the example, include the optional header in `Runtime.cpp`, after Common:

```cpp
#include "FrameCaptureBenchmark.h"
```

Replace the normal factory call in CreateRunObject with:

```cpp
rdPtr->pFrameCapture = CreateFrameCaptureBenchmark(rdPtr,
    NormalizeCaptureBackend(static_cast<std::uint32_t>(edPtr->captureBackend)),
    {100, 10}).release(); // Measured iterations, warmup calls.
```

The factory wraps the selected backend in `FrameCaptureBenchmark<Base>`. Counts
are constructor arguments, not macros. Restore `CreateFrameCapture` and remove
the optional include to return to the production implementation. No action,
property, serialized benchmark flag or saved event changes are required.

Let the scene render first (allow roughly a second for WGC), then call Capture
Frame Area once. The subclass performs warmup and measurement inside that action,
suppresses per-capture diagnostics and prints QPC wall-time statistics to VS debug
output/DebugView. Keep the same Release build, viewport, client size/DPI, filter,
effects, GPU and HWA/library settings when comparing backends. Keep any displayed
snapshot hidden to avoid recursion.

Samples include capture, allocation/normalization, synchronization and the
`_TempCapture` ownership handoff. DC is scaled to the same logical output size,
flushes GDI before cloning, and waits for its HWA upload under DX11. Saving a file
is outside measurement. Warmup and summary output are excluded. Success and
failure timings are separate; all-failed warmup skips measurement. P95 is nearest
rank. The first-call time is the first call of the batch, not necessarily a cold
process start. The last successful snapshot remains available after the loop.

There is no new Fusion Display between iterations. The subclass retains a ready
WGC input until the batch finishes, refreshing queued frames when available.
Results measure snapshot processing throughput, not unique-frame capture FPS.
CPU work and the completion helper's 1 ms polling wait are part of wall time.
Exception-safe session cleanup restores ordinary input release behavior.

## Build and verification

Use VS2022, Win32, the existing private Fusion SDK/dependencies and Fusion 296 /
DX11. WGC requires Windows 10 1903+ and current C++/WinRT headers (Windows SDK
10.0.19041+ is suitable). From an x86 VS Developer PowerShell:

```powershell
./Extensions/NeoPic/tests/CompileCapture.ps1 -RunGpuSmokeTest
msbuild Extensions/NeoPic/NeoPic.vcxproj /t:Rebuild /p:Configuration="Release Unicode" /p:Platform=Win32
```

The default script checks Main, Runtime, Edittime, General, the common capture
implementation and optional derived header against the real SDK, and compiles
the property resources. It fails if Inc/ccxhdr.h is absent. MFX linking is a
separate build step. Close Fusion before installing the rebuilt editor MFX;
rebuild/install Run_Only Unicode too when testing exported EXEs.

Public Windows CI uses `-NativeOnly -RunGpuSmokeTest`, with explicit scope:

- The actual independent WGC implementation compiles with native wchar_t.
- Production normalization runs with WARP: padded source cropping, logical
  resizing, RGBA/BGRA order, opaque alpha, completion and state restoration.
  Repeated 6x3, 2x6 and 2x2 outputs exercise independent aspect ratios and resizing.
- With the D3D debug layer available, a real WARP lifetime test checks named
  capture objects after one capture, 32 captures and repeated recreation. It
  verifies that live objects are observable during use, then that no named objects
  retain external references after destruction, without using ClearState. Missing
  debug-layer support is printed as SKIP, not represented as lifetime validation.
- Statistics tests check warmup, percentiles, failed samples and unavailable input.
- Production V1/V2 payload definitions and upgrade defaults are tested with a mock
  SDK header, including 64-byte reserve and backend-value validation.
- Production capture classes and optional subclass compile/run against small
  SDK test doubles: factory selection, virtual dispatch, owned publication,
  old-image preservation, failed warmup and exception/session cleanup.

Test doubles never enter the plugin include path. They do not reproduce Fusion
layout/rendering, prove a real SDK build, or establish actual WGC/Fusion image
correctness. The Linux environment runs the portable crop, completion, statistics
and edit-data tests; native/compiler/GPU evidence comes from the Windows workflow.

After installation, verify an old V1 MFA retains its settings and selects DC.
Select each new backend, save/reopen, and export to check persistence. On one
keypress, Capture then Save `_TempCapture` in the same event. Compare edge markers,
layers and image size against the intended child area. Test fixed resolution
presets, drag smaller/larger, then return to a preset. Capture failures must
preserve the old snapshot. Runtime resizing and the optional benchmark subclass
need actual application retesting after this refactor.

## Investigating D3D live-object warnings

The author reported exit-time live-object warnings appearing after GPU capture
was introduced. Treat this as a capture-resource regression to investigate, not
as evidence that the warnings are harmless. The old anonymous shutdown report
cannot identify owners or distinguish intended snapshot retention from a leak.

When the device debug layer is enabled, objects created by the normalization pass
are named `NeoPic.Capture.*`: shaders, context state, samplers, pipeline states,
sampling/resolve textures and views, and the owned output texture/view. The shared
Fusion device and input secondary are not renamed. Production code never calls
ClearState on Fusion's context to suppress live-object reports.

Simple process-termination reporting may still omit names. Obtain a detailed
`ID3D11Debug::ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)` report while the process is
alive, at a controlled checkpoint after capture objects and published snapshots
are released. A second report with `D3D11_RLDO_IGNORE_INTERNAL` helps separate
objects with external references from driver/runtime internal retention. Compare
no-capture, single-capture and repeated-capture runs at the same checkpoint.
A snapshot intentionally retained in the image library is expected to remain live
until the library releases it.

The standalone lifetime test isolates the native pass and its COM ownership.
It cannot prove that Fusion calls every extension destructor or releases its
image cache/WGC session in the same order. A passing test therefore does not by
itself resolve the application's original exit warnings.
