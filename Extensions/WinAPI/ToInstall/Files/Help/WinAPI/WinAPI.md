# Template

## Description

WinAPI, a bunch of small functions that uses Win32 API

## Properties

- Lock Mouse Properties
  - Keep Lock
    - *Keep the Mouse Locked After You Switch Back*
  - Update Lock
    - *Update Lock When Dragging/Resizing, Etc*
  - Rect Offset
    - *Keep rect offset from current window when locking mouse by rect relative to screen*

- IME Properties
  - Keep IME State
    - *Keep IME State After You Switch Back*

- Display Properties
  - Display Image
    - *Picture Captured in 'Capture Frame Area to Clipboard' will be Automatically Displayed*

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

    - Rect Offset
      - Set On
      - Set Off

- IME
  - *this extension allows you to disable and enable IME during runtime, as fusion cannot now*

  - Disable IME
  - Enable IME

- Window
  - Go FullScreen
  - Go Windowed

- Get FileList
  - *Get all files (including sub-folders)*

- Display
  - Settings
    - Default File Format
      - *set default file format*
      - *save action uses the format you inputted in filename, e.g. 'File.bmp', ext will save file to '.bmp'. if the format is not supported by fusion's image filter, ext will save as the format you defined here*
      - *if default format not set, ext will save image as '.jpg'*

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
  - *check if a process of given name is running*
- Another Instance is Running
  - *check if another instance of current application is running, able to check by name or file path*

- Mouse is Locked

- Return Key State
  - *check if a key is pressed, e.g. capslock*

- Application is Maximized

- Image Available in ClipBoard

- Fusion  
  - Object
    - Object has Animation in Direction
      - *check if object's direction has animation*

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
  - *font size issue: if you disable DPI scaling and set a string's font size to a value that equals it's current size, the font will get bigger as DPI scaling is on incorrectly*

- Get FullScreen State

- Time
  - Get Current Time
    - *output a time string in syntax YYYY-MM-DD HH:MM:SS, e.g. 2000-01-01 00:00:01*
  - Get Total Play Time
    - *input total play frame number and return the time string in syntax HH:MM:SS, e.g. 00:00:01, Max 99999999:59:59*

- FileList
  - Get FileList Size
  - Get FileList At

- Hash
  - Get File Hash (MD5)

- Fusion
  - Frame
    - Get Frame Name
  - Object
    - Get Object Display Animation Direction
      - *fusion will use another direction's animation when object's direction is set to a direction that doesn't have animation, this expression will do some calculation and return the displayed animation direction*

- Misc.
  - Get Val With Sign
    - *add signs in the beginning of numbers when converting to string, available for positive ones*
