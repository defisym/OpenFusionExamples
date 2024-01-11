# NeoStr

## Description

String that based on GDI Plus, with scaling, rotating & format control support

## Tab

DX9: tab size = 8, em space on
DX11: tab size = 4, em space off

## Format Control

### Escape

`\[` -> `[`

`[Command]` if not match follows, will be displayed as untouched depend on your flag settings

### Ignore

`[^]` ignore all formats after this

### Ignore non Icon

`[^-]` ignore all formats except `[ICon]` after this. icon controls like `[IConOffsetX]` are also ignored

### Stop Ignore

`[!^]` stop ignore all formats

### Reset

`[!]` reset all format to what you set in properties and events

### Remark

#### Insert

`[Remark = CharCount, Content]` insert remark  that display over texts to the following characters
As text may include ',', parse is started from left, unlike other formats. Remark is rendered in another NeoStr class, with the same config (color, font, etc), but overrode position & render object, and the font size is half of the parent object when it starts rendering.

- *if you use content, make sure it doesn't have open `[]`*
- *if remarked string changed line, then the render of its remark is skipped*
- *if the font / size changed too drastically there will be overlap*
- *position of remark is estimated by the mean size of remarked characters*

#### Format

`[RemarkOffsetX = 0.0][/RemarkOffsetX]` Remark Offset X

- ! = reset to default
- +/- = add/minus to current

`[RemarkOffsetY = 0.0][/RemarkOffsetY]` Remark Offset Y

- ! = reset to default
- +/- = add/minus to current

### ICon

#### Insert

`[ICon = Direction, Frame]` insert icon based on linked active. if param is less than two, will be referred from left. e.g., one param will be treated as frame.

ICon will be resized to the font size it inserted at.
*if you are linking to object you need to retrieve each param then handle it by your self you need to set a key for it for retrieving and value as pointer to a surface*

#### Format

`[IConOffsetX = 0.0][/IConOffsetX]` ICon Offset X

- ! = reset to default
- +/- = add/minus to current

`[IConOffsetY = 0.0][/IConOffsetY]` ICon Offset Y

- ! = reset to default
- +/- = add/minus to current

`[IConScale = 1.0][/IConScale]` ICon Scale

- ! = reset to default
- +/- = add/minus to current

`[IConResample = 1][/IConResample]` ICon Resample, 1 = Enable, 0 = Disable

- ! = reset to default

### Shake

`[Shake = Type, Amplitude, TimerCoef, CharOffset]`control shake.

if param is less than four, will be referred from right.
e.g., one param -> Type, two params -> Type & Amplitude, etc.

- type: shake type, default is None, can be X/Y/Random. X/Y is based on cosine.
- amplitude: default is 1.0, relative to character size. size = 30, amplitude = 0.5, will shake in range of [ -15 ~ 15 ]
- timer coef: default is 1.0, shake faster when higher.
- char offset: default is 1.0 / 6.0, relative to 360 degree. charOffset = 0.2, actual offset is 72 degree. determines the movement interval of adjacent characters.only works in X/Y mode ( cosine )

### Color

`[Color = #FFFFFFFF][/Color]`&`[C = #FFFFFFFF][/C]`	color, hex AARRGGBB, or A, R, G, B

- ! = reset to default
- +/- = add/minus to current

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

- ! = reset to default
- +/- = add/minus to current

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

This object has no properties

## Action

## Condition

## Expression

- Get Non Command Offset
  - if next char is a command, this expression will return the offset to command end