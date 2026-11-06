# Experimental DX11 frame capture

The existing **Capture Frame Area** action can test three backends. Select
`NEOPIC_CAPTURE_BACKEND` in `Macro.h` and **Rebuild the whole project** after each
change. The default remains `0`; no ACE IDs or saved object properties change.

| Value | Source | Action behavior |
| --- | --- | --- |
| `0` | Existing window DC | Existing synchronous capture |
| `1` | `rhFrame->m_pSecondarySurface` | Request in events; GPU copy in a subsequent Handle |
| `2` | Windows Graphics Capture | Session starts at object creation; copy the latest available window frame in a subsequent Handle |

Modes 1 and 2 require the DX11 renderer. They never fall back to DC. A failed
request leaves the current NeoPic image unchanged. Missing frames/secondary
surfaces are retried for up to 120 Handle calls; duplicate pending requests are
coalesced. Do not switch Fusion Frames or destroy the object before completion.

The experimental image is transferred as an owned Fusion RTT, avoiding the
`CreateCloneSurface` path that can read HWA alpha back to CPU. Display/fade uses
the normal NeoPic image and ink effects. Existing save actions perform CPU
readback only when explicitly requested. These test backends are asynchronous:
a save action immediately following Capture still sees the previous image.

## Build

Use VS2022, Win32, and the existing Fusion SDK/dependencies. WGC additionally
needs a current Windows SDK with C++/WinRT headers (10.0.19041 or newer is a
suitable baseline) and Windows 10 1903 or newer on the test machine.
`WgcCapture.cpp` uses native `wchar_t` in isolation, while the other project
sources retain `/Zc:wchar_t-`. C++/WinRT does not cross that translation-unit ABI.
WGC links `windowsapp.lib`, `runtimeobject.lib`, `d3d11.lib`, and `dwmapi.lib`; those dependencies are excluded in
modes 0 and 1.

From an x86 VS2022 Developer PowerShell, first check all three configurations:

```powershell
./Extensions/NeoPic/tests/CompileCapture.ps1
```

This runs MSVC syntax checks against the real `Main.cpp`, `Runtime.cpp`, and
`WgcCapture.cpp`; it does not link an MFX or prove runtime capture correctness.
Then set the selected macro in `Macro.h` and build:

```powershell
msbuild Extensions/NeoPic/NeoPic.vcxproj /t:Rebuild /p:Configuration="Release Unicode" /p:Platform=Win32
```

Close Fusion, back up the installed NeoPic MFX, and install the resulting
`Extensions/NeoPic/Obj/Unicode/Release/NeoPic.mfx` in `Extensions/Unicode`.
For an exported EXE also build `Run_Only Unicode` and install that MFX in
`Data/Runtime/Unicode`. The source-only PR does not include new MFX binaries.

## Minimal test scene (Fusion 296 / DX11)

1. Create a fixed-size Frame with three visibly labelled layers (`L1`, `L2`,
   `L3`), different colored Active objects, and a counter updated every loop.
2. Add a normal NeoPic with **HWA enabled**, initially blank and hidden. Keep it
   in the Frame throughout the test. Press **C once** to invoke Capture Frame
   Area. Avoid an Always event that repeatedly captures its own displayed image.
3. Wait a few loops (allow WGC roughly one second after startup), then press
   **V** to show NeoPic. The result should include the three layer markers. Use
   NeoPic's normal blend coefficient to test fade. Keep the original scene
   visible until capture has completed.
4. In a separate **S** event, use the existing save-current-image action with
   a writable absolute PNG path. This is the only step intended to read pixels
   back and write the snapshot to disk. Save once with each backend selected.
5. Observe Visual Studio's native Debug output or DebugView for lines beginning
   `NeoPic capture backend=`. `start`, `ready`, and `failed` include an HRESULT.
   A missing secondary surface should eventually fail, never silently use DC.

Repeat static and moving-counter tests under these configurations:

- No layer/Frame effects and no transitions (a missing secondary is possible).
- An obvious layer effect plus an obvious Frame effect, with their names noted.
- A configured Frame transition; first capture while staying in the same Frame.
- WGC: partially cover Fusion with another window, resize, minimize/restore, and
  move between monitors at different DPI scales. Check the client-area crop,
  borders, menu exclusion and image orientation. A minimized window may time out.

Record Windows/Fusion build, GPU/driver, backend macro, VSync/backbuffer settings,
effects, screenshots, and debug output. Use the visible counter to compare frame
age; WGC is not synchronized with a particular Fusion Display call. For timing,
compare one-shot captures at the same resolution without saving files; separate
first-use allocation from repeated captures. This PR makes no measured speed claim.

## Expected limits

- Secondary Surface availability, contents and inclusion of Frame effects are
  deliberately under test. No runtime booleans or backbuffer counts are modified.
  `m_pOldSecondarySurface` is not treated as the previous display frame.
- WGC captures the root window, then maps the Fusion child client rectangle from
  physical DWM bounds to capture coordinates. Resize/DPI mapping needs real tests.
  Its default OS border/cursor behavior is retained; it does not capture native
  child controls into a Fusion-only scene texture separately.
- Both experimental sources are copied into independent RTTs using Fusion's DX11
  device, with format/dimension/device checks. Unsupported format conversions
  fail explicitly; no staging texture, Map, LockBuffer, or pixel loop is used.
- WGC capture frames remain retained while their textures are copied. All Fusion
  calls/copies happen on the runtime thread; there is no capture callback touching
  Fusion. No GPU-idle wait is introduced.
- Device loss/session failure requires recreating the test object/application.
  The request timeout is a loop count, not a wall-clock deadline. Capture success
  is currently reported in debug output, not through a new Fusion condition.

The client-crop arithmetic also has a portable test:

```sh
c++ -std=c++20 -Wall -Wextra -Werror Extensions/NeoPic/tests/CaptureCropTest.cpp -o capture-crop-test
./capture-crop-test
```

## Validation status

The development environment is Linux and cannot compile MSVC/C++/WinRT or run
Fusion/DWM. The crop tests passed locally with AddressSanitizer and UBSan (leak detection
disabled because the execution environment uses ptrace). Project XML and source
checks passed. The Windows compile
script and the manual matrix above are provided for actual validation. Keep the
PR experimental until those results are available.
