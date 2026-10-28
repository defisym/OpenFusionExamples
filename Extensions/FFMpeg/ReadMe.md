# ReadMe

FFMpeg object for fusion.

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/FFMpeg/ToInstall/Files/Help/FFMpeg/FFMpeg.md)

## Building the Windows plugin

Open `FFMpeg.sln` and build a Win32 configuration. The plugin references two utility projects: `SDLDependencies` prepares the combined SDL2/MixerX SDK, and `FFmpegDependencies` downloads and prepares the prebuilt FFmpeg SDK. CMake 3.21+ is required; the Visual Studio bundled CMake is detected automatically. No FFmpeg source build or extra submodule is required.

### Selecting the FFmpeg version

Edit [`FFmpegVersion.cmake`](../_3rdLib/FFmpeg-build/FFmpegVersion.cmake) and commit it with OFE. The default is **9.0, Win32, LGPL shared** from [FFmpeg-Builds-Win32](https://github.com/defisym/FFmpeg-Builds-Win32/releases/tag/latest). Version, variant, release tag, repository and archive SHA-256 are kept together in that file. When updating, select the matching shared archive and copy its SHA-256 from the release's `checksums.sha256` or asset digest. Do not reuse a previous archive's hash.

The `latest` release can replace its assets. OFE pins the current bytes by SHA-256 and fails explicitly if they change; it does not silently upgrade or accept a new checksum. Existing prepared SDKs and cached archives can still be reused. Fresh machines need the original matching archive if it has disappeared upstream. For long-term reproducible downloads, retain releases under permanent tags with the same asset names and record that tag in the configuration.

The generated SDK is `Extensions/_3rdLib/FFmpeg-sdk`, preserving the package's `include`, `lib`, `bin`, license and other files. Downloads are cached under `FFmpeg-build/out`; both directories are ignored by Git. Preparation is serialized and reused when the configuration and required files match. Only Win32 packages are supported by this dependency target.

`FFMpegDependencies.h` reads `LIBAV*_VERSION_MAJOR` and `LIBSW*_VERSION_MAJOR` from the selected SDK headers and expands `OFE_FFMPEG_DEPENDENCIES` into Fusion's static `GetDependencies()` list. DLL major versions are not maintained in the plugin source. Preparation also verifies that all seven matching DLLs and MSVC import libraries exist and that the DLLs are x86. This synchronizes DLL names with the headers; upgrades can still require FFmpeg API changes in the plugin.

### Existing SDKs and manual preparation

Set the MSBuild property `OfeFFmpegRoot` in your user property sheet or on the build command line to keep using an existing shared SDK, such as `D:\Dev\FFMpeg`, containing `include` and `lib`. An external root disables automatic preparation by default. `OfeAutoBuildFFmpeg=false` disables it explicitly, and `OfeFFmpegCMake` selects another CMake executable if necessary. These settings are shared by the plugin and utility project through the usual platform user property sheet.

From the repository root, prepare the default SDK manually:

```powershell
cmake -DOFE_ARCH=x86 -P Extensions/_3rdLib/FFmpeg-build/EnsureSDK.cmake
```

For an offline build, add `-DOFE_FFMPEG_ARCHIVE="D:/Downloads/ffmpeg-n9.0-latest-win32-lgpl-shared-9.0.zip"`; the same pinned SHA-256 is still required. `-DOFE_FFMPEG_ROOT="D:/Dev/FFMpeg"` selects a different SDK destination for manual preparation; use the same path for `OfeFFmpegRoot` when compiling the plugin.

After rebuilding the edit-time and runtime `.mfx` files, deploy the seven FFmpeg DLLs from the SDK's `bin` directory into the Fusion root and `Data/Runtime/Unicode`, alongside the matching SDL DLLs. Dependency preparation does not copy DLLs into a Fusion installation. Check the updated `GetDependencies()` names when replacing an existing installation.
