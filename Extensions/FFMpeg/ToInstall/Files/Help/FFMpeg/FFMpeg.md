# FFMpeg

## Description

FFMpeg object for fusion.

Talking about the ADS...Tired about the uncontrollable gap before playing? Don't want the micro-stuttering when VSync is on? Want to play other formats like mp4? Need to protect your precious cut scenes? Then try this object!

Special thanks to @SelfReflection31 (From Github) and @Ruthoranium (From Discord), who help a lot to polishing this extension.

This object is based on cross platform lib, FFMpeg & SDL, in theory there is not too difficult to bring it to other platforms like Android.

**NOTE:** this object may conflict with other objects that need SDL, due to the dll version, and the possible double initialize (SDL needs to be initialized before using, and will fail if it's already initialized). This extension uses a custom version of SDL_Mixer_X to detect if initialized or not, to compatible with other extensions in this repo, e.g., SDL_Mixer

**NOTE:** this object uses a bitmap surface, as fusion didn't provide methods to write a HWA texture directly, and convert it will cost more than 3ms.

**NOTE:** this object uses SDL_Mixer for multi-instance. Also because of the HWA issue mentioned above, too many objects will take a long time to blit.

**NOTE:** this object runs synchronously, it will try decode video frame in `HandleRunObject` routine called once per frame. If your game is heavy then it will skip more frames. Audio decode is done in another thread.

**NOTE:** you need to check `Run when minimized` & `Run while resizing`, or audio will be paused due to main thread is paused

**NOTE:** this object requires FFMpeg & SDL

FFMpeg: you need to download release (e.g., `ffmpeg-n7.1-latest-win32-lgpl-shared-7.1.zip` ) from <https://github.com/defisym/FFmpeg-Builds-Win32/releases>, then put dlls into Fusion root & `Runtime/Unicode`, or you need to build X86 version from FFMpeg source code, as the official 32 bit support has stopped.

SDL: you need to download `SDL2-2.24.0-win32-x86.zip` from <https://github.com/libsdl-org/SDL/releases>, then put X86 dlls into Fusion root & `Runtime/Unicode`

SDL Mixer X: Upstream is <https://github.com/WohlSoft/SDL-Mixer-X>, this extension added a few custom functions to original lib, so please clone [this repo](https://github.com/defisym/SDL-Mixer-X) first, then follow the [guide](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/index.md#general-build-on-unix-like-platform-and-install-into-the-system) to build it by yourself

Here follows all dlls needed by this object:

- FFMpeg
  - avcodec-61.dll
  - avdevice-61.dll
  - avfilter-10.dll
  - avformat-61.dll
  - avutil-59.dll
  - postproc-58.dll
  - swresample-5.dll
  - swscale-8.dll

- SDL
  - SDL2.dll
  - SDL2_mixer_ext.dll

**NOTE:** this object requires `Microsoft Visual C++ Redistributable 2015+` / `MSVCP14x.dll` & `VCRUNTIME14x.dll`

## Properties

- Options
  - Stretch
    - *Stretch to fit object size*
  - Play after load
    - *Auto start play when a file is loaded*
  - Loop
    - *Loop video*
    - *When playing finishes, extension will seek back to time stamp 0, and may need some time to rebuild codec context and restart playing*
  - Accurate seek
    - *To avoid display issues usually we should seek to a keyframe, but it may not that accurate. If enable this option, after seeking extension will decode frames until reaches the given time stamp, and will be time-consuming*
    - *To make this work, make sure you use seek with flag `AVSEEK_FLAG_BACKWARD`. Seek action will use this flag as default so it's transparent to users, but I still note it here as we have an action which allows you to set seek flags...*
  - Cache
    - Cache decrypted data over frames
    - Currently reading decrypt file didn't use a stream (it's provided by another class and FFMpeg only has a memory reader), so it may cost some time to decrypt. With this on extension will cache these file in memory over frames instead of reloading
  - Force No Audio
    - *Don't play audio and sync with external clock*
  - Thread Count
    - *Thread used for decoding, more thread will be faster but use more and memory*
    - *0 for auto detect, but the actual thread used won't greater than the number of CPU cores.*

- Hardware Decode
  - Shared Hardware Device
    - *When decoding, FFMpeg will create a hardware device, and this can be shared by all instances to reduce memory usage and optimize the open speed*
  - Copy To Texture
    - *do not copy hardware frame to memory from GPU, then copy to fusion bitmap surface, instead, use the hardware frame directly*
    - *Only supported in D3D11 mode, when check this, Hardware Device will be set to D3D11VA. In this case, extension will utilize D3D11 pipeline to render the texture directly to fusion HWA surface, which can significantly boost the performance*
  - Hardware Device
    - *set the device you want object to use. object will try using the device you set, if not supported, it will try using other hardware devices. if all devices are not supported, it will fallback to software decode*
    - *due to fusion limitation, by default hardware frame needs to be copied to memory from GPU, then copy to fusion surface, so usually it's slower comparing to software decode if you are using modern hardwares*

## Action

- Reset Display
  - *reset to display nothing if video is not playing (paused or closed), auto recover if continue*
  - *it's done by skipping the display routine, if you get surface after this you'll still get a frame instead of blank——previously this is achieved by set alpha channel to transparent, while when copy to texture is enabled, the display surface is a render target texture, which do can have alpha channel but ignored when display*

