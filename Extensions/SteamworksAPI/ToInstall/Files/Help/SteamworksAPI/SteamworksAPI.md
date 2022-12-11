# SteamworksAPI

## Description

Alternative Steamworks object, compatible (maybe) with ten-years-old original one.

## Update SDK

as lib file will use steam_api.dll by default, which is conflict with built-in ones, so we need to modify it to load new ones...

- download SDK from valve: <https://partner.steamgames.com/dashboard>
- download pexports from: <https://sourceforge.net/projects/mingw/files/MinGW/Extension/pexports/>, or use `dumpbin` in vs prompt with some manual work
- export def: `pexports steam_api.dll > neoSteam_api.def`
- edit`neoSteam_api.def`, `LIBRARY steam_api.dll --> LIBRARY neoSteam_api.dll`
- generate new lib file in vs prompt: `lib /def:neoSteam_api.def /machine:i386 /out:neoSteam_api.lib`
- copy new version of `steam_api.dll` to fusion folder, then rename it to `neoSteam_api.dll`
