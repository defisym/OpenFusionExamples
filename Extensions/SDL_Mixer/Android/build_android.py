#!/usr/bin/env python3
"""Prepare pinned SDL2/Mixer X/SoundTouch sources and compile the JNI package's ABIs."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import urllib.request

PORT=Path(__file__).resolve().parent
ABIS=('arm64-v8a','armeabi-v7a','x86','x86_64')

def run(command):
    print('+',subprocess.list2cmdline(list(map(str,command))),flush=True)
    subprocess.run(list(map(str,command)),check=True)

def extract_portable(tar, destination):
    # Windows commonly lacks symlink privilege. Materialize checked source links
    # as files/directories, while leaving the pinned original archive untouched.
    links=[]
    def member_filter(member, path):
        checked=tarfile.data_filter(member,path)
        if checked is not None and (checked.issym() or checked.islnk()):
            links.append(checked);return None
        return checked
    tar.extractall(destination,filter=member_filter)
    for member in links:
        output=destination/member.name
        target=(destination/member.linkname) if member.islnk() else (output.parent/member.linkname)
        if target.is_file(): output.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,output)
        elif target.is_dir():shutil.copytree(target,output,dirs_exist_ok=True)
        # A dangling SoundTouch example link names a not-yet-built desktop DLL;
        # it is not an input to the native core build.

def prepare(cache: Path):
    if sys.version_info < (3,12): raise RuntimeError('Source preparation requires Python 3.12+')
    manifest=json.loads((PORT/'dependencies.json').read_text())
    sources={};cache.mkdir(parents=True,exist_ok=True)
    for name,item in manifest.items():
        archive=cache/(name+'-'+item['revision']+'.tar.gz')
        if not archive.is_file() or hashlib.sha256(archive.read_bytes()).hexdigest()!=item['sha256']:
            partial=archive.with_suffix('.part')
            url='https://codeload.github.com/'+item['repository']+'/tar.gz/'+item['revision']
            with urllib.request.urlopen(url,timeout=120) as response,partial.open('wb') as out: shutil.copyfileobj(response,out)
            if hashlib.sha256(partial.read_bytes()).hexdigest()!=item['sha256']: partial.unlink();raise ValueError(name+' source hash mismatch')
            partial.replace(archive)
        destination=cache/(name+'-'+item['revision'])
        if not destination.is_dir():
            temporary=cache/(name+'-extract')
            if temporary.exists(): shutil.rmtree(temporary)
            temporary.mkdir()
            with tarfile.open(archive) as tar: extract_portable(tar,temporary)
            roots=list(temporary.iterdir())
            if len(roots)!=1 or not (roots[0]/'CMakeLists.txt').is_file(): raise ValueError('Invalid source archive: '+name)
            roots[0].replace(destination);temporary.rmdir()
        sources[name]=destination
    return sources

def build(sources, build_root: Path, cmake: Path, ndk: Path | None, abis, jobs=4, tests=False, ninja=None, codecs=True):
    builds={}
    if ninja is None and os.name=='nt' and cmake.with_name('ninja.exe').is_file(): ninja=cmake.with_name('ninja.exe')
    for abi in abis:
        directory=build_root/abi
        command=[cmake,'-S',PORT/'cpp','-B',directory,'-DCMAKE_BUILD_TYPE=Release',
            '-DOFE_SDL_SOURCE='+str(sources['SDL2']),'-DOFE_MIXERX_SOURCE='+str(sources['MixerX']),
            '-DOFE_SOUNDTOUCH_SOURCE='+str(sources['SoundTouch'])]
        if ninja:
            command += ['-G','Ninja','-DCMAKE_MAKE_PROGRAM='+str(ninja)]
        elif os.name=='nt':
            command += ['-G','Ninja']
        if ndk:
            command += ['-DCMAKE_TOOLCHAIN_FILE='+str(ndk/'build/cmake/android.toolchain.cmake'),
                '-DANDROID_ABI='+abi,'-DANDROID_PLATFORM=android-21','-DANDROID_STL=c++_shared',
                '-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON']
        elif tests: command += ['-DOFE_MIXER_TESTS=ON']
        if codecs:
            # Build SDL first, then the same static AudioCodecs revision/profile as
            # Windows, and finally Mixer X. Reuse the SDL target; no second SDL copy.
            stage=directory/'codecs-sdk'
            ready=all((stage/'lib'/name).exists() for name in ['libopusfile.a','libwavpack.a','libxmp.a'])
            bootstrap=['-DOFE_AUDIOCODECS_ROOT='+str(stage),'-DOFE_AUDIOCODECS_SOURCE='+str(sources['AudioCodecs'])] if ready else ['-DOFE_AUDIOCODECS_ROOT=']
            run(command+bootstrap)
            run([cmake,'--build',directory,'--target','SDL2','--parallel',str(jobs)])
            include=stage/'include/SDL2';include.mkdir(parents=True,exist_ok=True)
            for header in (sources['SDL2']/'include').glob('*.h'):shutil.copy2(header,include/header.name)
            config=directory/'sdl/include-config-release/SDL2/SDL_config.h'
            if not config.exists():config=directory/'sdl/include-config-Release/SDL2/SDL_config.h'
            shutil.copy2(config,include/'SDL_config.h')
            (stage/'lib').mkdir(exist_ok=True)
            sdl_library=directory/'lib/libSDL2.so'
            if not sdl_library.exists():
                sdl_library=next((directory/'lib'/name for name in ['libSDL2-2.0.so','libSDL2.dylib','libSDL2-2.0.dylib'] if (directory/'lib'/name).exists()),sdl_library)
            shutil.copy2(sdl_library,stage/'lib'/('libSDL2.dylib' if sdl_library.suffix=='.dylib' else 'libSDL2.so'))
            codec_command=[cmake,'-S',sources['AudioCodecs'],'-B',directory/'codecs-build',
                '-DCMAKE_BUILD_TYPE=Release','-DCMAKE_INSTALL_PREFIX='+str(stage),
                '-DCMAKE_POLICY_VERSION_MINIMUM=3.5','-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
                '-DSDL2_REPO_PATH='+str(stage),'-DSDL2_INSTALL_DIR='+str(stage),
                '-DDOWNLOAD_SDL2_DEPENDENCY=OFF','-DUSE_LOCAL_SDL2=OFF',
                '-DCODECS_BUILD_SHARED=OFF','-DCODECS_BUILD_STATIC=ON',
                '-DBUILD_ADLMIDI=OFF','-DBUILD_OPNMIDI=OFF','-DBUILD_MPG123=OFF','-DBUILD_FLAC=OFF',
                '-DGME_YM2612_EMU=GENS']
            if ninja:codec_command+=['-G','Ninja','-DCMAKE_MAKE_PROGRAM='+str(ninja)]
            elif os.name=='nt':codec_command+=['-G','Ninja']
            if ndk and abi in ('armeabi-v7a','x86'):codec_command+=['-DHAVE_FSEEKO:BOOL=OFF']
            if ndk:codec_command+=['-DCMAKE_TOOLCHAIN_FILE='+str(ndk/'build/cmake/android.toolchain.cmake'),
                '-DANDROID_ABI='+abi,'-DANDROID_PLATFORM=android-21','-DANDROID_STL=c++_shared']
            run(codec_command)
            if ndk and abi in ('armeabi-v7a','x86'):
                # WavPack's cached large-file probe leaks into Opusfile's config.
                # API 21 LP32 has no 64-bit stdio entry points (introduced in 24).
                # Mixer X uses Opusfile's 64-bit RW callbacks, not its FILE helper.
                header=directory/'codecs-build/libopusfile/config/config.h'
                header.write_text(header.read_text().replace('#define _FILE_OFFSET_BITS 64','#define _FILE_OFFSET_BITS 32').replace('/* #undef _FILE_OFFSET_BITS */','#define _FILE_OFFSET_BITS 32'))
            run([cmake,'--build',directory/'codecs-build','--parallel',str(jobs)])
            run([cmake,'--install',directory/'codecs-build'])
            command+=['-DOFE_AUDIOCODECS_ROOT='+str(stage),'-DOFE_AUDIOCODECS_SOURCE='+str(sources['AudioCodecs'])]
        run(command);run([cmake,'--build',directory,'--parallel',str(jobs)])
        builds[abi]=directory
    return builds

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ndk',type=Path)
    parser.add_argument('--cmake',type=Path,default=Path(shutil.which('cmake') or 'cmake'))
    parser.add_argument('--sources',type=Path,default=PORT/'out/sources')
    parser.add_argument('--build-root',type=Path,default=PORT/'out/build')
    parser.add_argument('--abi',action='append',choices=ABIS)
    parser.add_argument('--host',action='store_true')
    parser.add_argument('--prepare-only',action='store_true')
    parser.add_argument('--jobs',type=int,default=min(os.cpu_count() or 2,6))
    args=parser.parse_args()
    if not args.host and not args.ndk and not args.prepare_only: parser.error('--ndk is required unless --host is selected')
    sources=prepare(args.sources.resolve())
    if args.prepare_only:
        print(json.dumps({name:str(path) for name,path in sources.items()},indent=2));sys.exit(0)
    build(sources,args.build_root.resolve(),args.cmake.resolve(),None if args.host else args.ndk.resolve(),
        ['host'] if args.host else args.abi or ABIS,args.jobs,args.host)
