# ReadMe

## Description

Audio Extension based on [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) & [SDL_Mixer_X](https://github.com/WohlSoft/SDL-Mixer-X), with [encryption](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files) support

Audio Effect powered by [SoundTouch](http://www.surina.net/soundtouch/index.html)

for more information, please check the [Help File](ToInstall/Files/Help/SDL_Mixer/SDL_Mixer.md)

## Reference

### SDL Mixer X

- [Music document](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/5_5_functions_music.texi)
- [Play music example](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/examples/playmus.c)

### Sound Touch

- [FAQ](https://www.surina.net/soundtouch/faq.html)
- [Example](https://codeberg.org/soundtouch/soundtouch/src/branch/master/source/SoundTouchDLL/DllTest/DllTest.cpp)
- [Theory behind](https://www.surina.net/article/time-and-pitch-scaling.html)

## How to build

SDL2 and SDL Mixer X are pinned submodules in `Extensions/_3rdLib/SDL2` and `Extensions/_3rdLib/SDL-Mixer-X`. SDL2 uses stable release 2.32.10; MixerX uses upstream 2.8.0 (`72139aa`). Headers are read from those sources when building the SDK, rather than maintained as copies in this repository.

### Automatic preparation in Visual Studio

With Git and CMake 3.21+ installed, clone OFE and open the plugin solution in Visual Studio 2022 or 2026, then build or rebuild it. Both solutions include the `SDLDependencies` utility project, and both plugins reference it as a build dependency. That project invokes CMake to initialize the two pinned submodules and build the combined SDK before the plugin is compiled; `SDL.props` only selects and checks the SDK paths. A recursive clone is optional. If the SDK matches the checked-out revisions, it is reused; parallel plugin builds wait for the same SDK preparation rather than building it twice. AudioCodecs is downloaded on the first build.

The generator follows the running IDE (VS 2022 or VS 2026); VS 2026 requires CMake 4.2+. The projects still request the v143 toolset, so install the VS 2022 C++ build tools component when using VS 2026. Explicit `OfeSDLGenerator` / `OfeSoundTouchGenerator` overrides are supported. Build directories are separated by generator, VS instance and toolset. Dependency command output is also saved in each generated SDK root as `.ofe-build.log` for troubleshooting.

The CMake installation bundled with Visual Studio's C++ CMake tools is detected automatically; otherwise `cmake` must be on PATH. Git must be on PATH, or its executable can be selected with `OfeSDLGit`. `OfeSDLCMake` can similarly select another CMake executable.

Selecting an external SDK with `OfeSDLRoot`, for example `D:\Dev\SDL`, disables automatic preparation by default and leaves that SDK under your control. Set `OfeAutoBuildSDL=false` to manage dependencies manually. Automatic preparation follows the submodule revisions recorded in the Git index, so stage updated submodule revisions before using the automatic build to test them.

### Preparing the combined SDK manually

From the repository root, initialize just these dependencies:

```powershell
git submodule update --init --depth 1 Extensions/_3rdLib/SDL2 Extensions/_3rdLib/SDL-Mixer-X
```

With Visual Studio 2022 C++ tools, CMake 3.21+ and Git installed, build the Win32 SDK:

```powershell
cmake -S Extensions/_3rdLib/SDL-build -B Extensions/_3rdLib/SDL-build/out/x86 -G "Visual Studio 17 2022" -A Win32
cmake --build Extensions/_3rdLib/SDL-build/out/x86 --config Release
```

The build first installs SDL2, then builds static codecs and shared MixerX against that same SDL2. AudioCodecs is fetched at pinned revision `bb11ca3`; it does not build its bundled SDL2. LGPL codecs are enabled, GPL codecs are disabled, MP3/FLAC use the built-in decoders, and GME uses its LGPL GENS backend.

The generated SDK preserves the combined layout:

```text
Extensions/_3rdLib/SDL-sdk/
  include/             SDL2 headers and MixerX's SDL_mixer.h
  lib/x86/             SDL2.dll, SDL2.lib, SDL2_mixer_ext.dll, SDL2_mixer_ext.lib
  lib/x64/             Same files from an optional x64 build
```

Upstream MixerX installs its header as `SDL_mixer_ext.h`; the SDK also stages the pinned MixerX header as `SDL_mixer.h`, which OFE includes. No official SDL_mixer headers or binaries are merged into this SDK.

Both plugin projects use `SDL.props` to select this SDK. You can also build it manually using the commands above, then rebuild the edit-time and runtime plugin configurations. Build outputs are ignored by Git. For x64 consumers, run the same two CMake commands with `out/x64` and `-A x64`; the current Fusion plugin projects remain Win32. Both architecture builds use Release dependencies and should be run sequentially because they share the SDK headers.

To keep using an existing combined SDK such as `D:\Dev\SDL`, set the MSBuild property `OfeSDLRoot` to that directory in your user property sheet or on the build command line. To generate the SDK somewhere else, configure with `-DOFE_SDL_SDK_ROOT="D:/Dev/SDL"` and use that same `OfeSDLRoot`. Use a separate output directory when switching between compiler toolchains.

The DLLs and import libraries in [MixerXLib](MixerXLib) are distributed for plugin users, separately from the generated development SDK. The checked-in MixerX binaries were built from upstream `72139aa`, AudioCodecs `bb11ca3` and the SDL2 2.28.5 SDK with MinGW-w64 GCC 14.2.0; they also work with SDL2 2.32.10. Rebuild both plugins before deploying the updated DLLs to the Fusion root and `Data/Runtime/Unicode`; older plugin binaries may still import `Mix_AudioOpened`. Use the same SDL2.dll and SDL2_mixer_ext.dll for both plugins.

To update dependencies, check out the desired upstream revisions inside the submodules, rebuild and test the SDK, and commit the new submodule revisions in OFE. Submodule updates do not automatically replace the binaries distributed to plugin users.

### SoundTouch dependency

SoundTouch has an official Git repository at <https://codeberg.org/soundtouch/soundtouch>. OFE pins its 2.3.3 release (`e83424d`) as `Extensions/_3rdLib/SoundTouch`.

The SDL_Mixer solution also includes `SoundTouchDependencies`, a separate CMake-driven utility project referenced by the plugin. It initializes the SoundTouch submodule and generates `Extensions/_3rdLib/SoundTouch-sdk/include/SoundTouchDLL.h` and `lib/x86` or `lib/x64` with `SoundTouchDLL.dll` and its import library. FFMpeg does not depend on this project. The core is linked statically inside the existing C DLL wrapper, with float internals and the `soundtouch_*_i16` conversion functions used by the plugin.

For a manual Win32 SDK build from the repository root:

```powershell
git submodule update --init --depth 1 Extensions/_3rdLib/SoundTouch
cmake -S Extensions/_3rdLib/SoundTouch-build -B Extensions/_3rdLib/SoundTouch-build/out/manual-x86 -G "Visual Studio 17 2022" -A Win32 -DCMAKE_INSTALL_PREFIX="$PWD/Extensions/_3rdLib/SoundTouch-sdk"
cmake --build Extensions/_3rdLib/SoundTouch-build/out/manual-x86 --config Release --target soundtouch_sdk
cmake --install Extensions/_3rdLib/SoundTouch-build/out/manual-x86 --config Release
```

Set `OfeSoundTouchRoot` to keep using an existing SDK, such as `D:\Dev\SoundTouch`. Its original flat layout with `SoundTouchDLL.h` and `SoundTouchDLL.lib` directly in that folder remains supported, as does the generated `include`/`lib/x86` layout. An external SDK disables automatic SoundTouch builds by default; `OfeAutoBuildSoundTouch=false` can also disable them explicitly. `OfeSoundTouchCMake` and `OfeSoundTouchGit` select tool executables if needed.

Deploy the generated Win32 `SoundTouchDLL.dll` alongside the SDL DLLs in the Fusion root and `Data/Runtime/Unicode`. The plugin still declares the same DLL dependency and uses the same C API; no audio-processing code or extra SoundTouch core DLL is introduced. SDK builds do not automatically publish runtime DLLs into a Fusion installation.

### Building MixerX separately (original workflow)

The original standalone CMake/Visual Studio workflow still applies. Use the upstream [WohlSoft/SDL-Mixer-X](https://github.com/WohlSoft/SDL-Mixer-X) checkout instead of the old fork, and follow the upstream [guide](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/index.md#general-build-on-unix-like-platform-and-install-into-the-system). The commands below run from a `build` directory inside that checkout. This standalone workflow builds its own SDL through AudioCodecs; use the combined SDK workflow above when building OFE's pinned pair together.

The yasm troubleshooting below applies if you enable the external mpg123 decoder in the standalone build; substitute the location of your own yasm installation. The combined SDK uses dr_mp3 and does not need yasm.

if you got error about no yasm found:

```plaintext
  CMake Error at libmpg123/ports/cmake/src/libmpg123/CMakeLists.txt:230 (message):
    Couldn't find yasm assembler.  Please set YASM_ASSEMBLER variable
```

 add yasm for libmpg123 at `SDL_Mixer_X\build\build32\external\AudioCodecs\src\AudioCodecs\libmpg123\ports\cmake\src\libmpg123\CMakeLists.txt`

```cmake
set(YASM_ASSEMBLER "D:/DEV/_YASM/vsyasm.exe")
```

build for 32bit

```cmake
cmake -G "Visual Studio 17 2022" -A Win32 -S .. -B "build32" -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=ON -DAUDIO_CODECS_BUILD_LOCAL_SDL2=ON

cmake --build build32 --config Release -j 12
```

you can then build it with cmake or visual studio.

For a shared LGPL-enabled build with GPL backends disabled, use the CMake options below instead of manually removing decoder macros and linker inputs. GME is disabled here because the standalone AudioCodecs download uses its GPL MAME backend by default; the combined SDK above selects the LGPL GENS backend and retains GME support:

```cmake
cmake -G "Visual Studio 17 2022" -A Win32 -S .. -B "build32" -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=ON -DAUDIO_CODECS_BUILD_LOCAL_SDL2=ON -DMIXERX_ENABLE_LGPL=ON -DMIXERX_ENABLE_GPL=OFF -DUSE_GME=OFF -DAUDIOCODECS_BUILD_GME=OFF -DSDL_MIXER_X_SHARED=ON -DSDL_MIXER_X_STATIC=OFF
```

to debug the object, you can generate the debug info from linker->debug, or use the `RelWithDebInfo` configuration.

## License

### SDL Mixer X

SDL Mixer X:  An extended audio mixer library, forked from SDL_mixer
Copyright (C) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
Original SDL_mixer Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

## Android JNI port

The Android runtime is a JNI wrapper over the pinned SDL Mixer X backend. It
preserves the Windows ACE IDs and supports exclusive/mixing streams, fades, AB
loops, effects, encrypted inputs and handled-memory playback. See the
[Android build and compatibility notes](Android/README.md) for upstream fixes,
codec coverage, shared-device lifetime and validation limits.

`Run_Only Unicode` publishes the four-ABI Android extension ZIP through
`_Script/SDL_Mixer_Android.bat`; set `/p:SDLMixerPackageAndroid=false` to skip it.
The existing Windows SDK and release-archive workflow remain available.

See the [combined Android build/file guide](../Android_Gradle/Media-JNI.md) for
coexistence with FFmpeg, application-owned cross-frame playback, the Windows-matched
codec suite and Android file/URI permission requirements.
