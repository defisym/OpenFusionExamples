# ReadMe

## Filetypes

This folder contains the following types of files:

- Interfaces or utils to work with 3rdLibs, like [FFMpeg](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/FFMpeg.h), [SDLUtilities](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/SDLUtilities.h)
- Modified 3rdLibs, like [ObjectSelection](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/ObjectSelection.h)
- 3rdLibs's release or submodule

## How to initialize

### Release

For manually maintained libraries, download the release from the developer site or clone its source into the corresponding directory. These source directories may be ignored by Git. The generated audio SDKs listed below are prepared by their dependency projects instead.

- [Eigen](https://eigen.tuxfamily.org/dox/)
- [LZ4](https://github.com/lz4/lz4)
- [ZLib](https://zlib.net/)
- [ZPaq](https://github.com/zpaq/zpaq)

### Submodule

Run `git submodule update --init <path>` from the repository root to initialize the dependencies you need, then follow their build instructions. SDL2, SDL Mixer X and SoundTouch are pinned submodules; their Visual Studio dependency projects can initialize them automatically. An ordinary clone is sufficient when using those projects.

For the other dependencies and the existing Boost setup instructions, see [Extensions/ReadMe.md](../ReadMe.md).

### Generated audio SDKs

| SDK directory | Preparation project | Used by | Instructions |
| --- | --- | --- | --- |
| `SDL-sdk` | `SDLDependencies` | SDL_Mixer and FFMpeg | [SDL2/MixerX](../SDL_Mixer/ReadMe.md#how-to-build) |
| `SoundTouch-sdk` | `SoundTouchDependencies` | SDL_Mixer | [SoundTouch](../SDL_Mixer/ReadMe.md#soundtouch-dependency) |
| `FFmpeg-sdk` | `FFmpegDependencies` | FFMpeg | [FFmpeg](../FFMpeg/ReadMe.md#building-the-windows-plugin) |

The generated SDK directories and dependency build/download caches are ignored by Git. FFmpeg uses a verified prebuilt archive; SDL2/MixerX and SoundTouch are built from pinned sources. Existing external SDKs remain supported through `OfeSDLRoot`, `OfeSoundTouchRoot` and `OfeFFmpegRoot`.

## Troubleshooting

Those libs are well-designed and are widely used in several famous infrastructure, so usually there's no compatibility issues even with the last builds. However, if the worst case occurs (e.g., a new pure virtual interface added), please feel free to open an issue.
