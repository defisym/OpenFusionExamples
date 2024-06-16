# FFMpeg

## Description

FFMpeg object for fusion.

Talking about the ADS...Tired about the uncontrollable gap before playing? Don't want the micro-stuttering when VSync is on? Want to play other formats like mp4? Need to protect your precious cut scenes? Then try this object!

Special thanks to @SelfReflection31 (From Github) and @Ruthoranium (From Discord), who help a lot to polishing this extension.

This object is based on cross platform lib, FFMpeg & SDL, in theory there is not too difficult to bring it to other platforms like Android.

**NOTE:** this object may conflict with other objects that need SDL, due to the dll version, and the possible double initialize (SDL needs to be initialized before using, and will fail if it's already initialized).

**NOTE:** this object uses a bitmap surface, as fusion didn't provide methods to write a HWA texture directly, and convert it will cost more than 3ms.

**NOTE:** this object uses SDL_Mixer for multi-instance. Also because of the HWA issue mentioned above, too many objects will take a long time to blit.

**NOTE:** this object runs synchronously, it will try decode video frame in `HandleRunObject` routine called once per frame. If your game is heavy then it will skip more frames. Audio decode is in another thread.

**NOTE:** you need to check `Run when minimized` & `Run while resizing`, or audio will be paused due to main thread is paused

**NOTE:** this object requires FFMpeg & SDL

FFMpeg: you need to download release (e.g., `ffmpeg-n5.0-latest-win32-lgpl-shared-5.0.zip` ) from <https://github.com/sudo-nautilus/FFmpeg-Builds-Win32/releases>, then put dlls into Fusion root & `Runtime/Unicode`, or you need to build X86 version from FFMpeg source code, as the official 32 bit support has stopped.

SDL: you need to download `SDL2-2.24.0-win32-x86.zip` from <https://github.com/libsdl-org/SDL/releases>, then put X86 dlls into Fusion root & `Runtime/Unicode`

SDL Mixer X: Upstream is <https://github.com/WohlSoft/SDL-Mixer-X>, this extension added a few custom functions to original lib, so please clone [this repo](https://github.com/defisym/SDL-Mixer-X) first, then follow the [guide](https://github.com/WohlSoft/SDL-Mixer-X/blob/master/docs/index.md#general-build-on-unix-like-platform-and-install-into-the-system) to build it by yourself

Here follows all dlls needed by this object:

- FFMpeg
  - swscale-6.dll
  - avcodec-59.dll
  - avdevice-59.dll
  - avfilter-8.dll
  - avformat-59.dll
  - avutil-57.dll
  - swresample-4.dll

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

- Hardware Decode
  - Hardware Device
    - *set the device you want object to use. object will try using the device you set, if not supported, it will try using other hardware devices. if all devices are not supported, it will fallback to software decode*

## Action

- Reset Display
  - *reset display to transparent if video is not playing*

- Open Video
  - *If open failed, extension will keep previous frame*
  - *If key is not empty, extension will treat it as an encrypted file. Decode a normal file in encrypted way usually will crash. You can encrypt files by another extension.*
  - *Encrypt & decrypt are based on AES so without your key no one can get your file, unless they dump it from memory.*
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

- Set Size
  - *set object size*

- Set Override Codec
  - *Force to use given codec. E.g., use `libvpx-vp9` to enable `WebM` alpha support*
  - *if given codec is invalid or cannot open video. extension will reset to default then try again. If the error still exists, `On Video Open Failed` will be triggered*
  - *supported codecs: see Appendix*
- Set Hardware Decode Device
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

- Get Video Override Codec Name
- Get Audio Override Codec Name

- Get Hardware Decode State
- Get Actual Hardware Device
  - *get the device object is using*
- Get Wanted Hardware Device
  - *get the device you set*

## Appendix

### Supported Codecs

#### Video Codec

- `aasc`
- `aic`
- `alias_pix`
- `agm`
- `amv`
- `anm`
- `ansi`
- `apng`
- `arbc`
- `argo`
- `asv1`
- `asv2`
- `aura`
- `aura2`
- `avrp`
- `avrn`
- `avs`
- `avui`
- `ayuv`
- `bethsoftvid`
- `bfi`
- `binkvideo`
- `bitpacked`
- `bmp`
- `bmv_video`
- `brender_pix`
- `c93`
- `cavs`
- `cdgraphics`
- `cdtoons`
- `cdxl`
- `cfhd`
- `cinepak`
- `clearvideo`
- `cljr`
- `cllc`
- `cpia`
- `cri`
- `camstudio`
- `cyuv`
- `dds`
- `dfa`
- `dirac`
- `dnxhd`
- `dpx`
- `dsicinvideo`
- `dvvideo`
- `dxa`
- `dxtory`
- `dxv`
- `eacmv`
- `eamad`
- `eatgq`
- `eatgv`
- `eatqi`
- `8bps`
- `escape124`
- `escape130`
- `exr`
- `ffv1`
- `ffvhuff`
- `fic`
- `fits`
- `flashsv`
- `flashsv2`
- `flic`
- `flv`
- `fmvc`
- `4xm`
- `fraps`
- `frwu`
- `g2m`
- `gdv`
- `gem`
- `gif`
- `h261`
- `h263`
- `h263i`
- `h263p`
- `h264`
- `h264_qsv`
- `hap`
- `hevc`
- `hevc_qsv`
- `hnm4video`
- `hq_hqa`
- `hqx`
- `huffyuv`
- `hymt`
- `idcinvideo`
- `iff`
- `imm4`
- `imm5`
- `indeo2`
- `indeo3`
- `indeo4`
- `indeo5`
- `interplayvideo`
- `ipu`
- `jpeg2000`
- `jpegls`
- `jv`
- `kgv1`
- `kmvc`
- `lagarith`
- `loco`
- `lscr`
- `m101`
- `magicyuv`
- `mdec`
- `mimic`
- `mjpeg`
- `mjpegb`
- `mmvideo`
- `mobiclip`
- `motionpixels`
- `mpeg1video`
- `mpeg2video`
- `mpeg4`
- `mpegvideo`
- `mpeg2_qsv`
- `msa1`
- `mscc`
- `msmpeg4v1`
- `msmpeg4v2`
- `msmpeg4`
- `msp2`
- `msrle`
- `mss1`
- `mss2`
- `msvideo1`
- `mszh`
- `mts2`
- `mv30`
- `mvc1`
- `mvc2`
- `mvdv`
- `mvha`
- `mwsc`
- `mxpeg`
- `notchlc`
- `nuv`
- `paf_video`
- `pam`
- `pbm`
- `pcx`
- `pfm`
- `pgm`
- `pgmyuv`
- `pgx`
- `phm`
- `photocd`
- `pictor`
- `pixlet`
- `png`
- `ppm`
- `prores`
- `prosumer`
- `psd`
- `ptx`
- `qdraw`
- `qoi`
- `qpeg`
- `qtrle`
- `r10k`
- `r210`
- `rasc`
- `rawvideo`
- `rl2`
- `roqvideo`
- `rpza`
- `rscc`
- `rv10`
- `rv20`
- `rv30`
- `rv40`
- `sanm`
- `scpr`
- `screenpresso`
- `sga`
- `sgi`
- `sgirle`
- `sheervideo`
- `simbiosis_imx`
- `smackvid`
- `smc`
- `smvjpeg`
- `snow`
- `sp5x`
- `speedhq`
- `srgc`
- `sunrast`
- `svq1`
- `svq3`
- `targa`
- `targa_y216`
- `tdsc`
- `theora`
- `thp`
- `tiertexseqvideo`
- `tiff`
- `tmv`
- `truemotion1`
- `truemotion2`
- `truemotion2rt`
- `camtasia`
- `tscc2`
- `txd`
- `ultimotion`
- `utvideo`
- `v210`
- `v210x`
- `v308`
- `v408`
- `v410`
- `vb`
- `vbn`
- `vble`
- `vc1`
- `vc1image`
- `vc1_qsv`
- `vcr1`
- `vmdvideo`
- `vmnc`
- `vp3`
- `vp4`
- `vp5`
- `vp6`
- `vp6a`
- `vp6f`
- `vp7`
- `vp8`
- `vp9`
- `vqavideo`
- `webp`
- `wcmv`
- `wrapped_avframe`
- `wmv1`
- `wmv2`
- `wmv3`
- `wmv3image`
- `wnv1`
- `xan_wc3`
- `xan_wc4`
- `xbm`
- `xface`
- `xl`
- `xpm`
- `xwd`
- `y41p`
- `ylc`
- `yop`
- `yuv4`
- `012v`
- `zerocodec`
- `zlib`
- `zmbv`
- `libdav1d`
- `libjxl`
- `libopenjpeg`
- `libvpx`
- `libvpx-vp9`
- `bintext`
- `xbin`
- `idf`
- `libaom-av1`
- `av1`
- `av1_cuvid`
- `av1_qsv`
- `libopenh264`
- `h264_cuvid`
- `hevc_cuvid`
- `mjpeg_cuvid`
- `mjpeg_qsv`
- `mpeg1_cuvid`
- `mpeg2_cuvid`
- `mpeg4_cuvid`
- `vc1_cuvid`
- `vp8_cuvid`
- `vp8_qsv`
- `vp9_cuvid`
- `vp9_qsv`

#### Audio Codec

- `comfortnoise`
- `dvaudio`
- `8svx_exp`
- `8svx_fib`
- `s302m`
- `speex`
- `aac`
- `aac_fixed`
- `aac_latm`
- `ac3`
- `ac3_fixed`
- `acelp.kelvin`
- `alac`
- `als`
- `amrnb`
- `amrwb`
- `ape`
- `aptx`
- `aptx_hd`
- `atrac1`
- `atrac3`
- `atrac3al`
- `atrac3plus`
- `atrac3plusal`
- `atrac9`
- `binkaudio_dct`
- `binkaudio_rdft`
- `bmv_audio`
- `cook`
- `dca`
- `dfpwm`
- `dolby_e`
- `dsd_lsbf`
- `dsd_msbf`
- `dsd_lsbf_planar`
- `dsd_msbf_planar`
- `dsicinaudio`
- `dss_sp`
- `dst`
- `eac3`
- `evrc`
- `fastaudio`
- `wavesynth`
- `flac`
- `g723_1`
- `g729`
- `gsm`
- `gsm_ms`
- `hca`
- `hcom`
- `iac`
- `ilbc`
- `imc`
- `interplayacm`
- `mace3`
- `mace6`
- `metasound`
- `mlp`
- `mp1`
- `mp1float`
- `mp2`
- `mp2float`
- `mp3float`
- `mp3`
- `mp3adufloat`
- `mp3adu`
- `mp3on4float`
- `mp3on4`
- `mpc7`
- `mpc8`
- `msnsiren`
- `nellymoser`
- `on2avc`
- `opus`
- `paf_audio`
- `qcelp`
- `qdm2`
- `qdmc`
- `real_144`
- `real_288`
- `ralf`
- `sbc`
- `shorten`
- `sipr`
- `siren`
- `smackaud`
- `sonic`
- `tak`
- `truehd`
- `truespeech`
- `tta`
- `twinvq`
- `vmdaudio`
- `vorbis`
- `wavpack`
- `wmalossless`
- `wmapro`
- `wmav1`
- `wmav2`
- `wmavoice`
- `ws_snd1`
- `xma1`
- `xma2`
- `pcm_alaw`
- `pcm_bluray`
- `pcm_dvd`
- `pcm_f16le`
- `pcm_f24le`
- `pcm_f32be`
- `pcm_f32le`
- `pcm_f64be`
- `pcm_f64le`
- `pcm_lxf`
- `pcm_mulaw`
- `pcm_s8`
- `pcm_s8_planar`
- `pcm_s16be`
- `pcm_s16be_planar`
- `pcm_s16le`
- `pcm_s16le_planar`
- `pcm_s24be`
- `pcm_s24daud`
- `pcm_s24le`
- `pcm_s24le_planar`
- `pcm_s32be`
- `pcm_s32le`
- `pcm_s32le_planar`
- `pcm_s64be`
- `pcm_s64le`
- `pcm_sga`
- `pcm_u8`
- `pcm_u16be`
- `pcm_u16le`
- `pcm_u24be`
- `pcm_u24le`
- `pcm_u32be`
- `pcm_u32le`
- `pcm_vidc`
- `derf_dpcm`
- `gremlin_dpcm`
- `interplay_dpcm`
- `roq_dpcm`
- `sdx2_dpcm`
- `sol_dpcm`
- `xan_dpcm`
- `adpcm_4xm`
- `adpcm_adx`
- `adpcm_afc`
- `adpcm_agm`
- `adpcm_aica`
- `adpcm_argo`
- `adpcm_ct`
- `adpcm_dtk`
- `adpcm_ea`
- `adpcm_ea_maxis_xa`
- `adpcm_ea_r1`
- `adpcm_ea_r2`
- `adpcm_ea_r3`
- `adpcm_ea_xas`
- `g722`
- `g726`
- `g726le`
- `adpcm_ima_acorn`
- `adpcm_ima_amv`
- `adpcm_ima_alp`
- `adpcm_ima_apc`
- `adpcm_ima_apm`
- `adpcm_ima_cunning`
- `adpcm_ima_dat4`
- `adpcm_ima_dk3`
- `adpcm_ima_dk4`
- `adpcm_ima_ea_eacs`
- `adpcm_ima_ea_sead`
- `adpcm_ima_iss`
- `adpcm_ima_moflex`
- `adpcm_ima_mtf`
- `adpcm_ima_oki`
- `adpcm_ima_qt`
- `adpcm_ima_rad`
- `adpcm_ima_ssi`
- `adpcm_ima_smjpeg`
- `adpcm_ima_wav`
- `adpcm_ima_ws`
- `adpcm_ms`
- `adpcm_mtaf`
- `adpcm_psx`
- `adpcm_sbpro_2`
- `adpcm_sbpro_3`
- `adpcm_sbpro_4`
- `adpcm_swf`
- `adpcm_thp`
- `adpcm_thp_le`
- `adpcm_vima`
- `adpcm_xa`
- `adpcm_yamaha`
- `adpcm_zork`
- `libopencore_amrnb`
- `libopencore_amrwb`
- `libopus`
- `libvorbis`
