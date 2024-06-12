# ReadMe

## Introduction

This is a Repo of several open source stuff about [Clickteam Fusion](https://www.clickteam.com/).

## How to Contribute

If you have something good and want to share, just simply fork this repo, then put files in a folder named by your works, place this folder by it's type, e.g. shaders in shaders folder, add a explanation to this file (ReadMe.md, alphabetically), and feel free to send a PR.

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=defisym/OpenFusionExamples&type=Date)](https://star-history.com/#defisym/OpenFusionExamples&Date)

## Catalogue

### Extensions

#### Extension

| Name               | Author  | Platform         | Released | CEM | Description                                                                                                                        |
| ------------------ | ------- | ---------------- | -------- | --- | ---------------------------------------------------------------------------------------------------------------------------------- |
| AndroidUtilities   | Defisym | Android          | Y        | N   | some features for android like extract binary files, etc                                                                           |
| Clipboard          | Defisym | Windows          | N        | N   | small tool that can save / load fusion clipboard contents                                                                          |
| Deque2D            | Defisym | Windows          | N        | N   | A modern version of MagicDeque based on std::variant                                                                               |
| EffectUtilities    | Defisym | Windows          | N        | N   | let you control shaders more efficiency, like cache texture params                                                                 |
| Encrypter          | Defisym | Windows          | Y        | N   | En/decrypt file with WinAPI CNG. You can Encrypt file here and then read them by other extensions I made, e.g. Fini/StringSplitter |
| EpicOnlineServices | Defisym | Windows          | N        | N   | Achievements and stats for Epic Game Store                                                                                         |
| FFMpeg             | Defisym | Windows          | Y        | Y   | bring the legendary player to fusion, with several handy A/C/Es like blit current video frame, etc                                 |
| FindTheWay         | Defisym | Windows, Android | Y        | N   | A\* Path Find, open source, 5X faster than Path Finding Object, all runtime compatible (currently available on windows & android). |
| Fini               | Defisym | Windows          | Y        | Y   | based on brofield's brilliant work simpleini, is a faster ini ext for fusion.                                                      |
| Func               | Defisym | Windows          | Y        | N   | Call Functions via Expression or Action                                                                                            |
| NeoPic             | Defisym | Windows          | N        | N   | Another active picture, but with encrypt support, all object shares a lib that over frame, significantly reduce load time          |
| NeoStr             | Defisym | Windows          | N        | N   | Powerful string extension with format control support                                                                              |
| RandomTable        | Defisym | Windows          | Y        | N   | Generate a random table like **Fire Emblem**, save/load by base64 string, for my own game.                                         |
| SDL_Mixer          | Defisym | Windows          | Y        | Y   | Audio player based on SDL Mixer X, with unlimited channel & encrypt support                                                        |
| SteamworksAPI      | Defisym | Windows          | N        | N   | alternative steamworks API object with modern feature, compatible (possibly) with current steamworks object                        |
| String Splitter    | Defisym | Windows          | N        | N   | This object is a string splitter base on C++ regex.                                                                                |
| WinAPI             | Defisym | Windows          | Y        | N   | a bunch of small functions.                                                                                                        |

#### Lib

| Name      | Author  | Description                                               |
| --------- | ------- | --------------------------------------------------------- |
| _3rdLib   | Defisym | something that used to work with or modified from 3rd lib |
| _DeLib    | Defisym | several functions that my extensions share                |
| _Template | Defisym | Ext template, but with something custom                   |

#### Utilities

| Name      | Author  | Description                                    |
| --------- | ------- | ---------------------------------------------- |
| _BuildAll | Defisym | Python script to build all extensions          |
| _Script   | Defisym | Publish multi-platform extensions by one click |

### Shaders

| Name             | Author  | Description                                                                                                                         |
| ---------------- | ------- | ----------------------------------------------------------------------------------------------------------------------------------- |
| AlphaMono        | Defisym | make semi-mono effect                                                                                                               |
| Compression      | Defisym | Simulate Compression effect                                                                                                         |
| DotMatrix        | Defisym | Simple dot matrix effect                                                                                                            |
| Gauss Blur       | Defisym | Finally here comes the gauss blur shader for DX11, GaussBlur2D is very slow, use GaussBlur instead, or two GaussBlur1D for X/Y blur |
| Heatwave         | Defisym | Create heatwave effect                                                                                                              |
| Pattern Fade     | Defisym | make gorgeous transition effects                                                                                                    |
| Ring Health Bar  | Defisym | simple health bar                                                                                                                   |
| Round Health Bar | Defisym | simple health bar again                                                                                                             |
| Round Rectangle  | Defisym | draw a round rectangle                                                                                                              |

### Tools

| Name                     | Author   | Description                                                                                                                                                                                                                                                                                                                                                                                                    |
| ------------------------ | -------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| NeoStr Dialog Editor     | Cnohares | a Dialog Editor with many NeoStr format functions, just as Fonts,Size,Color,Shake and so on. You can use it to edit dialog more easily. Notice that it's only have Chinese comments now.                                                                                                                                                                                                                       |
| CallofAS Toolbar         | Cnohares | a handy tool bar set                                                                                                                                                                                                                                                                                                                                                                                           |
| DX11 Shader Compile Tool | Defisym  | compile DX11 shaders by a simple drag                                                                                                                                                                                                                                                                                                                                                                          |
| FusionClipboard          | Defisym  | small tool that can save / load fusion clipboard contents                                                                                                                                                                                                                                                                                                                                                      |
| JDK Terminator           | Defisym  | Fusion will use JDK to build Android applications, but unfortunately, sometimes JDK process will not terminate correctly even you quit fusion. If you are using steam version, steam will think fusion is still running and refuse to restart fusion or steam itself, unless you terminate the process manually in task manager, which is annoying. This script can help you terminate it with a single click. |

### Examples

| Name             | Author   | Description                                                                                                              |
| ---------------- | -------- | ------------------------------------------------------------------------------------------------------------------------ |
| BinaryExtract    | Defisym  | Extract binary file to any folder you want                                                                               |
| Blank Char       | Defisym  | blank char used for align to git rid of DX11 string limit                                                                |
| Bullet Time      | Defisym  | A simple bullet time example                                                                                             |
| DecimalVerdict   | Defisym  | To check if a value has decimals                                                                                         |
| DirNormalization | Defisym  | convert fusion directions (0 ~ 31) to 4Dir (0 ~ 3)                                                                       |
| EasyBezier_v2    | Cnohares | A WYSIWYG Bezier example. You can modify points and see the curve result directly, and move object with easing effect.   |
| EasyStack        | Defisym  | Push/Pop value without extension                                                                                         |
| Ellipse          | Defisym  | move like a ellipse                                                                                                      |
| FastloopSave     | Defisym  | Save by Fastloop, could be easily extended to fit any systems                                                            |
| Flow             | Defisym  | a common error of beginners about flow control, like esc open/close menu but only one side can be triggered              |
| IniControlInit   | Defisym  | How to init a ini file                                                                                                   |
| MouseState       | Defisym  | to show if LMB/RMB is pushed down                                                                                        |
| PauseWithTextBox | Defisym  | pause app with an explanation                                                                                            |
| Regex_IsNum      | Defisym  | check if a string is number by using string splitter                                                                     |
| Shotgun          | Defisym  | Simple Shotgun                                                                                                           |
| StringScan       | Defisym  | Search the string and excerpt text in ()                                                                                 |
| Stupid Drag      | Defisym  | Drag to move camera                                                                                                      |
| Stupid HeatWave  | Defisym  | simple heatwave effect                                                                                                   |
| Stupid SRPG      | Defisym  | a SRPG example like **Fire Emblem** I wrote in 2017. it's totally a mess of spaghetti code, but it's enough for a start. |
| SubStretch       | Defisym  | Stretch Sub-application properly with WinAPI                                                                             |

### Tutorials

Some mfa files used for Fusion tutorials in [my blog](https://www.zhihu.com/column/FusionGamers).
