# Extensions

## Dependency

### VC++ Runtime

For Windows, you should install latest [x86 VC++ Runtime](https://aka.ms/vs/17/release/vc_redist.x86.exe), aka the Visual C++ Redistributable for Visual Studio 2015-2022. Or fusion will not be able to load the extension, like not show in extension list or report error when executing standalone.

Although this runtime is frequently used and almost everyone's computer have this installed, you also should ship this file when you distributing your game/apps, e.g., a installer to auto install it, in case like a refresh installed Windows.

Platforms like Steam has tools to help you achieve this, `App Admin` -> `Edit Steamworks Settings` -> `Installation` -> `Redistributables`, check the check box of `2022 (also includes 2019, 2017 & 2015)` under `Visual C++ Redist`.

Note that you should always install the latest X86 version, as the extension is built with the latest version and utilizes the features introduces in modern C++ standards. Error may not be reported when you trying to load any extension with older version of universal runtime installed, but it may crash when executing the code that involves latest features. So please try updating the runtime when you encounter any crash.

Reasons to use dynamic instead of static link:

- [avoid potential issues](https://learn.microsoft.com/en-us/troubleshoot/developer/visualstudio/cpp/libraries/use-c-run-time)
- [share objects across extensions](https://learn.microsoft.com/en-us/cpp/c-runtime-library/potential-errors-passing-crt-objects-across-dll-boundaries?view=msvc-170)

### Fusion

All extensions has build events to auto copy the mfx to fusion install folder, in this repo is copy to the Steam folder: `C:\Steam\steamapps\common\Clickteam Fusion 2.5`

You should change it to your fusion install folder to avoid errors

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

Don't forget to copy dlls to fusion root (along with `mmf2u.exe`) and fusion runtime root (along with `edrt.exe`)!

#### Boost

you can get boost from <https://www.boost.org/>

##### Direct download

download release then unzip it to `_3rdLib/Extensions/Boost`

##### Submodule

reference: <https://github.com/boostorg/wiki/wiki/Getting-Started%3A-Overview>

1. open git bash, then cd to this repo
2. run `git submodule update --init` to init submodule, which will clone boost first
3. cd boost folder
4. run `git submodule update --init` again to init submodule in boost repo
5. run `./bootstrap.bat` to build `b2.exe`
6. run `./b2 header`, which will build headers to boost folder, just like the release you downloaded

now you should build the extensions rely on boost

#### FFMpeg

Extension relies on X86 LGPL Shared 7.1

Located at the same level of repo folder, aka in repo's parent folder

You can get auto builds here:

- ~~<https://github.com/sudo-nautilus/FFmpeg-Builds-Win32/releases/tag/latest>~~
- <https://github.com/defisym/FFmpeg-Builds-Win32>

#### SDL

<https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5>

Located at the same level of repo folder, aka in repo's parent folder

#### SDL Mixer

Originally made by WohlSoft, this repo uses a forked version:

<https://github.com/defisym/SDL-Mixer-X>

You can build it following the readme of repo and `SDL_Mixer` extension, then put dependency to the SDL folder, `include` & `lib`

Located at the same level of repo folder, aka in repo's parent folder

#### SoundTouch

<https://www.surina.net/soundtouch/>

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
