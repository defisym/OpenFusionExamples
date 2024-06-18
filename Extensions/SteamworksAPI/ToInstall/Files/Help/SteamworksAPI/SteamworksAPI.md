# SteamworksAPI

## Description

Alternative Steamworks object, compatible (maybe) with ten-years-old original one.

## Update SDK

as lib file will use steam_api.dll by default, which is conflict with built-in ones, so we need to modify it to load new ones...

- download SDK from valve: <https://partner.steamgames.com/dashboard>
- put the SDK in the same level of repo folder
- download pexports from: <https://sourceforge.net/projects/mingw/files/MinGW/Extension/pexports/>, or use `dumpbin` in vs prompt with some manual work
- export def: `pexports steam_api.dll > SteamworksAPI.def`
- edit`SteamworksAPI.def`, `LIBRARY steam_api.dll --> LIBRARY SteamworksAPI.dll`
- generate new lib file in vs prompt: `lib /def:SteamworksAPI.def /machine:i386 /out:SteamworksAPI.lib`
- copy `SteamworksAPI.lib` to `redistributable_bin` folder of steamworks SDK for linker
- copy new version of `steam_api.dll` to fusion folder, then rename it to `SteamworksAPI.dll`

## Usage

You need [`steam_appid.txt`](https://partner.steamgames.com/doc/api/steam_api#SteamAPI_Init) to debug, you need to put this file to Data->Runtime->Unicode to work in editor, and put mfa folder for standalone. Note that some features may not work in steam version fusion's editor like steam overlay.

If your game supports other platform, e.g., Epic, you can create a file along with the execution file to disable everything. You should grantee no platform features will be called, or may crash. The easiest way is use `Platform Enabled` condition to disable event group.

for example, exe path is `C:\YourGame\Game.exe`, then you can create file `C:\YourGame\Platform\NoSteam` to disable `SteamworksAPI`. This filename is hard coded.
