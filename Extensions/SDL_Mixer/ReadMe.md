# ReadMe

## Description

Audio Extension based on [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) & [SDL_Mixer_X](https://github.com/WohlSoft/SDL-Mixer-X), with [encryption](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files) support

Audio Effect powered by [SoundTouch](http://www.surina.net/soundtouch/index.html)

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/SDL_MIXER/ToInstall/Files/Help/SDL_MIXER/SDL_MIXER.md)

## Reference

### SDL Mixer X

- [Music document](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/5_5_functions_music.texi)
- [Play music example](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/examples/playmus.c)

### Sound Touch

- [FAQ](https://www.surina.net/soundtouch/faq.html)
- [Example](https://codeberg.org/soundtouch/soundtouch/src/branch/master/source/SoundTouchDLL/DllTest/DllTest.cpp)
- [Theory behind](https://www.surina.net/article/time-and-pitch-scaling.html)

## How to build

This extension added a few custom functions to original lib, so please clone [this repo](https://github.com/defisym/SDL-Mixer-X) first, then follow the [guide](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/index.md#general-build-on-unix-like-platform-and-install-into-the-system)

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

To disable GPL part, remove macros like `MUSIC_MP3_MAD`, `MUSIC_MID_ADLMIDI`, `MUSIC_MID_OPNMIDI` and `MUSIC_FLAC` in properties, then remove the counterpart input of linker, or using the following code to generate a clipped project:

```cmake
cmake -G "Visual Studio 17 2022" -A Win32 -S .. -B "build32" -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_AUDIO_CODECS_DEPENDENCY=ON -DAUDIO_CODECS_BUILD_LOCAL_SDL2=ON -DMIXERX_ENABLE_LGPL=ON -DSDL_MIXER_X_SHARED=ON -DSDL_MIXER_X_STATIC=OFF
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
