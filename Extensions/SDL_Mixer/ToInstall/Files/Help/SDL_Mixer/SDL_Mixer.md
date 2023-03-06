# Template

## Description

Audio Extension based on [SDL_Mixer](https://github.com/libsdl-org/SDL_mixer) & [SDL_Mixer_X](https://github.com/WohlSoft/SDL-Mixer-X), with encryption support

for more information, please check the [Help File](https://github.com/defisym/OpenFusionExamples/blob/master/Extensions/SDL_MIXER/ToInstall/Files/Help/SDL_MIXER/SDL_MIXER.md)

This object has two types of channels, exclusive & mixing.

Only one music can be played at one time in exclusive ones, just like fusion channels. You can manage the audios in exclusive channel the same as you do with fusion.

Mixing channels won't provide some manage features like set position of exclusive ones, you should use this type to play short beeps, works like search for a spare channel, update channel volume, then play it in fusion.

Actually the channel is virtual, this object only uses the music stream of SDL_Mixer_X, managed internally and provide channels for fusion A/C/Es

## Properties

This object has no properties

## Action

## Condition

- Iterate
  - On Iterate Sections
  - On Iterate Items

## Expression
