# Experimental DX11 capture of Fusion's child client area

Select `NEOPIC_CAPTURE_BACKEND` in `Macro.h` and **Rebuild the whole project**.
The default is `0`. Existing ACE IDs and saved object properties are unchanged.

| Value | Source | Behavior |
| --- | --- | --- |
| `0` | Existing DC of `rhHEditWin` | Original baseline |
| `1` | Frame secondary Surface | Synchronous Fusion GPU Surface blit, with conservative final-composition checks |
| `2` | Windows Graphics Capture | Synchronous copy of an already available WGC frame, cropped to `rhHEditWin`'s client area |

## Required contract and current limits

A successful Capture Frame Area action must finish its GPU operation and load the
result through `LoadFromPointer(..., L"_TempCapture", ..., TakeOwnership)` **before
returning**. Show or Save may follow in the same event. No screenshot request is
deferred to another Handle. The action waits on a DX11 event query after one
asynchronous Flush; CPU bitmap conversion is not used to force completion. This
wait can cost time and is not a zero-latency/real-time deadline guarantee.

The output area is the child client area of the same `rhHEditWin` used by the DC
baseline, not the whole top-level window. WGC must capture the root window because
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
secondary Surface at all. Results still need Windows validation.

## Changes after the first Windows test

The first mode 1 implementation allocated its output from a native texture's
`GetDesc()` size and directly copied its storage. That did not establish the
mapping from the Fusion Surface's logical image to its underlying allocation.
The user reported fragmented images. The exact cause is not established yet.

Mode 1 now uses `source->GetWidth()/GetHeight()` and Fusion's own GPU `BlitEx`
pass to interpret Surface type, format, internal UV/orientation/padding and scale
the accepted viewport to the child client size. It no longer dereferences the
secondary's native texture or assumes its storage describes the image layout.
Unsupported composition is rejected rather than misrepresented as a screenshot.

Mode 2 still copies only matching native formats. It logs source/target native
sizes and formats and validates its source crop. The destination may have a
larger padded allocation than its logical image; only the logical client region
is written. It does not implement general format conversion or YUV decoding.

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
./Extensions/NeoPic/tests/CompileCapture.ps1
msbuild Extensions/NeoPic/NeoPic.vcxproj /t:Rebuild /p:Configuration="Release Unicode" /p:Platform=Win32
```

The first command syntax-checks all three backend configurations, including a
standalone capture-header translation unit to verify the Fusion preamble is
available. Secondary-only implementation is excluded from the WGC configuration,
and the opaque clear value is explicitly typed as RGBAREF to avoid C2398. After that,
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

## Regression tests and validation boundaries

```sh
c++ -std=c++20 -Wall -Wextra -Werror Extensions/NeoPic/tests/CaptureCropTest.cpp -o capture-crop-test
./capture-crop-test
c++ -std=c++20 -Wall -Wextra -Werror -IExtensions/NeoPic/tests/completion-mocks Extensions/NeoPic/tests/CaptureCompletionTest.cpp -o capture-completion-test
./capture-completion-test
```

Crop tests cover client mapping, scaling and negative monitor coordinates.
Completion tests exercise the production helper against small mock interfaces:
query allocation, immediate-context checks, ordered End/Flush, incomplete/complete
results, synchronous waiting, teardown and errors. Mocks are never on the plugin
include path and do not emulate a real GPU.

The Linux development environment cannot compile/link MSVC or run Fusion/WGC.
Portable tests pass with GCC, AddressSanitizer and UBSan (leak detection disabled
under ptrace). Windows syntax checks, real pixels, initialization and performance
remain unverified. The reported fragmentation and startup failure are not claimed
fixed until a real Windows retest succeeds. No speed improvement is claimed.
