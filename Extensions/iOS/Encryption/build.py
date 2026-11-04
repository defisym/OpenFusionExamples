#!/usr/bin/env python3
"""Build one shared OFEEncryption XCFramework for all Fusion plugins."""
import argparse
import os
from pathlib import Path
import plistlib
import re
import shutil
import subprocess
import sys


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--openssl',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--prepare-openssl',action='store_true')
    a=p.parse_args();root=Path(__file__).resolve().parent;extensions=root.parents[1];out=a.output.resolve()
    sys.path.insert(0,str(extensions/'_Script/Encryption'))
    sys.path.insert(0,str(root.parent/'Tools'))
    if not shutil.which('xcrun') or not shutil.which('xcodebuild'):raise SystemExit('Shared iOS encryption requires Xcode on macOS')
    prefixes=[('iphoneos','arm64'),('iphonesimulator','arm64'),('iphonesimulator','x86_64')]
    if any(not (a.openssl/f'{sdk}-{arch}/lib/libcrypto.a').is_file() for sdk,arch in prefixes):
        if not a.prepare_openssl:raise SystemExit('Missing target OpenSSL; use --prepare-openssl or supply prepared prefixes')
        from prepare_openssl import prepare
        prepare('ios',a.openssl,out/'openssl-build')
    out.mkdir(parents=True,exist_ok=True)
    # Export the stable C ABI only; keep EVP and C++ implementation symbols private.
    capi=extensions/'_DeLib/Encryption/CAPI.h';symbols=re.findall(r'ENCRYPTION_API\s+(?:\w+\s+)+(Encryption_\w+)\s*\(',capi.read_text())
    export_list=out/'exports.txt';export_list.write_text(''.join('_'+name+'\n' for name in sorted(set(symbols))))
    with (out/'publisher.log').open('w') as log:
        def run(cmd):
            print('+',' '.join(map(str,cmd)),flush=True);subprocess.run(list(map(str,cmd)),stdout=log,stderr=subprocess.STDOUT,check=True)
        frameworks={}
        for sdk,arch in prefixes:
            prefix=a.openssl.resolve()/f'{sdk}-{arch}';framework=out/f'{sdk}-{arch}/OFEEncryption.framework'
            headers=framework/'Headers';headers.mkdir(parents=True,exist_ok=True);shutil.copy2(capi,headers/'CAPI.h')
            modules=framework/'Modules';modules.mkdir(exist_ok=True)
            (modules/'module.modulemap').write_text('framework module OFEEncryption { umbrella header "CAPI.h" export * }\n')
            plist={'CFBundleExecutable':'OFEEncryption','CFBundleIdentifier':'org.openfusionexamples.encryption','CFBundleName':'OFEEncryption','CFBundlePackageType':'FMWK','CFBundleShortVersionString':'1.0','CFBundleVersion':'1','MinimumOSVersion':'17.0','CFBundleSupportedPlatforms':['iPhoneOS' if sdk=='iphoneos' else 'iPhoneSimulator']}
            (framework/'Info.plist').write_bytes(plistlib.dumps(plist))
            sysroot=subprocess.check_output(['xcrun','--sdk',sdk,'--show-sdk-path'],text=True).strip()
            target=f'{arch}-apple-ios17.0'+('-simulator' if sdk=='iphonesimulator' else '')
            run(['xcrun','--sdk',sdk,'clang++','-std=c++17','-O2','-fPIC','-fvisibility=hidden','-fvisibility-inlines-hidden','-DENCRYPTION_SHARED','-DENCRYPTION_BUILD','-D_FILE_OFFSET_BITS=64','-isysroot',sysroot,'-target',target,'-dynamiclib',f'-I{prefix}/include',capi.with_suffix('.cpp'),prefix/'lib/libcrypto.a','-Wl,-install_name,@rpath/OFEEncryption.framework/OFEEncryption','-Wl,-exported_symbols_list,'+str(export_list),'-o',framework/'OFEEncryption'])
            frameworks[(sdk,arch)]=framework
        sim=out/'simulator/OFEEncryption.framework'
        if sim.exists():shutil.rmtree(sim)
        shutil.copytree(frameworks[('iphonesimulator','arm64')],sim)
        run(['xcrun','lipo','-create',frameworks[('iphonesimulator','arm64')]/'OFEEncryption',frameworks[('iphonesimulator','x86_64')]/'OFEEncryption','-output',sim/'OFEEncryption'])
        pending=out/'pending.xcframework';final=out/'OFEEncryption.xcframework'
        if pending.exists():shutil.rmtree(pending)
        run(['xcodebuild','-create-xcframework','-framework',frameworks[('iphoneos','arm64')],'-framework',sim,'-output',pending])
        if final.exists():shutil.rmtree(final)
        pending.rename(final)
        if (a.openssl/'LICENSE.txt').is_file():shutil.copy2(a.openssl/'LICENSE.txt',out/'OpenSSL-LICENSE.txt')
        from apple_artifacts import stage,REPO
        sources=['Extensions/_DeLib/Encryption/'+name for name in ('CAPI.h','CAPI.cpp','Core.h','KeyGen.h')]+['Extensions/_DeLib/UnicodeConversion.h','Extensions/iOS/Encryption/build.py','Extensions/_Script/Encryption/prepare_openssl.py']
        licenses={'OpenSSL-LICENSE.txt':a.openssl/'LICENSE.txt'} if (a.openssl/'LICENSE.txt').is_file() else {}
        stage(final,'OFEEncryption',sources,root/'Prebuilt',extra_files=licenses)
        print('Shared runtime for all plugins:',final)

if __name__=='__main__':main()
