"""Publish FFmpeg's Android JNI extension, using the FTW SDK/NDK discovery rules."""
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
import sys
sys.path.insert(0,str(Path(__file__).resolve().parent))
from AndroidNative import ABIS, check_elf, copy_audio_sources

EXTENSIONS = Path(__file__).resolve().parent.parent
ANDROID = EXTENSIONS / 'Android_Gradle'
PORT = EXTENSIONS / 'FFMpeg/Android'
JAVA = ANDROID / 'app/src/main/java'
SEEDS = ['Extensions/CRunFFMpeg.java', '_DeLib/FFMpegPlayer.java', '_DeLib/FFMpegTexture.java', '_DeLib/FFMpegSource.java', '_DeLib/MediaSource.java', '_DeLib/SDLAudioRuntime.java', '_DeLib/FusionStorage.java', '_DeLib/FusionFiles.java', '_DeLib/FFMpegStorage.java']
COMPONENTS = ('avformat', 'avcodec', 'avutil', 'swscale', 'swresample', 'avfilter')

def sdk_builder():
    spec = importlib.util.spec_from_file_location('ofe_ffmpeg_sdk', PORT/'build_sdk.py')
    builder = importlib.util.module_from_spec(spec); spec.loader.exec_module(builder)
    return builder


def ftw_tools():
    spec = importlib.util.spec_from_file_location('ofe_ftw_publisher', Path(__file__).with_name('FindTheWay.py'))
    module = importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
    return module

def run(command):
    print('+', subprocess.list2cmdline(list(map(str,command))), flush=True)
    subprocess.run(list(map(str,command)), check=True)

def copy(source, destination):
    destination.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(source, destination)


