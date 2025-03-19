# NeoStr

## Description

String that based on GDI Plus, with scaling, rotating & format control support

## Tab

- DX9: tab size = 8, em space on
- DX11: tab size = 4, em space off

## Format Control

### Escape

`\[` -> `[`

`[Command]` if not match follows, will be displayed as untouched depend on your flag settings

### Ignore

`[^ = Exclude, ...]` ignore all formats after this exclude given ones

### Stop Ignore

`[!^]` stop ignore all formats

### Reset

`[!]` reset all format to what you set in properties and events

### Values General

- ! = reset to default
- +/- = add/minus to current
- \ = use given value directly, including sign
  - e.g., current is `1.0`
    - `-0.5` -> `1.0` - `0.5` = `0.5`
    - `\+0.5` -> `+0.5`
    - `\-0.5` -> `-0.5`

### Trigger

#### Insert

`[Trigger = TriggerName, Content][/Trigger]`

Embraced characters are trigger rect.
As text may include ',', parse is started from left, unlike other formats

- *if you use content, make sure it doesn't have open `[]`*

### Tag

#### Insert

`[Tag = CallbackName, Params]`

Trigger callback when render to it, tags in remarks will be ignored.
*you need to retrieve each param then handle it by your self*

### Remark

#### Insert

`[Remark = CharCount, Content]` insert remark that display over texts to the following characters.
As text may include `,`, parse is started from left, unlike other formats. Remark is rendered in another NeoStr class, with the same config (color, font, etc), but overrode position & render object, and the font size is half of the parent object when it starts rendering.

- *if you use content, make sure it doesn't have open `[]`*
- *if remarked string changed line, then the render of its remark is skipped*
- *if the font / size changed too drastically there will be overlap*
- *position of remark is estimated by the mean size of remarked characters*

#### Format

`[RemarkOffsetX = 0.0][/RemarkOffsetX]` Remark Offset X

See `Values General`

`[RemarkOffsetY = 0.0][/RemarkOffsetY]` Remark Offset Y

See `Values General`

### ICon

#### Insert

`[ICon = Direction, Frame]` insert icon based on linked active. if param is less than two, will be referred from left. e.g., one param will be treated as frame.

ICon will be resized to the font size it inserted at.
*if you are linking to object you need to retrieve each param then handle it by your self you need to set a key for it for retrieving and value as pointer to a surface*

#### Format

`[IConOffsetX = 0.0][/IConOffsetX]` ICon Offset X

See `Values General`

`[IConOffsetY = 0.0][/IConOffsetY]` ICon Offset Y

See `Values General`

`[IConScale = 1.0][/IConScale]` ICon Scale

See `Values General`

`[IConResample = 1][/IConResample]` ICon Resample, 1 = Enable, 0 = Disable

- ! = reset to default

### Align

`[Align = DT_LEFT]`

change align of different lines, DT_LEFT = 0, DT_CENTER = 1, DT_RIGHT = 2

### Char Offset

`[CharOffsetX = 0.0][/CharOffsetX]` Char Offset Y

See `Values General`

`[CharOffsetY = 0.0][/CharOffsetY]` Char Offset Y

See `Values General`

### Shake

`[Shake = Type, Amplitude, TimerCoef, CharOffset]`control shake.

if param is less than four, will be referred from right.
e.g., one param -> Type, two params -> Type & Amplitude, etc.

- type: shake type, default is None, can be X/Y/Random. X/Y is based on cosine.
- amplitude: default is 1.0, relative to character size. size = 30, amplitude = 0.5, will shake in range of [ -15 ~ 15 ]
- timer coef: default is 1.0, shake faster when higher.
- char offset: default is 1.0 / 6.0, relative to 360 degree. charOffset = 0.2, actual offset is 72 degree. determines the movement interval of adjacent characters. only works in X/Y mode ( cosine )

### Color

