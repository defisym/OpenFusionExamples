#!/usr/bin/env python3
"""Verify each extension is standalone and their shared export payloads agree."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--ffmpeg',type=Path,required=True)
parser.add_argument('--sdl',type=Path,required=True)
args=parser.parse_args()
abis=['arm64-v8a','armeabi-v7a','x86','x86_64']
with zipfile.ZipFile(args.ffmpeg) as video,zipfile.ZipFile(args.sdl) as music:
    for zip,own,libs in [(video,'CRunFFMpeg.java',['ofe_ffmpeg','avcodec','avformat','avutil','swscale','swresample','avfilter']),
                         (music,'CRunSDL_Mixer.java',['ofe_sdl_mixer'])]:
        assert zip.testzip() is None
        manifest=next(n for n in zip.namelist() if n.endswith('package-manifest.json'))
        for name,sha in json.loads(zip.read(manifest)).items():assert hashlib.sha256(zip.read(name)).hexdigest()==sha,name
        assert 'app/src/main/java/Extensions/'+own in zip.namelist()
        for abi in abis:
            for lib in libs+['SDL2','SDL2_mixer_ext','ofe_audio','c++_shared']:
                assert 'app/src/main/jniLibs/'+abi+'/lib'+lib+'.so' in zip.namelist(),(abi,lib)
        for name in ['FusionStorage','FusionFiles','MediaSource','SDLAudioRuntime']:
            assert 'app/src/main/java/_DeLib/'+name+'.java' in zip.namelist()
        for forbidden in ['AndroidManifest.xml','app/build.gradle','app/src/main/java/Application/CRunApp.java','app/src/main/java/Extensions/CExtStorage.java','app/src/main/java/Runtime/MMFRuntime.java']:
            assert forbidden not in zip.namelist(),forbidden
    shared=set(video.namelist())&set(music.namelist())
    for name in shared:assert video.read(name)==music.read(name),'Different overlapping payload: '+name
    print('PASS: two standalone four-ABI packages, source manifests/CRC,',len(shared),'identical shared files, no Fusion runtime/template replacement')