- Open Video
  - *If open failed, extension will keep previous frame*
  - *If filename is an URL, e.g., [Apple iPhone X](https://www.apple.com/105/media/us/iphone-x/2017/01df5b43-28e4-4848-bf20-490c34a926a7/films/feature/iphone-x-feature-tpl-cc-us-20170912_1920x1080h.mp4), the key will be ignored. Note that player may freeze if play over a bad network connection. This lag can be reduced by moving queue to another thread, which is not implemented*
  - *If filename and key can be convert to number and it's non-zero, then filename will be treat to the data address, key is size of data. Note that extension will not check if the address is valid, and you should not release it before you close the video.*
  - *If key is not empty, extension will treat it as an encrypted file. Decode a normal file in encrypted way usually will crash. You can encrypt files by [Encrypter](https://github.com/defisym/OpenFusionExamples/tree/master/Extensions/Encrypter/ToInstall/Files). Encrypt & decrypt are based on AES so without your key no one can get your file, unless they dump it from memory. Play encrypted videos may cost more memory as decrypt stream is not implemented*
- Open Video To
  - *Open video to a given timestamp*

- Close Video

- Play Video
- Pause Video

- Set Volume
  - *volume is kept when opening another video*
- Set Loop
  - *loop state is kept when opening another video*
- Set Force No Audio
  - *see props*
- Set Audio Tempo
  - *tempo = 2.0 -> play video in 2X speed*
  - *also works if sync with external clock (aka no audio)*
- Set Accurate Seek
  - see Properties->Accurate Seek
- Set Video Position
  - *seek to the given time, in ms*
- Set Video Position With Flag
  - *similar as above, but can let you set seek flag*  
  - *depend on the flag you use, first param is used in different ways*

  ```C
    #define AVSEEK_FLAG_BACKWARD 1 ///< seek backward
    #define AVSEEK_FLAG_BYTE     2 ///< seeking based on position in bytes
    #define AVSEEK_FLAG_ANY      4 ///< seek to any frame, even non-keyframes
    #define AVSEEK_FLAG_FRAME    8 ///< seeking based on frame number
  ```

  - *except `FFMpeg` flags, there are other flag you can use. by default, it will decode to first valid frame (accurate seek enabled & not `AVSEEK_FLAG_BYTE`), and revert back if target timestamp is 0*

  ```C++
    constexpr auto SeekFlag_NoGoto = 0x0001 << 16;    // do not decode to next valid frame
    constexpr auto SeekFlag_NoRevert = 0x0002 << 16;  // do not revert if timestamp is 0
  ```

- Set Size
  - *set object size*

- Set Thread Count
  - *see Properties->Thread Count*
- Set Override Codec
  - *Force to use given codec. E.g., use `libvpx-vp9` to enable `WebM` alpha support*
  - *if given codec is invalid or cannot open video. extension will reset to default then try again. If the error still exists, `On Video Open Failed` will be triggered*
  - *supported codecs: see Appendix*
- Set Hardware Decode Device
  - *if not D3D11VA, Copy To Texture will be set to false*
  - *see Properties->Hardware Decode Device and the table below*

| Device Name  | AVHWDeviceType                |
| ------------ | ----------------------------- |
| NONE         | AV_HWDEVICE_TYPE_NONE         |
| VDPAU        | AV_HWDEVICE_TYPE_VDPAU        |
| CUDA         | AV_HWDEVICE_TYPE_CUDA         |
| VAAPI        | AV_HWDEVICE_TYPE_VAAPI        |
| DXVA2        | AV_HWDEVICE_TYPE_DXVA2        |
| QSV          | AV_HWDEVICE_TYPE_QSV          |
| VIDEOTOOLBOX | AV_HWDEVICE_TYPE_VIDEOTOOLBOX |
| D3D11VA      | AV_HWDEVICE_TYPE_D3D11VA      |
| DRM          | AV_HWDEVICE_TYPE_DRM          |
| OPENCL       | AV_HWDEVICE_TYPE_OPENCL       |
| MEDIACODEC   | AV_HWDEVICE_TYPE_MEDIACODEC   |
| VULKAN       | AV_HWDEVICE_TYPE_VULKAN       |
| D3D12VA      | AV_HWDEVICE_TYPE_D3D12VA      |

- Set Copy To Texture
  - *if true, Set Hardware Decode Device will be set to D3D11VA*
  - *see Properties->Copy To Texture*

- Cache Video
  - *decrypt given file then cache it in memory*
- Erase Cached Video
  - *erase cached given file*
  - *the file is using now won't be erased*
  - *input an empty file name will erase all cached file*

## Condition

- Video Open
- Video Play
- Video Loop
- Video Finish

- Video Hardware Decode
  - *object is using hardware decode*
- Wanted Hardware Decode
  - *you set object to use hardware decode*

- On Video Finish
  - *immediate event, triggered when video finish playing.*
- On Video Open Failed
  - *immediate event, triggered when video failed to open.*

## Expression

- Get Video Name
- Get Video Position
- Get Video Duration

- Get Volume
- Get Audio Tempo

- Get Video Open State
- Get Video Play State
- Get Video Loop State
- Get Video Finish State

- Get Current Video Frame Pointer
- Get Grabbed Video Frame Pointer
  - *You can get the cSurface pointer of current frame / frame with given time stamp. Pass it to another extension then do anything you like.*
  - *Note the target extension should build in /MD, aka use the same heap. Or it may crash due to access violation. See: <https://learn.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-170>*

- Get Thread Count
- Get Video Override Codec Name
- Get Audio Override Codec Name

- Get Hardware Decode State
- Get Actual Hardware Device
  - *get the device object is using*
- Get Wanted Hardware Device
  - *get the device you set*

## Appendix

### Supported Codecs

listed by `ffmpeg -codecs`

#### Video Codec

| Codec           | Info                                                                                                                                                               |
| --------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 012v            | Uncompressed 4:2:2 10-bit                                                                                                                                          |
| 4xm             | 4X Movie                                                                                                                                                           |
| 8bps            | QuickTime 8BPS video                                                                                                                                               |
| aasc            | Autodesk RLE                                                                                                                                                       |
| agm             | Amuse Graphics Movie                                                                                                                                               |
| aic             | Apple Intermediate Codec                                                                                                                                           |
| alias_pix       | Alias/Wavefront PIX image                                                                                                                                          |
| amv             | AMV Video                                                                                                                                                          |
| anm             | Deluxe Paint Animation                                                                                                                                             |
| ansi            | ASCII/ANSI art                                                                                                                                                     |
| apng            | APNG (Animated Portable Network Graphics) image                                                                                                                    |
| arbc            | Gryphon's Anim Compressor                                                                                                                                          |
| argo            | Argonaut Games Video                                                                                                                                               |
| asv1            | ASUS V1                                                                                                                                                            |
| asv2            | ASUS V2                                                                                                                                                            |
| aura            | Auravision AURA                                                                                                                                                    |
| aura2           | Auravision Aura 2                                                                                                                                                  |
| av1             | Alliance for Open Media AV1 (decoders: libdav1d libaom-av1 av1 av1_cuvid av1_qsv) (encoders: libaom-av1 av1_nvenc av1_qsv av1_amf av1_vaapi)                       |
| avrn            | Avid AVI Codec                                                                                                                                                     |
| avrp            | Avid 1:1 10-bit RGB Packer                                                                                                                                         |
| avs             | AVS (Audio Video Standard) video                                                                                                                                   |
| avui            | Avid Meridien Uncompressed                                                                                                                                         |
| bethsoftvid     | Bethesda VID video                                                                                                                                                 |
| bfi             | Brute Force & Ignorance                                                                                                                                            |
| binkvideo       | Bink video                                                                                                                                                         |
| bintext         | Binary text                                                                                                                                                        |
| bitpacked       | Bitpacked                                                                                                                                                          |
| bmp             | BMP (Windows and OS/2 bitmap)                                                                                                                                      |
| bmv_video       | Discworld II BMV video                                                                                                                                             |
| brender_pix     | BRender PIX image                                                                                                                                                  |
| c93             | Interplay C93                                                                                                                                                      |
| cavs            | Chinese AVS (Audio Video Standard) (AVS1-P2, JiZhun profile)                                                                                                       |
| cdgraphics      | CD Graphics video                                                                                                                                                  |
| cdtoons         | CDToons video                                                                                                                                                      |
| cdxl            | Commodore CDXL video                                                                                                                                               |
| cfhd            | GoPro CineForm HD                                                                                                                                                  |
| cinepak         | Cinepak                                                                                                                                                            |
| clearvideo      | Iterated Systems ClearVideo                                                                                                                                        |
| cljr            | Cirrus Logic AccuPak                                                                                                                                               |
| cllc            | Canopus Lossless Codec                                                                                                                                             |
| cmv             | Electronic Arts CMV video (decoders: eacmv)                                                                                                                        |
| cpia            | CPiA video format                                                                                                                                                  |
| cri             | Cintel RAW                                                                                                                                                         |
| cscd            | CamStudio (decoders: camstudio)                                                                                                                                    |
| cyuv            | Creative YUV (CYUV)                                                                                                                                                |
| dds             | DirectDraw Surface image decoder                                                                                                                                   |
| dfa             | Chronomaster DFA                                                                                                                                                   |
| dirac           | Dirac (encoders: vc2)                                                                                                                                              |
| dnxhd           | VC3/DNxHD                                                                                                                                                          |
| dpx             | DPX (Digital Picture Exchange) image                                                                                                                               |
| dsicinvideo     | Delphine Software International CIN video                                                                                                                          |
| dvvideo         | DV (Digital Video)                                                                                                                                                 |
| dxa             | Feeble Files/ScummVM DXA                                                                                                                                           |
| dxtory          | Dxtory                                                                                                                                                             |
| dxv             | Resolume DXV                                                                                                                                                       |
| escape124       | Escape 124                                                                                                                                                         |
| escape130       | Escape 130                                                                                                                                                         |
| exr             | OpenEXR image                                                                                                                                                      |
| ffv1            | FFmpeg video codec #1                                                                                                                                              |
| ffvhuff         | Huffyuv FFmpeg variant                                                                                                                                             |
| fic             | Mirillis FIC                                                                                                                                                       |
| fits            | FITS (Flexible Image Transport System)                                                                                                                             |
| flashsv         | Flash Screen Video v1                                                                                                                                              |
| flashsv2        | Flash Screen Video v2                                                                                                                                              |
| flic            | Autodesk Animator Flic video                                                                                                                                       |
| flv1            | FLV / Sorenson Spark / Sorenson H.263 (Flash Video) (decoders: flv) (encoders: flv)                                                                                |
| fmvc            | FM Screen Capture Codec                                                                                                                                            |
| fraps           | Fraps                                                                                                                                                              |
| frwu            | Forward Uncompressed                                                                                                                                               |
| g2m             | Go2Meeting                                                                                                                                                         |
| gdv             | Gremlin Digital Video                                                                                                                                              |
| gem             | GEM Raster image                                                                                                                                                   |
| gif             | CompuServe GIF (Graphics Interchange Format)                                                                                                                       |
| h261            | H.261                                                                                                                                                              |
| h263            | H.263 / H.263-1996, H.263+ / H.263-1998 / H.263 version 2                                                                                                          |
| h263i           | Intel H.263                                                                                                                                                        |
| h263p           | H.263+ / H.263-1998 / H.263 version 2                                                                                                                              |
| h264            | H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10 (decoders: h264 h264_qsv libopenh264 h264_cuvid) (encoders: libopenh264 h264_amf h264_mf h264_nvenc h264_qsv h264_vaapi) |
| hap             | Vidvox Hap                                                                                                                                                         |
| hdr             | HDR (Radiance RGBE format) image                                                                                                                                   |
| hevc            | H.265 / HEVC (High Efficiency Video Coding) (decoders: hevc hevc_qsv hevc_cuvid) (encoders: hevc_amf hevc_mf hevc_nvenc hevc_qsv hevc_vaapi libkvazaar)            |
| hnm4video       | HNM 4 video                                                                                                                                                        |
| hq_hqa          | Canopus HQ/HQA                                                                                                                                                     |
| hqx             | Canopus HQX                                                                                                                                                        |
| huffyuv         | HuffYUV                                                                                                                                                            |
| hymt            | HuffYUV MT                                                                                                                                                         |
| idcin           | id Quake II CIN video (decoders: idcinvideo)                                                                                                                       |
| idf             | iCEDraw text                                                                                                                                                       |
| iff_ilbm        | IFF ACBM/ANIM/DEEP/ILBM/PBM/RGB8/RGBN (decoders: iff)                                                                                                              |
| imm4            | Infinity IMM4                                                                                                                                                      |
| imm5            | Infinity IMM5                                                                                                                                                      |
| indeo2          | Intel Indeo 2                                                                                                                                                      |
| indeo3          | Intel Indeo 3                                                                                                                                                      |
| indeo4          | Intel Indeo Video Interactive 4                                                                                                                                    |
| indeo5          | Intel Indeo Video Interactive 5                                                                                                                                    |
| interplayvideo  | Interplay MVE video                                                                                                                                                |
| ipu             | IPU Video                                                                                                                                                          |
| jpeg2000        | JPEG 2000 (encoders: jpeg2000 libopenjpeg)                                                                                                                         |
| jpegls          | JPEG-LS                                                                                                                                                            |
| jpegxl          | JPEG XL (decoders: libjxl) (encoders: libjxl)                                                                                                                      |
| jv              | Bitmap Brothers JV video                                                                                                                                           |
| kgv1            | Kega Game Video                                                                                                                                                    |
| kmvc            | Karl Morton's video codec                                                                                                                                          |
| lagarith        | Lagarith lossless                                                                                                                                                  |
| lead            | LEAD MCMP                                                                                                                                                          |
| loco            | LOCO                                                                                                                                                               |
| lscr            | LEAD Screen Capture                                                                                                                                                |
| m101            | Matrox Uncompressed SD                                                                                                                                             |
| mad             | Electronic Arts Madcow Video (decoders: eamad)                                                                                                                     |
| magicyuv        | MagicYUV video                                                                                                                                                     |
| mdec            | Sony PlayStation MDEC (Motion DECoder)                                                                                                                             |
| media100        | Media 100i                                                                                                                                                         |
| mimic           | Mimic                                                                                                                                                              |
| mjpeg           | Motion JPEG (decoders: mjpeg mjpeg_cuvid mjpeg_qsv) (encoders: mjpeg mjpeg_qsv mjpeg_vaapi)                                                                        |
| mjpegb          | Apple MJPEG-B                                                                                                                                                      |
| mmvideo         | American Laser Games MM Video                                                                                                                                      |
| mobiclip        | MobiClip Video                                                                                                                                                     |
| motionpixels    | Motion Pixels video                                                                                                                                                |
| mpeg1video      | MPEG-1 video (decoders: mpeg1video mpeg1_cuvid)                                                                                                                    |
| mpeg2video      | MPEG-2 video (decoders: mpeg2video mpegvideo mpeg2_qsv mpeg2_cuvid) (encoders: mpeg2video mpeg2_qsv mpeg2_vaapi)                                                   |
| mpeg4           | MPEG-4 part 2 (decoders: mpeg4 mpeg4_cuvid)                                                                                                                        |
| msa1            | MS ATC Screen                                                                                                                                                      |
| mscc            | Mandsoft Screen Capture Codec                                                                                                                                      |
| msmpeg4v1       | MPEG-4 part 2 Microsoft variant version 1                                                                                                                          |
| msmpeg4v2       | MPEG-4 part 2 Microsoft variant version 2                                                                                                                          |
| msmpeg4v3       | MPEG-4 part 2 Microsoft variant version 3 (decoders: msmpeg4) (encoders: msmpeg4)                                                                                  |
| msp2            | Microsoft Paint (MSP) version 2                                                                                                                                    |
| msrle           | Microsoft RLE                                                                                                                                                      |
| mss1            | MS Screen 1                                                                                                                                                        |
| mss2            | MS Windows Media Video V9 Screen                                                                                                                                   |
| msvideo1        | Microsoft Video 1                                                                                                                                                  |
| mszh            | LCL (LossLess Codec Library) MSZH                                                                                                                                  |
| mts2            | MS Expression Encoder Screen                                                                                                                                       |
| mv30            | MidiVid 3.0                                                                                                                                                        |
| mvc1            | Silicon Graphics Motion Video Compressor 1                                                                                                                         |
| mvc2            | Silicon Graphics Motion Video Compressor 2                                                                                                                         |
| mvdv            | MidiVid VQ                                                                                                                                                         |
| mvha            | MidiVid Archive Codec                                                                                                                                              |
| mwsc            | MatchWare Screen Capture Codec                                                                                                                                     |
| mxpeg           | Mobotix MxPEG video                                                                                                                                                |
| notchlc         | NotchLC                                                                                                                                                            |
| nuv             | NuppelVideo/RTJPEG                                                                                                                                                 |
| paf_video       | Amazing Studio Packed Animation File Video                                                                                                                         |
| pam             | PAM (Portable AnyMap) image                                                                                                                                        |
| pbm             | PBM (Portable BitMap) image                                                                                                                                        |
| pcx             | PC Paintbrush PCX image                                                                                                                                            |
| pdv             | PDV (PlayDate Video)                                                                                                                                               |
| pfm             | PFM (Portable FloatMap) image                                                                                                                                      |
| pgm             | PGM (Portable GrayMap) image                                                                                                                                       |
| pgmyuv          | PGMYUV (Portable GrayMap YUV) image                                                                                                                                |
| pgx             | PGX (JPEG2000 Test Format)                                                                                                                                         |
| phm             | PHM (Portable HalfFloatMap) image                                                                                                                                  |
| photocd         | Kodak Photo CD                                                                                                                                                     |
| pictor          | Pictor/PC Paint                                                                                                                                                    |
| pixlet          | Apple Pixlet                                                                                                                                                       |
| png             | PNG (Portable Network Graphics) image                                                                                                                              |
| ppm             | PPM (Portable PixelMap) image                                                                                                                                      |
| prores          | Apple ProRes (iCodec Pro) (encoders: prores prores_aw prores_ks)                                                                                                   |
| prosumer        | Brooktree ProSumer Video                                                                                                                                           |
| psd             | Photoshop PSD file                                                                                                                                                 |
| ptx             | V.Flash PTX image                                                                                                                                                  |
| qdraw           | Apple QuickDraw                                                                                                                                                    |
| qoi             | QOI (Quite OK Image)                                                                                                                                               |
| qpeg            | Q-team QPEG                                                                                                                                                        |
| qtrle           | QuickTime Animation (RLE) video                                                                                                                                    |
| r10k            | AJA Kona 10-bit RGB Codec                                                                                                                                          |
| r210            | Uncompressed RGB 10-bit                                                                                                                                            |
| rasc            | RemotelyAnywhere Screen Capture                                                                                                                                    |
| rawvideo        | raw video                                                                                                                                                          |
| rl2             | RL2 video                                                                                                                                                          |
| roq             | id RoQ video (decoders: roqvideo) (encoders: roqvideo)                                                                                                             |
| rpza            | QuickTime video (RPZA)                                                                                                                                             |
| rscc            | innoHeim/Rsupport Screen Capture Codec                                                                                                                             |
| rtv1            | RTV1 (RivaTuner Video)                                                                                                                                             |
| rv10            | RealVideo 1.0                                                                                                                                                      |
| rv20            | RealVideo 2.0                                                                                                                                                      |
| rv30            | RealVideo 3.0                                                                                                                                                      |
| rv40            | RealVideo 4.0                                                                                                                                                      |
| sanm            | LucasArts SANM/SMUSH video                                                                                                                                         |
| scpr            | ScreenPressor                                                                                                                                                      |
| screenpresso    | Screenpresso                                                                                                                                                       |
| sga             | Digital Pictures SGA Video                                                                                                                                         |
| sgi             | SGI image                                                                                                                                                          |
| sgirle          | SGI RLE 8-bit                                                                                                                                                      |
| sheervideo      | BitJazz SheerVideo                                                                                                                                                 |
| simbiosis_imx   | Simbiosis Interactive IMX Video                                                                                                                                    |
| smackvideo      | Smacker video (decoders: smackvid)                                                                                                                                 |
| smc             | QuickTime Graphics (SMC)                                                                                                                                           |
| smvjpeg         | Sigmatel Motion Video                                                                                                                                              |
| snow            | Snow                                                                                                                                                               |
| sp5x            | Sunplus JPEG (SP5X)                                                                                                                                                |
| speedhq         | NewTek SpeedHQ                                                                                                                                                     |
| srgc            | Screen Recorder Gold Codec                                                                                                                                         |
| sunrast         | Sun Rasterfile image                                                                                                                                               |
| svq1            | Sorenson Vector Quantizer 1 / Sorenson Video 1 / SVQ1                                                                                                              |
| svq3            | Sorenson Vector Quantizer 3 / Sorenson Video 3 / SVQ3                                                                                                              |
| targa           | Truevision Targa image                                                                                                                                             |
| targa_y216      | Pinnacle TARGA CineWave YUV16                                                                                                                                      |
| tdsc            | TDSC                                                                                                                                                               |
| tgq             | Electronic Arts TGQ video (decoders: eatgq)                                                                                                                        |
| tgv             | Electronic Arts TGV video (decoders: eatgv)                                                                                                                        |
| theora          | Theora (encoders: libtheora)                                                                                                                                       |
| thp             | Nintendo Gamecube THP video                                                                                                                                        |
| tiertexseqvideo | Tiertex Limited SEQ video                                                                                                                                          |
| tiff            | TIFF image                                                                                                                                                         |
| tmv             | 8088flex TMV                                                                                                                                                       |
| tqi             | Electronic Arts TQI video (decoders: eatqi)                                                                                                                        |
| truemotion1     | Duck TrueMotion 1.0                                                                                                                                                |
| truemotion2     | Duck TrueMotion 2.0                                                                                                                                                |
| truemotion2rt   | Duck TrueMotion 2.0 Real Time                                                                                                                                      |
| tscc            | TechSmith Screen Capture Codec (decoders: camtasia)                                                                                                                |
| tscc2           | TechSmith Screen Codec 2                                                                                                                                           |
| txd             | Renderware TXD (TeXture Dictionary) image                                                                                                                          |
| ulti            | IBM UltiMotion (decoders: ultimotion)                                                                                                                              |
| utvideo         | Ut Video                                                                                                                                                           |
| v210            | Uncompressed 4:2:2 10-bit                                                                                                                                          |
| v210x           | Uncompressed 4:2:2 10-bit                                                                                                                                          |
| v308            | Uncompressed packed 4:4:4                                                                                                                                          |
| v408            | Uncompressed packed QT 4:4:4:4                                                                                                                                     |
| v410            | Uncompressed 4:4:4 10-bit                                                                                                                                          |
| vb              | Beam Software VB                                                                                                                                                   |
| vble            | VBLE Lossless Codec                                                                                                                                                |
| vbn             | Vizrt Binary Image                                                                                                                                                 |
| vc1             | SMPTE VC-1 (decoders: vc1 vc1_qsv vc1_cuvid)                                                                                                                       |
| vc1image        | Windows Media Video 9 Image v2                                                                                                                                     |
| vcr1            | ATI VCR1                                                                                                                                                           |
| vixl            | Miro VideoXL (decoders: xl)                                                                                                                                        |
| vmdvideo        | Sierra VMD video                                                                                                                                                   |
| vmix            | vMix Video                                                                                                                                                         |
| vmnc            | VMware Screen Codec / VMware Video                                                                                                                                 |
| vnull           | Null video codec                                                                                                                                                   |
| vp3             | On2 VP3                                                                                                                                                            |
| vp4             | On2 VP4                                                                                                                                                            |
| vp5             | On2 VP5                                                                                                                                                            |
| vp6             | On2 VP6                                                                                                                                                            |
| vp6a            | On2 VP6 (Flash version, with alpha channel)                                                                                                                        |
| vp6f            | On2 VP6 (Flash version)                                                                                                                                            |
| vp7             | On2 VP7                                                                                                                                                            |
| vp8             | On2 VP8 (decoders: vp8 libvpx vp8_cuvid vp8_qsv) (encoders: libvpx vp8_vaapi)                                                                                      |
| vp9             | Google VP9 (decoders: vp9 libvpx-vp9 vp9_cuvid vp9_qsv) (encoders: libvpx-vp9 vp9_vaapi vp9_qsv)                                                                   |
| vqc             | ViewQuest VQC                                                                                                                                                      |
| vvc             | H.266 / VVC (Versatile Video Coding)                                                                                                                               |
| wbmp            | WBMP (Wireless Application Protocol Bitmap) image                                                                                                                  |
| wcmv            | WinCAM Motion Video                                                                                                                                                |
| webp            | WebP (encoders: libwebp_anim libwebp)                                                                                                                              |
| wmv1            | Windows Media Video 7                                                                                                                                              |
| wmv2            | Windows Media Video 8                                                                                                                                              |
| wmv3            | Windows Media Video 9                                                                                                                                              |
| wmv3image       | Windows Media Video 9 Image                                                                                                                                        |
| wnv1            | Winnov WNV1                                                                                                                                                        |
| wrapped_avframe | AVFrame to AVPacket passthrough                                                                                                                                    |
| ws_vqa          | Westwood Studios VQA (Vector Quantized Animation) video (decoders: vqavideo)                                                                                       |
| xan_wc3         | Wing Commander III / Xan                                                                                                                                           |
| xan_wc4         | Wing Commander IV / Xxan                                                                                                                                           |
| xbin            | eXtended BINary text                                                                                                                                               |
| xbm             | XBM (X BitMap) image                                                                                                                                               |
| xface           | X-face image                                                                                                                                                       |
| xpm             | XPM (X PixMap) image                                                                                                                                               |
| xwd             | XWD (X Window Dump) image                                                                                                                                          |
| y41p            | Uncompressed YUV 4:1:1 12-bit                                                                                                                                      |
| ylc             | YUY2 Lossless Codec                                                                                                                                                |
| yop             | Psygnosis YOP Video                                                                                                                                                |
| yuv4            | Uncompressed packed 4:2:0                                                                                                                                          |
| zerocodec       | ZeroCodec Lossless Video                                                                                                                                           |
| zlib            | LCL (LossLess Codec Library) ZLIB                                                                                                                                  |
| zmbv            | Zip Motion Blocks Video                                                                                                                                            |

#### Audio Codec

| Codec             | Info                                                                                                      |
| ----------------- | --------------------------------------------------------------------------------------------------------- |
| 8svx_exp          | 8SVX exponential                                                                                          |
| 8svx_fib          | 8SVX fibonacci                                                                                            |
| aac               | AAC (Advanced Audio Coding) (decoders: aac aac_fixed) (encoders: aac aac_mf)                              |
| aac_latm          | AAC LATM (Advanced Audio Coding LATM syntax)                                                              |
| ac3               | ATSC A/52A (AC-3) (decoders: ac3 ac3_fixed) (encoders: ac3 ac3_fixed ac3_mf)                              |
| acelp.kelvin      | Sipro ACELP.KELVIN                                                                                        |
| adpcm_4xm         | ADPCM 4X Movie                                                                                            |
| adpcm_adx         | SEGA CRI ADX ADPCM                                                                                        |
| adpcm_afc         | ADPCM Nintendo Gamecube AFC                                                                               |
| adpcm_agm         | ADPCM AmuseGraphics Movie AGM                                                                             |
| adpcm_aica        | ADPCM Yamaha AICA                                                                                         |
| adpcm_argo        | ADPCM Argonaut Games                                                                                      |
| adpcm_ct          | ADPCM Creative Technology                                                                                 |
| adpcm_dtk         | ADPCM Nintendo Gamecube DTK                                                                               |
| adpcm_ea          | ADPCM Electronic Arts                                                                                     |
| adpcm_ea_maxis_xa | ADPCM Electronic Arts Maxis CDROM XA                                                                      |
| adpcm_ea_r1       | ADPCM Electronic Arts R1                                                                                  |
| adpcm_ea_r2       | ADPCM Electronic Arts R2                                                                                  |
| adpcm_ea_r3       | ADPCM Electronic Arts R3                                                                                  |
| adpcm_ea_xas      | ADPCM Electronic Arts XAS                                                                                 |
| adpcm_g722        | G.722 ADPCM (decoders: g722) (encoders: g722)                                                             |
| adpcm_g726        | G.726 ADPCM (decoders: g726) (encoders: g726)                                                             |
| adpcm_g726le      | G.726 ADPCM little-endian (decoders: g726le) (encoders: g726le)                                           |
| adpcm_ima_acorn   | ADPCM IMA Acorn Replay                                                                                    |
| adpcm_ima_alp     | ADPCM IMA High Voltage Software ALP                                                                       |
| adpcm_ima_amv     | ADPCM IMA AMV                                                                                             |
| adpcm_ima_apc     | ADPCM IMA CRYO APC                                                                                        |
| adpcm_ima_apm     | ADPCM IMA Ubisoft APM                                                                                     |
| adpcm_ima_cunning | ADPCM IMA Cunning Developments                                                                            |
| adpcm_ima_dat4    | ADPCM IMA Eurocom DAT4                                                                                    |
| adpcm_ima_dk3     | ADPCM IMA Duck DK3                                                                                        |
| adpcm_ima_dk4     | ADPCM IMA Duck DK4                                                                                        |
| adpcm_ima_ea_eacs | ADPCM IMA Electronic Arts EACS                                                                            |
| adpcm_ima_ea_sead | ADPCM IMA Electronic Arts SEAD                                                                            |
| adpcm_ima_iss     | ADPCM IMA Funcom ISS                                                                                      |
| adpcm_ima_moflex  | ADPCM IMA MobiClip MOFLEX                                                                                 |
| adpcm_ima_mtf     | ADPCM IMA Capcom's MT Framework                                                                           |
| adpcm_ima_oki     | ADPCM IMA Dialogic OKI                                                                                    |
| adpcm_ima_qt      | ADPCM IMA QuickTime                                                                                       |
| adpcm_ima_rad     | ADPCM IMA Radical                                                                                         |
| adpcm_ima_smjpeg  | ADPCM IMA Loki SDL MJPEG                                                                                  |
| adpcm_ima_ssi     | ADPCM IMA Simon & Schuster Interactive                                                                    |
| adpcm_ima_wav     | ADPCM IMA WAV                                                                                             |
| adpcm_ima_ws      | ADPCM IMA Westwood                                                                                        |
| adpcm_ms          | ADPCM Microsoft                                                                                           |
| adpcm_mtaf        | ADPCM MTAF                                                                                                |
| adpcm_psx         | ADPCM Playstation                                                                                         |
| adpcm_sbpro_2     | ADPCM Sound Blaster Pro 2-bit                                                                             |
| adpcm_sbpro_3     | ADPCM Sound Blaster Pro 2.6-bit                                                                           |
| adpcm_sbpro_4     | ADPCM Sound Blaster Pro 4-bit                                                                             |
| adpcm_swf         | ADPCM Shockwave Flash                                                                                     |
| adpcm_thp         | ADPCM Nintendo THP                                                                                        |
| adpcm_thp_le      | ADPCM Nintendo THP (Little-Endian)                                                                        |
| adpcm_vima        | LucasArts VIMA audio                                                                                      |
| adpcm_xa          | ADPCM CDROM XA                                                                                            |
| adpcm_xmd         | ADPCM Konami XMD                                                                                          |
| adpcm_yamaha      | ADPCM Yamaha                                                                                              |
| adpcm_zork        | ADPCM Zork                                                                                                |
| alac              | ALAC (Apple Lossless Audio Codec)                                                                         |
| amr_nb            | AMR-NB (Adaptive Multi-Rate NarrowBand) (decoders: amrnb libopencore_amrnb) (encoders: libopencore_amrnb) |
| amr_wb            | AMR-WB (Adaptive Multi-Rate WideBand) (decoders: amrwb libopencore_amrwb)                                 |
| anull             | Null audio codec                                                                                          |
| apac              | Marian's A-pac audio                                                                                      |
| ape               | Monkey's Audio                                                                                            |
| aptx              | aptX (Audio Processing Technology for Bluetooth)                                                          |
| aptx_hd           | aptX HD (Audio Processing Technology for Bluetooth)                                                       |
| atrac1            | ATRAC1 (Adaptive TRansform Acoustic Coding)                                                               |
| atrac3            | ATRAC3 (Adaptive TRansform Acoustic Coding 3)                                                             |
| atrac3al          | ATRAC3 AL (Adaptive TRansform Acoustic Coding 3 Advanced Lossless)                                        |
| atrac3p           | ATRAC3+ (Adaptive TRansform Acoustic Coding 3+) (decoders: atrac3plus)                                    |
| atrac3pal         | ATRAC3+ AL (Adaptive TRansform Acoustic Coding 3+ Advanced Lossless) (decoders: atrac3plusal)             |
| atrac9            | ATRAC9 (Adaptive TRansform Acoustic Coding 9)                                                             |
| avc               | On2 Audio for Video Codec (decoders: on2avc)                                                              |
| binkaudio_dct     | Bink Audio (DCT)                                                                                          |
| binkaudio_rdft    | Bink Audio (RDFT)                                                                                         |
| bmv_audio         | Discworld II BMV audio                                                                                    |
| bonk              | Bonk audio                                                                                                |
| cbd2_dpcm         | DPCM Cuberoot-Delta-Exact                                                                                 |
| comfortnoise      | RFC 3389 Comfort Noise                                                                                    |
| cook              | Cook / Cooker / Gecko (RealAudio G2)                                                                      |
| derf_dpcm         | DPCM Xilam DERF                                                                                           |
| dfpwm             | DFPWM (Dynamic Filter Pulse Width Modulation)                                                             |
| dolby_e           | Dolby E                                                                                                   |
| dsd_lsbf          | DSD (Direct Stream Digital), least significant bit first                                                  |
| dsd_lsbf_planar   | DSD (Direct Stream Digital), least significant bit first, planar                                          |
| dsd_msbf          | DSD (Direct Stream Digital), most significant bit first                                                   |
| dsd_msbf_planar   | DSD (Direct Stream Digital), most significant bit first, planar                                           |
| dsicinaudio       | Delphine Software International CIN audio                                                                 |
| dss_sp            | Digital Speech Standard - Standard Play mode (DSS SP)                                                     |
| dst               | DST (Direct Stream Transfer)                                                                              |
| dts               | DCA (DTS Coherent Acoustics) (decoders: dca) (encoders: dca)                                              |
| dvaudio           | DV audio                                                                                                  |
| eac3              | ATSC A/52B (AC-3, E-AC-3)                                                                                 |
| evrc              | EVRC (Enhanced Variable Rate Codec)                                                                       |
| fastaudio         | MobiClip FastAudio                                                                                        |
| flac              | FLAC (Free Lossless Audio Codec)                                                                          |
| ftr               | FTR Voice                                                                                                 |
| g723_1            | G.723.1                                                                                                   |
| g729              | G.729                                                                                                     |
| gremlin_dpcm      | DPCM Gremlin                                                                                              |
| gsm               | GSM                                                                                                       |
| gsm_ms            | GSM Microsoft variant                                                                                     |
| hca               | CRI HCA                                                                                                   |
| hcom              | HCOM Audio                                                                                                |
| iac               | IAC (Indeo Audio Coder)                                                                                   |
| ilbc              | iLBC (Internet Low Bitrate Codec)                                                                         |
| imc               | IMC (Intel Music Coder)                                                                                   |
| interplay_dpcm    | DPCM Interplay                                                                                            |
| interplayacm      | Interplay ACM                                                                                             |
| mace3             | MACE (Macintosh Audio Compression/Expansion) 3:1                                                          |
| mace6             | MACE (Macintosh Audio Compression/Expansion) 6:1                                                          |
| metasound         | Voxware MetaSound                                                                                         |
| misc4             | Micronas SC-4 Audio                                                                                       |
| mlp               | MLP (Meridian Lossless Packing)                                                                           |
| mp1               | MP1 (MPEG audio layer 1) (decoders: mp1 mp1float)                                                         |
| mp2               | MP2 (MPEG audio layer 2) (decoders: mp2 mp2float) (encoders: mp2 mp2fixed libtwolame)                     |
| mp3               | MP3 (MPEG audio layer 3) (decoders: mp3float mp3) (encoders: libmp3lame mp3_mf)                           |
| mp3adu            | ADU (Application Data Unit) MP3 (MPEG audio layer 3) (decoders: mp3adufloat mp3adu)                       |
| mp3on4            | MP3onMP4 (decoders: mp3on4float mp3on4)                                                                   |
| mp4als            | MPEG-4 Audio Lossless Coding (ALS) (decoders: als)                                                        |
| msnsiren          | MSN Siren                                                                                                 |
| musepack7         | Musepack SV7 (decoders: mpc7)                                                                             |
| musepack8         | Musepack SV8 (decoders: mpc8)                                                                             |
| nellymoser        | Nellymoser Asao                                                                                           |
| opus              | Opus (Opus Interactive Audio Codec) (decoders: opus libopus) (encoders: opus libopus)                     |
| osq               | OSQ (Original Sound Quality)                                                                              |
| paf_audio         | Amazing Studio Packed Animation File Audio                                                                |
| pcm_alaw          | PCM A-law / G.711 A-law                                                                                   |
| pcm_bluray        | PCM signed 16\|20\|24-bit big-endian for Blu-ray media                                                   |
| pcm_dvd           | PCM signed 20\|24-bit big-endian                                                                          |
| pcm_f16le         | PCM 16.8 floating point little-endian                                                                     |
| pcm_f24le         | PCM 24.0 floating point little-endian                                                                     |
| pcm_f32be         | PCM 32-bit floating point big-endian                                                                      |
| pcm_f32le         | PCM 32-bit floating point little-endian                                                                   |
| pcm_f64be         | PCM 64-bit floating point big-endian                                                                      |
| pcm_f64le         | PCM 64-bit floating point little-endian                                                                   |
| pcm_lxf           | PCM signed 20-bit little-endian planar                                                                    |
| pcm_mulaw         | PCM mu-law / G.711 mu-law                                                                                 |
| pcm_s16be         | PCM signed 16-bit big-endian                                                                              |
| pcm_s16be_planar  | PCM signed 16-bit big-endian planar                                                                       |
| pcm_s16le         | PCM signed 16-bit little-endian                                                                           |
| pcm_s16le_planar  | PCM signed 16-bit little-endian planar                                                                    |
| pcm_s24be         | PCM signed 24-bit big-endian                                                                              |
| pcm_s24daud       | PCM D-Cinema audio signed 24-bit                                                                          |
| pcm_s24le         | PCM signed 24-bit little-endian                                                                           |
| pcm_s24le_planar  | PCM signed 24-bit little-endian planar                                                                    |
| pcm_s32be         | PCM signed 32-bit big-endian                                                                              |
| pcm_s32le         | PCM signed 32-bit little-endian                                                                           |
| pcm_s32le_planar  | PCM signed 32-bit little-endian planar                                                                    |
| pcm_s64be         | PCM signed 64-bit big-endian                                                                              |
| pcm_s64le         | PCM signed 64-bit little-endian                                                                           |
| pcm_s8            | PCM signed 8-bit                                                                                          |
| pcm_s8_planar     | PCM signed 8-bit planar                                                                                   |
| pcm_sga           | PCM SGA                                                                                                   |
| pcm_u16be         | PCM unsigned 16-bit big-endian                                                                            |
| pcm_u16le         | PCM unsigned 16-bit little-endian                                                                         |
| pcm_u24be         | PCM unsigned 24-bit big-endian                                                                            |
| pcm_u24le         | PCM unsigned 24-bit little-endian                                                                         |
| pcm_u32be         | PCM unsigned 32-bit big-endian                                                                            |
| pcm_u32le         | PCM unsigned 32-bit little-endian                                                                         |
| pcm_u8            | PCM unsigned 8-bit                                                                                        |
| pcm_vidc          | PCM Archimedes VIDC                                                                                       |
| qcelp             | QCELP / PureVoice                                                                                         |
| qdm2              | QDesign Music Codec 2                                                                                     |
| qdmc              | QDesign Music                                                                                             |
| qoa               | QOA (Quite OK Audio)                                                                                      |
| ra_144            | RealAudio 1.0 (14.4K) (decoders: real_144) (encoders: real_144)                                           |
| ra_288            | RealAudio 2.0 (28.8K) (decoders: real_288)                                                                |
| ralf              | RealAudio Lossless                                                                                        |
| rka               | RKA (RK Audio)                                                                                            |
| roq_dpcm          | DPCM id RoQ                                                                                               |
| s302m             | SMPTE 302M                                                                                                |
| sbc               | SBC (low-complexity subband codec)                                                                        |
| sdx2_dpcm         | DPCM Squareroot-Delta-Exact                                                                               |
| shorten           | Shorten                                                                                                   |
| sipr              | RealAudio SIPR / ACELP.NET                                                                                |
| siren             | Siren                                                                                                     |
| smackaudio        | Smacker audio (decoders: smackaud)                                                                        |
| sol_dpcm          | DPCM Sol                                                                                                  |
| sonic             | Sonic                                                                                                     |
| speex             | Speex                                                                                                     |
| tak               | TAK (Tom's lossless Audio Kompressor)                                                                     |
| truehd            | TrueHD                                                                                                    |
| truespeech        | DSP Group TrueSpeech                                                                                      |
| tta               | TTA (True Audio)                                                                                          |
| twinvq            | VQF TwinVQ                                                                                                |
| vmdaudio          | Sierra VMD audio                                                                                          |
| vorbis            | Vorbis (decoders: vorbis libvorbis) (encoders: vorbis libvorbis)                                          |
| wady_dpcm         | DPCM Marble WADY                                                                                          |
| wavarc            | Waveform Archiver                                                                                         |
| wavesynth         | Wave synthesis pseudo-codec                                                                               |
| wavpack           | WavPack                                                                                                   |
| westwood_snd1     | Westwood Audio (SND1) (decoders: ws_snd1)                                                                 |
| wmalossless       | Windows Media Audio Lossless                                                                              |
| wmapro            | Windows Media Audio 9 Professional                                                                        |
| wmav1             | Windows Media Audio 1                                                                                     |
| wmav2             | Windows Media Audio 2                                                                                     |
| wmavoice          | Windows Media Audio Voice                                                                                 |
| xan_dpcm          | DPCM Xan                                                                                                  |
| xma1              | Xbox Media Audio 1                                                                                        |
| xma2              | Xbox Media Audio 2                                                                                        |
