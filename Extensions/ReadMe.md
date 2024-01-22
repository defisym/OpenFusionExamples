# Extensions

## Dependency

### Fusion

All extensions has generate event to auto copy the mfx, to the Steam folder: `C:\Steam\steamapps\common\Clickteam Fusion 2.5`

### Fusion SDK

#### Windows

The repo is organized like the official [Fusion SDK](https://community.clickteam.com/forum/thread/89104-fusion-2-5-sdk-release/).

If you unzip Fusion SDK, you can find those folders:

- Inc
- Lib
- Extensions

Please copy `Inc` to the same level of `Extensions`, aka root of repo, and `Lib` in `Extensions` folder.

#### Android

unzip the [Android SDK](https://community.clickteam.com/forum/thread/89105-official-android-sdk-release/) in `Extensions/Android_Gradle` folder

#### HTML & iOS

the same as Android

### Open source libs

#### Note

Don't forget to copy dlls to fusion root and fusion runtime root!

#### Boost

https://www.boost.org/

Located at `C` root

#### FFMpeg

Extension relies on X86 LGPL Shared 5.1

Located at the same level of repo folder, aka in repo's parent folder

You can get auto builds here: 
- https://github.com/sudo-nautilus/FFmpeg-Builds-Win32/releases/tag/latest
- https://github.com/defisym/FFmpeg-Builds-Win32

#### SDL

https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5

Located at the same level of repo folder, aka in repo's parent folder

#### SDL Mixer

Originally made by WohlSoft, this repo uses a forked version:

https://github.com/defisym/SDL-Mixer-X

You can build it following the readme of repo and `SDL_Mixer` extension, then put dependency to the SDL folder, `include` & `lib`

Located at the same level of repo folder, aka in repo's parent folder

#### SoundTouch

https://www.surina.net/soundtouch/

Located at the same level of repo folder, aka in repo's parent folder

#### Steamworks

Located at the same level of repo folder, aka in repo's parent folder

#### Epic Online Service

Located at the same level of repo folder, aka in repo's parent folder

#### 3rdLib

`Extensions\_3rdLib` uses some lib's source, please follow the readme in that folder, download the source then put it in the name of it, e.g.,:

- _3rdLib
  - Eigen
  - nlohmann
  - ...