`[Color = #FFFFFFFF][/Color]`&`[C = #FFFFFFFF][/C]` color, hex AARRGGBB, or A, R, G, B

See `Values General`

if RGB param is less than four, then it's referred in the following order:

- R
- R G
- R G B
- A R G B

### Font

#### Font

`[Font = FontName][/Font]`&`[F = FontName][/F]`

- ! = reset to default

#### Size

`[Size = FontSize][/Size]`&`[S = FontSize][/S]`

See `Values General`

#### Bold

`[Bold][/Bold]`&`[B][/B]`

`[!Bold][/!Bold]`&`[!B][/!B]`

set to non-bold

#### Italic

`[Italic][/Italic]`&`[I][/I]`

`[!Italic][/!Italic]`&`[!I][/!I]`

set to non-italic

#### Underline

`[Underline][/Underline]`&`[U][/U]`

`[!Underline][/!Underline]`&`[!U][/!U]`

set to non-underline

#### Strike Out

`[StrikeOut][/StrikeOut]`&`[S][/S]`

`[!StrikeOut][/!StrikeOut]`&`[!S][/!S]`

set to non-strike out

## Properties

### Display

- Hotspot
  - Default
  - X
  - Y

### Font

- Font
  - Vertical offset
  - Row space
  - Col space
  - Tab size
  - Tab EM space
    - *use EM space instead of space*

- Render
  - Clip
    - *if text is out of visible area, skip it's render*
  - Clip to object
    - *if text is out of object area, skip it's render*
  - Border offset
    - *internal offscreen surface is bigger than render size, to give enough space for shader, icon, remark, etc*
  - Text Rendering Hint
    - *See GDI Plus document*
  - Smoothing Mode
    - *ditto*
  - PixelOffset Mode
    - *ditto*

- Format
  - Filter unknown
    - *will not display unknown command*
  - Filter incomplete
    - *will not display incomplete command, aka open `[` at end*

  - ICon global
    - *enabled objects will refresh automatically if icon source changed*
  - ICon global force update
  - ICon resample
  - ICon offset x
  - ICon offset y
  - ICon scale

  - Remark offset x
  - Remark offset y

## Action

