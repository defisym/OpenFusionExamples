#!/usr/bin/env python3
"""Build the Windows-selected OpenSSL release for Android, with source verification."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tarfile
import urllib.request

VERSION_FILE = Path(__file__).resolve().parents[2]/'_3rdLib/Encrypt-build/EncryptVersion.cmake'
if not VERSION_FILE.is_file(): VERSION_FILE=Path(__file__).with_name('EncryptVersion.cmake')
def settings(path=VERSION_FILE):
    text=path.read_text()
    def value(name):
        found=re.search(r'^set\('+name+r'\s+"([^"\n]+)"\)',text,re.M)
        if not found: raise ValueError('Missing '+name+' in '+str(path))
        return found.group(1)
    version,commit=value('OFE_ENCRYPT_OPENSSL_VERSION'),value('OFE_ENCRYPT_VCPKG_COMMIT')
    if not re.fullmatch(r'\d+(?:\.\d+)+',version) or not re.fullmatch(r'[0-9a-f]{40}',commit):raise ValueError('Invalid OpenSSL version pin')
    return version,commit
VERSION,COMMIT=settings()
ABIS={'arm64-v8a':'android-arm64','armeabi-v7a':'android-arm','x86':'android-x86','x86_64':'android-x86_64'}

def matches(root,abis,api=21):
    try:
        data=json.loads((root/'manifest.json').read_text())
        return data['version']==VERSION and data['vcpkg_commit']==COMMIT and data['api']==api and all(
            abi in data['abis'] and (root/abi/'lib/libcrypto.a').is_file()
            and (root/abi/'include/openssl/evp.h').is_file() for abi in abis)
    except (OSError,KeyError,ValueError,TypeError):return False

def download(url,path):
    temporary=path.with_suffix(path.suffix+'.part')
    with urllib.request.urlopen(url,timeout=120) as response,temporary.open('wb') as output:shutil.copyfileobj(response,output)
    temporary.replace(path)

def prepare(cache):
    cache.mkdir(parents=True,exist_ok=True)
    port=cache/('openssl-port-'+COMMIT+'.cmake')
    if not port.exists():download('https://raw.githubusercontent.com/microsoft/vcpkg/'+COMMIT+'/ports/openssl/portfile.cmake',port)
    text=port.read_text()
    found=re.search(r'vcpkg_from_github\([\s\S]*?REPO\s+openssl/openssl[\s\S]*?SHA512\s+([0-9a-f]{128})',text)
    if not found:raise ValueError('Pinned OpenSSL port has no source SHA512')
    sha=found.group(1)
    archive=cache/('openssl-'+VERSION+'.tar.gz')
    if not archive.exists() or hashlib.sha512(archive.read_bytes()).hexdigest()!=sha:
        download('https://github.com/openssl/openssl/archive/refs/tags/openssl-'+VERSION+'.tar.gz',archive)
    if hashlib.sha512(archive.read_bytes()).hexdigest()!=sha:raise ValueError('OpenSSL source SHA512 mismatch')
    source=cache/('openssl-openssl-'+VERSION)
    if not source.exists():
        with tarfile.open(archive) as tar:tar.extractall(cache,filter='data')
    if not (source/'Configure').is_file():raise ValueError('Missing verified OpenSSL Configure')
    return source,archive,sha,port

def build(ndk,output,cache,abis,api,jobs):
    if os.name=='nt':raise RuntimeError('Prepare Android OpenSSL under Linux/WSL using its Linux NDK, then use that SDK with the Windows publisher')
    source,archive,sha,port=prepare(cache)
    toolchain=next((ndk/'toolchains/llvm/prebuilt').iterdir())/'bin'
    environment=dict(os.environ,ANDROID_NDK_ROOT=str(ndk),PATH=str(toolchain)+os.pathsep+os.environ['PATH'])
    previous=[]
    if matches(output,[],api):previous=json.loads((output/'manifest.json').read_text())['abis']
    for abi in abis:
        prefix=output/abi
        if matches(output,[abi],api):continue
        work=cache/('build-'+abi);work.mkdir(exist_ok=True)
        # Out-of-source OpenSSL builds keep the verified source immutable.
        command=['perl',source/'Configure',ABIS[abi],'-D__ANDROID_API__='+str(api),
            'no-shared','no-module','no-tests','no-apps','no-docs','no-engine','no-legacy','-fPIC',
            '--prefix='+str(prefix),'--openssldir='+str(prefix/'ssl')]
        subprocess.run(list(map(str,command)),cwd=work,env=environment,check=True)
        subprocess.run(['make','-j'+str(jobs),'build_libs'],cwd=work,env=environment,check=True)
        subprocess.run(['make','install_dev'],cwd=work,env=environment,check=True)
        previous.append(abi)
    output.mkdir(parents=True,exist_ok=True)
    shutil.copy2(archive,output/archive.name);shutil.copy2(port,output/port.name)
    shutil.copy2(source/'LICENSE.txt',output/'LICENSE.txt')
    data=dict(version=VERSION,vcpkg_commit=COMMIT,source_sha512=sha,api=api,abis=sorted(set(previous)))
    (output/'manifest.json').write_text(json.dumps(data,indent=2)+'\n')

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ndk',type=Path,required=True);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--cache',type=Path);parser.add_argument('--abi',choices=ABIS,action='append')
    parser.add_argument('--api',type=int,default=21);parser.add_argument('--jobs',type=int,default=4)
    args=parser.parse_args();output=args.output.resolve()
    build(args.ndk.resolve(),output,(args.cache or output/'.build').resolve(),args.abi or list(ABIS),args.api,args.jobs)
