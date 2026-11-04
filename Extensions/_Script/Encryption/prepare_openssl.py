#!/usr/bin/env python3
"""Prepare target OpenSSL for the existing encryption core, never a new cipher.
Source version/hash match the repository's pinned vcpkg OpenSSL port.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import tarfile
import urllib.request

VERSION='3.6.5'
SOURCE_URL='https://github.com/openssl/openssl/archive/refs/tags/openssl-'+VERSION+'.tar.gz'
SOURCE_SHA512='40f0b346a9a5cf4cb37f1ecebdc74750c39a95783710061bfac58964883b0831543ecee0aee9f747ea093781b13c58a543b0b935d222519e3cb19648d77a0276'
ANDROID={'armeabi-v7a':('android-arm','armv7a-linux-androideabi'),'arm64-v8a':('android-arm64','aarch64-linux-android'),'x86':('android-x86','i686-linux-android'),'x86_64':('android-x86_64','x86_64-linux-android')}
IOS={'iphoneos-arm64':('iphoneos','arm64','ios64-xcrun'),'iphonesimulator-arm64':('iphonesimulator','arm64','iossimulator-arm64-xcrun'),'iphonesimulator-x86_64':('iphonesimulator','x86_64','iossimulator-x86_64-xcrun')}

def source_tree(work,archive=None):
    work.mkdir(parents=True,exist_ok=True)
    archive=Path(archive) if archive else work/f'openssl-{VERSION}.tar.gz'
    if not archive.is_file():
        temporary=archive.with_suffix('.download')
        try:
            with urllib.request.urlopen(SOURCE_URL,timeout=30) as src,temporary.open('wb') as out:
                shutil.copyfileobj(src,out)
            if hashlib.sha512(temporary.read_bytes()).hexdigest()!=SOURCE_SHA512:
                raise RuntimeError('Downloaded OpenSSL source SHA512 mismatch')
            temporary.replace(archive)
        finally:
            temporary.unlink(missing_ok=True)
    if hashlib.sha512(archive.read_bytes()).hexdigest()!=SOURCE_SHA512:
        raise RuntimeError('OpenSSL source SHA512 mismatch: '+str(archive))
    source=work/f'openssl-{VERSION}'
    marker=source/'.fini-source-sha512'
    if not marker.is_file() or marker.read_text()!=SOURCE_SHA512:
        if source.exists():shutil.rmtree(source)
        # Python's data filter rejects escaping links and special files.
        with tarfile.open(archive) as tar:
            names={member.name.split('/')[0] for member in tar.getmembers()}
            if len(names)!=1:raise RuntimeError('Unexpected OpenSSL source archive layout')
            top=work/next(iter(names))
            for member in tar.getmembers():
                if Path(member.name).is_absolute() or '..' in Path(member.name).parts:
                    raise RuntimeError('Unsafe OpenSSL archive path')
                if not member.isdir() and not member.isfile():
                    raise RuntimeError('Unexpected link/special file in OpenSSL source')
            if top!=source and top.exists():shutil.rmtree(top)
            tar.extractall(work)
        if top!=source:top.rename(source)
        marker.write_text(SOURCE_SHA512)
    return source


def prepare(kind,output,work,ndk=None,abis=None,api=26,archive=None):
    if os.name=='nt':
        raise RuntimeError('For native Windows publishing supply prebuilt ABI OpenSSL via --openssl; automatic source preparation currently requires a Unix build host (Linux/macOS).')
    if not shutil.which('perl') or not shutil.which('make'):
        raise RuntimeError('OpenSSL source build requires Perl and GNU make (Apple make is supported).')
    output=Path(output).resolve();work=Path(work).resolve();source=source_tree(work,archive)
    output.mkdir(parents=True,exist_ok=True);shutil.copy2(source/'LICENSE.txt',output/'LICENSE.txt')
    selected=list(abis or ANDROID) if kind=='android' else list(IOS)
    with (work/'openssl-build.log').open('a') as log:
        def run(cmd,cwd,environment):
            command=list(map(str,cmd));print('+',' '.join(command),flush=True)
            log.write('+ '+' '.join(command)+'\n');log.flush()
            subprocess.run(command,cwd=cwd,env=environment,stdout=log,stderr=subprocess.STDOUT,check=True)
        for name in selected:
            prefix=output/name;environment=os.environ.copy();options=['no-shared','no-module','no-tests','no-apps','no-docs','-fPIC','--libdir=lib','--prefix='+str(prefix)]
            if kind=='android':
                if ndk is None:raise RuntimeError('Android OpenSSL requires --ndk')
                ndk=Path(ndk).resolve();host='darwin-x86_64' if platform.system()=='Darwin' else 'linux-x86_64'
                tools=ndk/'toolchains/llvm/prebuilt'/host/'bin';target,triple=ANDROID[name]
                environment.update({'ANDROID_NDK_ROOT':str(ndk),'PATH':str(tools)+os.pathsep+environment['PATH'],'CC':str(tools/(triple+str(api)+'-clang')),'AR':str(tools/'llvm-ar'),'RANLIB':str(tools/'llvm-ranlib')})
                options+=['-D__ANDROID_API__='+str(api)]
                identity=str(ndk)+'|'+str(api)
            else:
                if platform.system()!='Darwin':raise RuntimeError('iOS OpenSSL preparation requires macOS/Xcode')
                sdk,arch,target=IOS[name]
                sysroot=subprocess.check_output(['xcrun','--sdk',sdk,'--show-sdk-path'],text=True).strip()
                compiler=subprocess.check_output(['xcrun','--sdk',sdk,'--find','clang'],text=True).strip()
                environment.update({'CC':compiler,'CROSS_TOP':str(Path(sysroot).parent.parent),'CROSS_SDK':Path(sysroot).name,'SDKROOT':sysroot})
                options+=['-isysroot',sysroot,'-arch',arch,'-mios-version-min=17.0' if sdk=='iphoneos' else '-mios-simulator-version-min=17.0']
                identity=sysroot+'|17.0'
            signature=json.dumps({'source_sha512':SOURCE_SHA512,'target':target,'options':options,'toolchain':identity},sort_keys=True)
            marker=prefix/'fini-openssl.json'
            if marker.is_file() and marker.read_text()==signature and (prefix/'lib/libcrypto.a').is_file() and (prefix/'include/openssl/evp.h').is_file():
                print('OpenSSL up to date:',name,flush=True);continue
            build=work/(kind+'-'+name);build.mkdir(parents=True,exist_ok=True)
            run(['perl',source/'Configure',target,*options],build,environment)
            run(['make','-j'+str(min(os.cpu_count() or 2,8))],build,environment)
            run(['make','install_dev'],build,environment)
            marker.write_text(signature)
    return output


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--platform',choices=['android','ios'],required=True);p.add_argument('--ndk',type=Path)
    p.add_argument('--abis',nargs='+',choices=ANDROID);p.add_argument('--api',type=int,default=26)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--work',type=Path,required=True);p.add_argument('--source-archive',type=Path)
    a=p.parse_args();prepare(a.platform,a.output,a.work,a.ndk,a.abis,a.api,a.source_archive)
if __name__=='__main__':main()