- Embed Font
  - *TL;DR: you **MUST** use **this action** to embed font*
  - *Explanation*
    - *Fusion's font system works differently under different runtimes. For windows, DX9 relies on GDI and DX11 relies on other font libs*
    - *For DX9, aka GDI, you can use the font embed object, which is a wrapper of `AddFontResourceEx` or it's similar. You need to ship fonts with your game or embed it in binary files then handle it by yourself.*
    - *For DX11, fusion auto embed referred fonts. However, the document was outdated so there's no info about this mechanism (there's a feature called font bank on other runtimes including Android, and there's a few documents about the counterpart of images. But as for me, the attempt to utilize it to auto embed fonts in DX11 failed due to the lack of document).*
    - *This extensions relies on GDI plus, a successor of GDI for now (and there's no grantee to not move to other font libs in the future), which is a different render system comparing to Fusion, so the previous methods (font embed object for GDI in DX9 and built-in embed in DX11) to embed font will not work except **this action***
    - *Note that this object also embed GDI font, so you can replace that object safely if needed*

- Change Display String
- App Display String
- App Display String at NewLine

- Render
  - Render
    - *render is delayed to display routine, or expression that need render result, you can force it here*

  - Change Render Size
    - *resize object during runtime*
  - Change Visible Ration
    - *let character be displayed gradually, displayed char / total char, 1.0 -> display all*
    - *include alpha: E.g., 10 chars*
      - *not included: 5 / 10 = 0.5, display 5 chars*
      - *included: (4 * 255 + 125) / 10 * 255 = 0.45, display 4 char, last with 50% extra transparency*
  - Change Tag Callback Index
    - *only tag's index (location in the raw string) lager than this will be triggered*
    - *default is `-1`*
  - Change Tag Callback Index Managed
    - *default is true*
    - *reset to false if you change the index manually*
    - *index is update by the following rules:*
      - *after render, the index will be set to the current raw string length*
      - *when using `Change Display String`, if it's not an append of previous string, then the index is reset to 0, aka trigger all*

  - Force Redraw
    - *if no change, last render result will be used, this action will force redraw even not change*
  - Force Redraw Global ICon
    - *redraw all objects with `ICon global` and `ICon global force update` checked*

- Alignment
  - Change Row Space
  - Change Col Space

  - Change Vertical Offset
    - *when vertical align is set to center, some font may have an offset, use this to fix it*

  - Change Horizontal Align
    - *DT_LEFT = 0, DT_CENTER = 1, DT_RIGHT = 2*
  - Change Vertical Align
    - *DT_TOP = 0, DT_VCENTER = 4, DT_BOTTOM = 8*

  - Change Tab Settings

- Scroll
  - *use object size as viewport, scroll content inside it*

  - Disable
  - Set Scroll Coef
    - *will enable scroll automatically*

- Zoom && Rotate
  - Change HotSpot

  - Zoom
  - Stretch

  - Rotate

- Format
  - Link Active
    - *if you link active, the icon will be read from it's animation frames*

  - Link Object
    - *if you link object, then you need to analyze icon param by yourself in callback*
  - Set ICon Key Value
    - *return the surface pointer here*

  - Set Filter Flag

  - Set ICon Offset
  - Set ICon Scale
  - Set ICon Resample

  - Set Remark Offset

- Format String
  - New Format
    - *`std::format` like format string. All params is string, so please format value to string first*
    - *if format failed, the format string will be returned*

  - Add String Param
  - Add Value Param

## Condition

- On Get Object ICon Callback
  - *if you link to a object, this will be triggered to ask for icon surface pointer*

- On Tag Callback
- On Tag Callback Forward
  - *triggered for each callback, for forwarding*

- Position Overlap Trigger Rect
  - *this condition will call render, so if you call immediate events inside render, e.g., tag, fusion above 295 will crash*

## Expression

- Get Font Family Name

- Get Display String
- Get Filtered String
  - *filter given string*
    - `FORMAT_IGNORE_UNKNOWN = 0b00000001`
    - `FORMAT_IGNORE_INCOMPLETE = 0b00000010`
    - `FORMAT_IGNORE_AllowEmptyCharStringAfterFormatParsing = 0b00000100`
- Get Padding String
  - *replace content in `[]` to `□`, as control characters may conflict with you owns*
- Get Raw String By Filtered String Length
- Get Non Command Offset
  - *if next char is a command, this expression will return the offset to command*

- Render Options
  - Get Visible Ratio
  
  - Get Tag Callback Name

  - Get Tag Param Num
  - Get Tag Param String

- Alignment
  - Get Row Space
  - Get Col Space

  - Get Vertical Offset

  - Get Horizontal Align
  - Get Vertical Align

  - Get Tab Size
  - Get Tab Use EM Space

- Size
  - Get Max Width
  - Get Total Height

  - Get Render Width
  - Get Render Height

- Character Position
  - Get Char X
  - Get Char Y

  - Get Last Char X
  - Get Last Char Y

- Scroll
  - Get Scroll Coef X
  - Get Scroll Coef Y

- Zoom && Rotate
  - Get HotSpot X
  - Get HotSpot Y

  - Get X Scale
  - Get Y Scale

  - Get Angle

- Format
  - Get Hashed String
    - *if you parser icon command by yourself, then this is helpful to use it as cache key*
  - Get ICon Param Num
  - Get ICon Param String

  - Get Filter Flag

  - Get ICon Offset X
  - Get ICon Offset Y
  - Get ICon Scale
  - Get ICon Resample

  - Get Remark Offset X
  - Get Remark Offset Y

- Format String
  - Get Format String
