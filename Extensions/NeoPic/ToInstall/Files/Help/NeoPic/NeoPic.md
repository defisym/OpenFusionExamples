# NeoPic

## Description

Enable the power to load [encrypted](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files) image files, to store them to a lib that keeps data over frames, or to do something like stack blur or affine transformation.

Special thanks to @PiKeyAr (From Github) and @Senda (From Discord), who help a lot to polishing this extension.

**Note** only one lib is maintained, so create multiple lib objects has no effect.

## Known issue

- due to fusion's image filter limitation, doesn't support PNG-8

## Properties

- Lib Settings
  - Lib Object
    - *Lib Object will hold surfaces for other objects, you can retrieve them by filename. Lib will keep them over frames.*
  - auto clean
    - *auto clean data if exceeds the following limit*
  - memory limit
  - size limit

- Load Options
  - Load Callback
    - *trigger a callback for you to process it if a new file is loaded*

- Display
  - HWA

- Quality
  - Resample

- Hotspot
  - default hotspot

## Action

- Load
  - Set Load Callback
  - Update Load Callback Surface
  
  - Load From File
    - *keep key empty if file is not encrypted*
  - Load From Pointer
    - *clone target surface*

  - Load From Lib
    - *keep key empty if file is not encrypted*
    - *if target lib doesn't contain target file, lib will try loading that file first, if success, it will be added to lib*
  - Load From Display
    - *clone from another non-lib NeoPic object*

  - Capture FrameArea
    - *if capture by lib then it's saved by access name `_TempCapture` for later use*

- Preload
  - Set Preload Path
    - *preload all files in given path in background, `On Preload Complete` is triggered when background loading completed*
    - *if the lib object triggered loading is destroyed, then the loading will be paused, until another valid lib object is created*
    - *images loaded in background is bitmap due to fusion's limitation*
    - *preload will only use half of the memory limit to reserve enough memory for other objects, as they may also allocate memory before preload finishes and auto clean triggers, which may causes random crash*

  - Set Preload List
    - *preload given files in base path, delimited by `|`*
  - Set Preload List By Pointer
    - *preload given files in `std::vector<std::wstring>*` in base path*

  - Set Keep List
  - Set Keep List By Pointer
    - *same as above, files in this list won't be auto cleaned*

  - Iterate Ref Count

- Save
  - Save to File
  - Save to File With Stretch
    - *save is a wrapper of `get surface pointer`, so the behaviour is the same: fileName & key are ignored for display object and return it's display surface directly, and lib object will try to load it first if not exist*
    - *file format is set by save file name's extension, e.g., `FileName.png` will save as `png` but `FileName.jpg` is `jpg`. if no valid extension then it's saved as `jpg` by default*

- Lib
  - Attach Object To Lib
    - Attach without copy by using load by pointer, to speed up huge bitmap process speed, works like `std::move`
  - Detach Object From Lib

  - Reset Lib
    - *release all non-refed files*
  - Clean Cache

  - Erase Lib Item
    - *erase the given item if not in use*
  - Update Lib Item
    - *update the given item if the file hash changed*

- Animation
  - Set Animation Source
  
  - Set Animation Speed
  
  - Set Animation Step
    - *set interpolation step*
  - Set Animation Frame ID
    - *Frame ID: including interpolation frames*
  - Set Animation Frame Index
    - *Frame Index: frame you defined in json*
  
  - Load Animation
  - Stop Animation
    - *called automatically if conflict occurs with other features*
  
  - Pause Animation
  - Resume Animation

- Nine Slice
  - Set Nine Slice Source

  - Load Nine Slice
  - Reset Nine Slice
  - *called automatically if conflict occurs with other features*

- Display
  - Settings
    - Set HotSpot
    - Set Quality

  - Transform
    - Zoom
    - Stretch

    - Offset
    - Rotate

    - Stack Blur
    - Fill Mosaic
    - Perspective
    - Set Transparent Color

    - Set Effect Param
      - *set object's effect param, can ref lib's pointer as image parameter*

    - Fill Mosaic
      - *fill mosaic using current image*

  - Add Backdrop

## Condition

- Can Display
- Current Display Transparent
  - *check if nothing to display*

- Lib Has Item

- On Load Callback
- On Preload Complete

- On Iterate Ref Count

- Animation
  - On Animation Finished
  - Is Animation Finished
  - Is Animation Playing
  - Is Animation Paused

## Expression

- Load
  - Get Load Callback FileName
  - Get Load Callback Surface Pointer

  - Get Pic FileName
  - Get File FileName
    - *`..\\path\\fileName.ext`, return `fileName.ext`*

  - Get Pic FilePath
  - Get Pic Relative FilePath
  - Get File Relative FilePath
  
  - Get Key
  
  - GetSurfacePointer
    - *if exists, return the target surface's pointer*
    - *make sure you **KNOW WHAT YOU ARE DOING WITH THIS EXPRESSION***

  - Get Iterate Ref Count Key
  - Get Iterate Ref Count Value Count
  - Get Iterate Ref Count Value Priority

- HotSpot
  - Get X HotSpot
  - Get Y HotSpot

- Size
  - Get Original Width
  - Get Original Height

  - Get Current Width
  - Get Current Height

- Zoom
  - Get X Zoom Scale
  - Get Y Zoom Scale

- Rotate
  - Get Angle

- Animation
  - Get Animation File Name
  - Get Animation Name
  
  - Get Animation Speed
  
  - Get Animation Step
  - Get Animation Frame ID
  - Get Animation Frame Index

- Hibiscus AVG Engine
  - Get AVG Coord X
  - Get AVG Coord Y
    - *do coord conversion for my own game*

- System Info
  - *you need to enable `DXGI` macro then rebuild it to enable all expressions (or return -1 / error message), then this ext will not be compatible with windows 7*
  - Get RAM Usage MB
  
  - Get GPU Name
  
  - Get VRAM Usage MB
  - Get VRAM Budget MB
