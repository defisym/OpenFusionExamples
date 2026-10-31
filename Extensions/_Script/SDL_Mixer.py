"""Publish the Android SDL Mixer X JNI extension without replacing Fusion templates."""
from __future__ import annotations
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import zipfile

EXTENSIONS=Path(__file__).resolve().parent.parent
PORT=EXTENSIONS/'SDL_Mixer/Android'
JAVA=EXTENSIONS/'Android_Gradle/app/src/main/java'

def module(name,path):
    spec=importlib.util.spec_from_file_location(name,path);loaded=importlib.util.module_from_spec(spec);spec.loader.exec_module(loaded);return loaded

def copy(source,target):
    target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)

def package(sources,source_cache,builds,ndk,output):
    common=module('ofe_android_native',Path(__file__).with_name('AndroidNative.py'))
    host=next((ndk/'toolchains/llvm/prebuilt').iterdir());suffix='.exe' if os.name=='nt' else ''
    output.parent.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='sdl-mixer-package-',dir=output.parent) as temporary:
        stage=Path(temporary)/'stage';stage.mkdir()
        for name in ['Extensions/CRunSDL_Mixer.java','_DeLib/SDLMixer.java','_DeLib/SDLMixerStorage.java','_DeLib/SDLAudioRuntime.java','_DeLib/MediaSource.java','_DeLib/FusionStorage.java','_DeLib/FusionFiles.java']:
            copy(JAVA/name,stage/'app/src/main/java'/name)
        vendor=sources['SDL2']/'android-project/app/src/main/java/org/libsdl/app'
        for source in vendor.glob('*.java'):copy(source,stage/'app/src/main/java/org/libsdl/app'/source.name)
        common.copy_audio_sources(sources,source_cache,PORT,stage)
        copy(EXTENSIONS.parent/'LICENSE',stage/'Sources/SDL_Mixer/LICENSE')
        for name in ['NOTICE','NOTICE.toolchain','source.properties']:copy(ndk/name,stage/'Sources/Android-NDK'/name)
        rules=stage/'app/libs/SDL_Mixer-JNI-rules.jar';rules.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(rules,'w',zipfile.ZIP_DEFLATED) as jar:
            jar.writestr('META-INF/proguard/SDL_Mixer.pro','-keep class org.libsdl.app.** { *; }\n-keep class _DeLib.SDLMixer { *; }\n-keep class _DeLib.SDLAudioRuntime { *; }\n')
        system={'libc.so','libm.so','libdl.so','liblog.so','libandroid.so','libOpenSLES.so','libGLESv1_CM.so','libGLESv2.so','libEGL.so','libz.so'}
        for abi,directory in builds.items():
            target=stage/'app/src/main/jniLibs'/abi
            for library in ['SDL2','SDL2_mixer_ext','ofe_audio','ofe_sdl_mixer']:copy(directory/'lib'/('lib'+library+'.so'),target/('lib'+library+'.so'))
            triple,machine=common.ABIS[abi]
            copy(host/'sysroot/usr/lib'/triple/'libc++_shared.so',target/'libc++_shared.so')
            names={p.name for p in target.glob('*.so')}
            for binary in target.glob('*.so'):
                subprocess.run([str(host/'bin'/('llvm-strip'+suffix)),'--strip-unneeded',str(binary)],check=True)
                for dependency in common.check_elf(binary,host/'bin'/('llvm-readelf'+suffix),machine):
                    if dependency not in names|system:raise ValueError(f'Unpackaged native dependency {dependency}: {binary}')
        hashes={p.relative_to(stage).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(stage.rglob('*')) if p.is_file()}
        (stage/'SDL_Mixer-package-manifest.json').write_text(json.dumps(hashes,indent=2)+'\n')
        archive=Path(temporary)/'SDL_Mixer.zip'
        with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as zip:
            for path in sorted(stage.rglob('*')):
                if path.is_file():zip.write(path,path.relative_to(stage))
        archive.replace(output)
    print('Published:',output)

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ndk',type=Path);parser.add_argument('--cmake',type=Path)
    parser.add_argument('--sources',type=Path,default=PORT/'out/sources')
    parser.add_argument('--build-root',type=Path,default=PORT/'out/build')
    parser.add_argument('--output',type=Path,default=EXTENSIONS/'SDL_Mixer/ToInstall/Files/Data/Runtime/Android/SDL_Mixer.zip')
    parser.add_argument('--abi',action='append',choices=['arm64-v8a','armeabi-v7a','x86','x86_64'])
    parser.add_argument('--no-pause',action='store_true',help=argparse.SUPPRESS)
    args=parser.parse_args()
    builder=module('ofe_mixer_builder',PORT/'build_android.py')
    ninja=None
    if args.ndk and args.cmake:ndk,cmake=args.ndk,args.cmake
    else:ndk,cmake,ninja=module('ofe_android_tools',Path(__file__).with_name('AndroidNative.py')).native_tools()
    sources=builder.prepare(args.sources.resolve())
    builds=builder.build(sources,args.build_root.resolve(),cmake.resolve(),ndk.resolve(),args.abi or builder.ABIS,ninja=ninja)
    package(sources,args.sources.resolve(),builds,ndk.resolve(),args.output.resolve())
