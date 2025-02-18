# ReadMe

## Filetypes

This folder contains the following types of files:

- Interfaces or utils to work with 3rdLibs, like [FFMpeg](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/FFMpeg.h), [SDLUtilities](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/SDLUtilities.h)
- Modified 3rdLibs, like [ObjectSelection](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/ObjectSelection.h)
- 3rdLibs's release or submodule, like [Boost](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/_3rdLib/FFMpeg.h)

## How to initialize

### Release

Files listed in gitignore are also used but remain untouched, please download the release from the developer site or clone it from origin repo to compile.

- [Eigen](https://eigen.tuxfamily.org/dox/)
- [LZ4](https://github.com/lz4/lz4)
- [ZLib](https://zlib.net/)
- [ZPaq](https://github.com/zpaq/zpaq)

### Submodule

1. run `git submodule update --init` to update submodules
2. build according to the document. [This file](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/ReadMe.md)contains an example about how to build boost from submodule

## Troubleshooting

Those libs are well-designed and are widely used in several famous infrastructure, so usually there's no compatibility issues even with the last builds. However, if the worst case occurs (e.g., a new pure virtual interface added), please feel free to open an issue.
