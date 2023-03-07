# ReadMe

## Description

Audio Extension based on [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) & [SDL_Mixer_X](https://github.com/WohlSoft/SDL-Mixer-X), with encryption support

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/SDL_MIXER/ToInstall/Files/Help/SDL_MIXER/SDL_MIXER.md)

## How to build

follow the [guide](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/index.md#general-build-on-unix-like-platform-and-install-into-the-system)

if you got error about no yasm found, add yasm for libmpg123 at `SDL_Mixer_X\build\build32\external\AudioCodecs\src\AudioCodecs\libmpg123\ports\cmake\src\libmpg123\CMakeLists.txt`

```cmake
set(YASM_ASSEMBLER "F:/DEV/_YASM/vsyasm.exe")
```

build for 32bit

```cmake
cmake -G "Visual Studio 17 2022" -A Win32 -S .. -B "build32" -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=ON -DAUDIO_CODECS_BUILD_LOCAL_SDL2=ON

cmake --build build32 --config Release -j 12
```
you can then build it with cmake or visual studio.

To disable GPL part, remove macros like `MUSIC_MP3_MAD`, `MUSIC_MID_ADLMIDI`, `MUSIC_MID_OPNMIDI` and `MUSIC_FLAC` in properties, then remove the counterpart input of linker, or using the following code to generate a clipped project:

```cmake
cmake -G "Visual Studio 17 2022" -A Win32 -S .. -B "build32" -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=ON -DAUDIO_CODECS_BUILD_LOCAL_SDL2=ON -DMIXERX_ENABLE_LGPL=ON -DSDL_MIXER_X_SHARED=ON -DSDL_MIXER_X_STATIC=OFF
```

to debug the object, you can generate the debug info from linker->debug, or use the `RelWithDebInfo` configuration.
