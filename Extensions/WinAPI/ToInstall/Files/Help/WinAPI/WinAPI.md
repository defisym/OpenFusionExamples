# Template

## Description

WinAPI, a bunch of small functions that uses Win32 API

## Properties

- Lock Mouse Properties
  - Keep Lock
    - *Keep the Mouse Locked After You Switch Back*
  - Update Lock
    - *Update Lock When Dragging/Resizing, Etc*
  - Rect Offect
    - *Keep rect offset from current window when locking mouse by rect relative to screen*

- IME Properties
  - Keep IME State
    - *Keep IME State After You Switch Back*

- Display Properties
  - Display Image
    - *Picture Captured in 'Capture Frame Area to Clipboard' will be Automaticly Displayed*

## Action

- Application
  - Run Application
    - Run Application
    - Run 16Bit Application

  - Stop Application
    - Stop Application By Name
    - Stop Application By Process ID

- Mouse
  - *you can set mouse relative to frame area, and handle stretch properly*

  - Set Mouse Position

  - Lock Mouse
    - Lock Mouse
    - Lock Mouse By Rect

  - Unlock Mouse
  
  - Settings
    - Keep Lock
      - Set On
      - Set Off

    - Update Lock
      - Set On
      - Set Off

    - Rect Offest
      - Set On
      - Set Off

- IME
  - *this extension allows you to disable and enable IME during runtime, as fusion cannot now*

  - Disable IME
  - Enable IME

- Window
  - Go FullScreen
  - Go Windowed

- Display
  - Settings
    - Stretch Quality
      - *all image will be stretched to the object size*
      - *Note: blur functions will downscale image before blur and stretch them back later. if you set quality to resample blur will take more time*
      - Fast
      - Resample

    - MultiThread Save
      - On
      - Off

  - Capture Frame Area
    - *capture the frame area, handle resize and DPI scaling properly*
    - *you can set whether save to clipboard/file or not*

  - Save
    - Save Current Image to File
    - Save Current Image to Temp
    - Save Current Image to Clipboard

  - Load
    - Load From File
    - Load From Temp
    - Load From BackDrop
      - *not work yet*
    - Load From Clipboard

  - Blur
    - Stack Blur
    - Gauss Blur
    - Recursive Gauss Blur

## Condition

- Application is a Process

- Mouse is Locked

- Return Key State
  - *check if a key is pressed, e.g. capslock*

- Application is Maximized

- Image Available in ClipBoard

## Expression

- Get Process ID By Name

- Window
  - Get Rect
    - Left
    - Right
    - Top
    - Bottom

  - Get Border Offset
    - X Offset
    - Y Offset

- Get Frame Scale
  - *get frame scale relative to it's original size, ext uses this to make sure capture&set mouse work properly*
  - X Scale
  - Y Scale

- Lock Mouse
  - Get Locked Rect
    - Left
    - Right
    - Top
    - Bottom

  - Get Current Lock Type

- Get IME State

- Get DPI Scaling
  - *Get application's DPI scale, to fix font size issue*
  - *font size issue: if you disable DPI scaling and set a string's font size to a value that euqals it's current size, the font will get bigger as DPI scaling is on incorrectly*

- Get FullScreen State

- Fusion
  - Frame
    - Get Frame Name
