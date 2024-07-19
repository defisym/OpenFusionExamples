# Template

## Description

Audio Extension based on [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) & [SDL_Mixer_X](https://github.com/WohlSoft/SDL-Mixer-X), with [encryption](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files) support

Audio Effect powered by [SoundTouch](http://www.surina.net/soundtouch/index.html)

This object has two types of channels, exclusive & mixing.

Only one music can be played at one time in exclusive ones, just like fusion channels. You can manage the audios in exclusive channel the same as you do with fusion.

Mixing channels won't provide some manage features like set position of exclusive ones, you should use this type to play short beeps, works like search for a spare channel, update channel volume, then play it in fusion. You can also set score for mixing channels, which will change the semi-tones to make the same sample sounds less noisy.

Actually the channel is virtual, this object only uses the music stream of SDL_Mixer_X, managed internally and provide channels for fusion A/C/Es

**NOTE:** you need to check `Run when minimized` & `Run while resizing`, or audio will be paused due to main thread is paused

## Properties

This object has no properties

## Action

- Play Exclusive
- Play Mixing
  - *you can use `Get Play From Memory Name` & `Get Play From Handled Memory Name` as filename here and keep key empty, to play from memory*
  - *when playing from memory, you should not release the memory it referenced, as it's not copied for sake of performance, or app may crash*

- Set Master Volume
- Set Channel Volume

- Set Exclusive Position
- Set Exclusive ABLoop
- Set Mixing Channel Score
  - `{ 1, 2, 3, 4, 5 }`

- Stop Specific Channel
- Stop All Channel
- Pause Specific Channel
- Resume Specific Channel

- Binary
  - Load Binary
    - *load file as binary*
    - *you can check if load success by checking `Get Binary Address` equals to `nullptr`, aka `0`*
  - Release Binary
    - *if binary is referenced, it won't be released*
  - Update Binary
    - *if binary is referenced or hash is the same, it won't be updated*

- Effect
  - Set Effect Panning
  - Set Effect Position
  - Set Effect Distance

## Condition

- Channel Playing
- No Channel Playing

- Channel Paused
- All Channel Paused

- Channel Has No Output
  - *not playing or is paused*

- Exclusive Channel Fading Complete

- Binary Has No Reference
  - *binary file currently is not referenced, and can safely be released or updated*

## Expression

- Get Master Volume
- Get Channel Volume

- Get Channel ID By Name
  - *Get channel ID by audio name it's playing*
  - *for mixing channel, will return channel that includes the name*
- Get Channel State

- Get Exclusive Channel Position
- Get Exclusive Channel Duration
- Get Exclusive Channel Name By ID
  - *Get audio name it's playing by channel ID*

- Binary
  - Get Play From Memory Name
    - *access file name is the one used for search internally*
    - *return a serialized string to play from memory*
    - *the validity of memory address won't be checked, as it's handled by you*
  - Get Play From Handled Memory Name
    - *access file name is the one used for search internally*
    - *return a serialized string to play from memory*
    - *from memory that handled by this extension, you need to load & free by binary actions*
  - Get Binary Address
    - *get address from handled memory*

- Effect
  - Get Effect Panning Left
  - Get Effect Panning Right
  - Get Effect Angle
  - Get Effect Distance