def package(sdk, ndk, cmake, output, abis, build_root, audio_sources, audio_cache, audio_builds, ninja=None):
    host = next((ndk/'toolchains/llvm/prebuilt').iterdir())
    suffix = '.exe' if os.name == 'nt' else ''
    readelf = host/'bin'/('llvm-readelf'+suffix)
    # SDKs must carry their source and license material alongside binary dependencies.
    manifest = sdk/'manifest.json'
    if not manifest.is_file(): raise FileNotFoundError('FFmpeg SDK manifest missing; use Android/build_sdk.py')
    metadata = json.loads(manifest.read_text())
    if not sdk_builder().sdk_matches(sdk, abis):
        raise ValueError('FFmpeg Android SDK does not match FFmpegVersion.cmake or requested ABIs; rebuild with Android/build_sdk.py')
    source = sdk/f"ffmpeg-n{metadata['version']}.tar.gz"
    if hashlib.sha256(source.read_bytes()).hexdigest() != metadata['source_sha256']:
        raise ValueError('FFmpeg corresponding source hash mismatch')
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='ffmpeg-package-', dir=output.parent) as temp:
        stage = Path(temp)/'stage'; stage.mkdir()
        for name in SEEDS: copy(JAVA/name, stage/'app/src/main/java'/name)
        vendor=audio_sources['SDL2']/'android-project/app/src/main/java/org/libsdl/app'
        for source_java in vendor.glob('*.java'):copy(source_java,stage/'app/src/main/java/org/libsdl/app'/source_java.name)
        copy_audio_sources(audio_sources,audio_cache,EXTENSIONS/'SDL_Mixer/Android',stage)
        copy(EXTENSIONS.parent/'LICENSE',stage/'Sources/SDL_Mixer/LICENSE')
        shutil.copytree(PORT/'cpp', stage/'Sources/FFMpeg/cpp')
        for name in ('FFMpeg.h', 'FFMpeg.cpp', 'FFMpegDefinition.h', 'FFMpegUtilities.h',
                     'FFMpegAdapter.h', 'FFMpegAdapter.cpp', 'FFMpegAdapterAndroid.h',
                     'FFMpegAdapterAndroid.cpp', 'SDLAudioDefinition.h', 'PacketQueue.h', 'MemBuf.h'):
            copy(EXTENSIONS/'_3rdLib'/name, stage/'Sources/_3rdLib'/name)
        for name in ('LockHelper.h', 'HoldHelper.h'):
            copy(EXTENSIONS/'_DeLib'/name, stage/'Sources/_DeLib'/name)
        for name in ('CopyAdapter.h', 'CopyAdapter.cpp', 'CopyAdapterAndroid.h', 'CopyAdapterAndroid.cpp'):
            copy(EXTENSIONS/'FFMpeg/CopyAdapter'/name, stage/'Sources/FFMpeg/CopyAdapter'/name)
        copy(PORT/'build_sdk.py', stage/'Sources/FFMpeg/build_sdk.py')
        copy(EXTENSIONS/'_3rdLib/FFmpeg-build/FFmpegVersion.cmake', stage/'Sources/FFMpeg/FFmpegVersion.cmake')
        copy(PORT/'README.md', stage/'FFMpeg-JNI.md')
        copy(source, stage/'Sources/FFmpeg'/source.name)
        copy(manifest, stage/'Sources/FFmpeg/manifest.json')
        if not (sdk/'licenses/COPYING.LGPLv2.1').is_file(): raise FileNotFoundError('FFmpeg LGPL license missing')
        shutil.copytree(sdk/'licenses', stage/'Sources/FFmpeg/licenses')
        copy(EXTENSIONS.parent/'LICENSE', stage/'Sources/FFMpeg/LICENSE')
        for notice in ('NOTICE', 'NOTICE.toolchain'):
            copy(ndk/notice, stage/'Sources/Android-NDK'/notice)
        copy(ndk/'source.properties', stage/'Sources/Android-NDK/source.properties')
        rules = stage/'app/libs/FFMpeg-JNI-rules.jar'; rules.parent.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(rules, 'w', zipfile.ZIP_DEFLATED) as jar:
            jar.writestr('META-INF/proguard/FFMpeg.pro', '-keep class _DeLib.FFMpegPlayer { *; }\n-keep class _DeLib.FFMpegTexture { *; }\n-keep class org.libsdl.app.** { *; }\n-keep class _DeLib.SDLAudioRuntime { *; }\n')
        for abi in abis:
            triple, machine = ABIS[abi]
            build = build_root/abi; lib = stage/'app/src/main/jniLibs'/abi
            command = [cmake, '-S', PORT/'cpp', '-B', build,
                '-DCMAKE_TOOLCHAIN_FILE='+str(ndk/'build/cmake/android.toolchain.cmake'),
                '-DANDROID_ABI='+abi, '-DANDROID_PLATFORM=android-21', '-DANDROID_STL=c++_shared',
                '-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON', '-DOFE_FFMPEG_ROOT='+str(sdk), '-DOFE_AUDIO_ROOT='+str(audio_builds[abi]), '-DCMAKE_BUILD_TYPE=Release']
            if ninja: command += ['-G', 'Ninja', '-DCMAKE_MAKE_PROGRAM='+str(ninja)]
            run(command); run([cmake, '--build', build, '--parallel', '4'])
            copy(build/'libofe_ffmpeg.so', lib/'libofe_ffmpeg.so')
            for name in ['SDL2','SDL2_mixer_ext','ofe_audio']:copy(audio_builds[abi]/'lib'/('lib'+name+'.so'),lib/('lib'+name+'.so'))
            for component in COMPONENTS: copy(sdk/abi/'lib'/f'lib{component}.so', lib/f'lib{component}.so')
            copy(host/'sysroot/usr/lib'/triple/'libc++_shared.so', lib/'libc++_shared.so')
            for config in ('config.h', 'config.mak'): copy(sdk/abi/config, stage/'Sources/FFmpeg'/abi/config)
            system = {'libc.so','libm.so','libdl.so','liblog.so','libandroid.so','libmediandk.so','libz.so','libGLESv2.so','libOpenSLES.so','libGLESv1_CM.so','libEGL.so'}
            names = {p.name for p in lib.glob('*.so')}
            for binary in lib.glob('*.so'):
                run([host/'bin'/('llvm-strip'+suffix), '--strip-unneeded', binary])
                for dependency in check_elf(binary, readelf, machine):
                    if dependency not in system | names: raise ValueError(f'Unpackaged dependency {dependency}: {binary}')
        hashes = {str(p.relative_to(stage)).replace('\\','/'): hashlib.sha256(p.read_bytes()).hexdigest()
                  for p in sorted(stage.rglob('*')) if p.is_file()}
        (stage/'FFMpeg-package-manifest.json').write_text(json.dumps(hashes, indent=2)+'\n')
        archive = Path(temp)/'FFMpeg.zip'
        with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as zipped:
            for p in sorted(stage.rglob('*')):
                if p.is_file(): zipped.write(p, p.relative_to(stage))
        archive.replace(output)
    print('Published:', output)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ffmpeg-sdk', type=Path, default=Path(os.environ.get('OFE_FFMPEG_ROOT', str(EXTENSIONS/'_3rdLib/FFmpeg-sdk-Android'))))
    parser.add_argument('--ndk', type=Path)
    parser.add_argument('--cmake', type=Path)
    parser.add_argument('--output', type=Path, default=EXTENSIONS/'FFMpeg/ToInstall/Files/Data/Runtime/Android/FFMpeg.zip')
    parser.add_argument('--audio-sources',type=Path,default=EXTENSIONS/'SDL_Mixer/Android/out/sources')
    parser.add_argument('--audio-build-root',type=Path,default=EXTENSIONS/'SDL_Mixer/Android/out/build')
    parser.add_argument('--build-root', type=Path, default=PORT/'out/jni')
    parser.add_argument('--abi', choices=list(ABIS), action='append', help='Subset for development; release packages need all four ABIs')
    parser.add_argument('--no-pause', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()
    ninja = None
    if args.ndk and args.cmake: ndk, cmake = args.ndk, args.cmake
    else:
        ndk, cmake, ninja = ftw_tools().native_tools()
        ndk = args.ndk or ndk; cmake = args.cmake or cmake
    sdk = args.ffmpeg_sdk.resolve()
    if not sdk_builder().sdk_matches(sdk, args.abi or list(ABIS)):
        run([os.sys.executable, PORT/'build_sdk.py', '--ndk', ndk, '--output', sdk])
    audio_spec=importlib.util.spec_from_file_location('ofe_audio_builder',EXTENSIONS/'SDL_Mixer/Android/build_android.py')
    audio_builder=importlib.util.module_from_spec(audio_spec);audio_spec.loader.exec_module(audio_builder)
    audio_sources=audio_builder.prepare(args.audio_sources.resolve())
    audio_builds=audio_builder.build(audio_sources,args.audio_build_root.resolve(),cmake.resolve(),ndk.resolve(),args.abi or list(ABIS),ninja=ninja)
    package(sdk, ndk.resolve(), cmake.resolve(), args.output.resolve(), args.abi or list(ABIS), args.build_root.resolve(), audio_sources, args.audio_sources.resolve(), audio_builds, ninja)
