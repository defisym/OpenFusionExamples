# FFMpeg

## Description

FFMpeg object for fusion.

Talking about the ADS...Tired about the uncontrollable gap before playing? Don't want the micro-stuttering when VSync is on? Want to play other formats like mp4? Need to protect your precious cut scenes? Then try this object!

Special thanks to @SelfReflection31, who help a lot to polishing this extension.

This object is based on cross platform lib, FFMpeg & SDL, in theory there is not too difficult to bring it to other platforms like Android.

**NOTE:** this object may conflict with other objects that need SDL, due to the dll version, and the possible double initialize (SDL needs to be initialized before using, and will fail if it's already initialized).

**NOTE:** this object uses a bitmap surface, as fusion didn't provide methods to write a HWA texture directly, and convert it will cost more than 3ms.

**NOTE:** this object didn't designed to be multi-instance, you'll encounter audio issues if use in that way. Also because of the HWA issue mentioned above, too many objects will take a long time to blit.

**NOTE:** this object runs synchronously, it will try decode video frame in `HandleRunObject` routine called once per frame. If your game is heavy then it will skip more frames. Audio decode is in another thread, but the packet queue is filled in `HandleRunObject`.

**NOTE:** this object requires FFMpeg & SDL

FFMpeg: you need to download release (e.g., `ffmpeg-n5.0-latest-win32-lgpl-shared-5.0.zip` ) from <https://github.com/sudo-nautilus/FFmpeg-Builds-Win32/releases>, then put dlls into Fusion root & `Runtime/Unicode`, or you need to build X86 version from FFMpeg source code, as the official 32 bit support has stopped.

SDL: you need to download `SDL2-2.24.0-win32-x86.zip` from <https://github.com/libsdl-org/SDL/releases>, then put X86 dlls into Fusion root & `Runtime/Unicode`

Here follows all dlls needed by this object:

- FFMpeg
  - swscale-6.dll
  - avcodec-59.dll
  - avdevice-59.dll
  - avfilter-8.dll
  - avformat-59.dll
  - avutil-57.dll
  - swresample-4.dll

- SDL
  - SDL2.dll

## Properties

- Options
  - Stretch
    - *Stretch to fit object size*
  - Play after load
    - *Auto start play when a file is loaded*
  - Loop
    - *Loop video*
    - *When playing finishes, extension will seek back to time stamp 0, and may need some time to rebuild codec context and restart playing*
  - Accurate seek
    - *To avoid display issues usually we should seek to a keyframe, but it may not that accurate. If enable this option, after seeking extension will decode frames until reaches the given time stamp, and will be time-consuming*
    - *To make this work, make sure you use seek with flag `AVSEEK_FLAG_BACKWARD`. Seek action will use this flag as default so it's transparent to users, but I still note it here as we have an action which allows you to set seek flags...*

- Queue
  - *Data is split into packet in FFMpeg, and usually it needs several packets to decode one frame. For audio, it's played in another thread and use a callback to ask for more data, so the decoded packets are cached in a queue, and won't add new ones if cached data exceeds this limit. However, some formats with higher resolution may need more data to decode, which may exceed the limit (e.g., aida_alpha.mov), then the application will keep waiting forever. In this case, extend this limit will solve it.*
  - *Note that when decoding, extension will firstly try filling up the queue. Usually it's fast but if the queue size is too big it may be time-consuming, and causes lag when seeking.*

## Action

- Open Video
  - *If open failed, extension will keep previous frame*
  - *If key is not empty, extension will treat it as an encrypted file. Decode a normal file in encrypted way usually will crash. You can encrypt files by another extension.*
  - *Encrypt & decrypt are based on AES so without your key no one can get your file, unless they dump it from memory.*

- Close Video
- Play Video
- Pause Video
- Set Volume
  - *volume is kept when opening another video*
- Set Loop
  - *loop state is kept when opening another video*
- Set Accurate Seek
  - see Properties->Accurate Seek
- Set Video Position
  - *seek to the given time, in ms*
- Set Video Position With Flag
  - *similar as above, but can let you set seek flag*  
  - *depend on the flag you use, first param is used in different ways*

  ```C
    #define AVSEEK_FLAG_BACKWARD 1 ///< seek backward
    #define AVSEEK_FLAG_BYTE     2 ///< seeking based on position in bytes
    #define AVSEEK_FLAG_ANY      4 ///< seek to any frame, even non-keyframes
    #define AVSEEK_FLAG_FRAME    8 ///< seeking based on frame number
  ```

- Set Queue Size
  - see Properties->Queue

## Condition

- Video Open
- Video Play
- Video Loop
- Video Finish
- On Video Finish
  - immediate event, triggered when video finish playing.

## Expression

- Get Video Name
- Get Video Position
- Get Video Duration
- Get Volume
- Get Video Open State
- Get Video Play State
- Get Video Loop State
- Get Video Finish State
- Get Current Video Frame Pointer
- Get Grabbed Video Frame Pointer
  - *You can get the cSurface pointer of current frame / frame with given time stamp. Pass it to another extension then do anything you like.*
  - *Note the target extension should build in /MD, aka use the same heap. Or it may crash due to access violation. See: <https://learn.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-170>*
