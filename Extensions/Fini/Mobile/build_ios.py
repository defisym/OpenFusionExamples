#!/usr/bin/env python3
"""Build owned Objective-C++ adapter + reused C++ core as an iOS XCFramework.
Requires macOS, Xcode, a licensed exporter Runtime, and target OpenSSL prefixes.
Does not redistribute or modify the exporter SDK. iOS has no JNI.
"""
import argparse,subprocess,shutil,os,sys
from pathlib import Path

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--runtime',type=Path,required=True,help='Exporter project root containing Classes/')
    p.add_argument('--openssl',type=Path,required=True,help='Prefixes: iphoneos-arm64, iphonesimulator-arm64, iphonesimulator-x86_64')
    p.add_argument('--encryption-framework',type=Path,help='Use an already built shared OFEEncryption.xcframework')
    p.add_argument('--prepare-openssl',action='store_true',help='Build missing iOS OpenSSL slices from pinned source')
    p.add_argument('--output',type=Path,default=Path('work/fini-ios'));a=p.parse_args()
    if not shutil.which('xcrun') or not shutil.which('xcodebuild'):raise SystemExit('Fini iOS requires macOS with Xcode/xcrun; no iOS binary was produced.')
    root=Path(__file__).resolve().parents[1];classes=a.runtime.resolve()/'Classes'
    if not (classes/'Extensions/CRunExtension.h').is_file():raise SystemExit('Missing licensed exporter Runtime headers')
    # Shared encryption is built once, outside the plugin archive.
    common_output=root.parents[1]/'work/mobile-encryption/ios'
    if a.encryption_framework:
        if not (a.encryption_framework/'Info.plist').is_file():raise SystemExit('Invalid shared encryption XCFramework')
    else:
        command=[sys.executable,root.parent/'_DeLib/Encryption/Mobile/build_ios.py','--openssl',a.openssl,'--output',common_output]
        if a.prepare_openssl:command.append('--prepare-openssl')
        subprocess.run(list(map(str,command)),check=True)
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    includes=[f'-I{classes}']+[f'-I{d}' for d in classes.iterdir() if d.is_dir()]+[f'-I{root.parent}/_DeLib']
    source=root.parent/'iOS/Classes/Extensions/CRunFini.mm'
    headers=out/'Headers';headers.mkdir(exist_ok=True);shutil.copy2(source.with_suffix('.h'),headers/'CRunFini.h')
    with (out/'publisher.log').open('w') as log:
        def run(cmd):
            print('+',' '.join(map(str,cmd)),flush=True);subprocess.run(list(map(str,cmd)),stdout=log,stderr=subprocess.STDOUT,check=True)
        slices={}
        for sdk,arch in [('iphoneos','arm64'),('iphonesimulator','arm64'),('iphonesimulator','x86_64')]:
            directory=out/f'{sdk}-{arch}';directory.mkdir(exist_ok=True)
            sysroot=subprocess.check_output(['xcrun','--sdk',sdk,'--show-sdk-path'],text=True).strip()
            target=f'{arch}-apple-ios17.0'+('-simulator' if sdk=='iphonesimulator' else '')
            run(['xcrun','--sdk',sdk,'clang++','-std=c++20','-O2','-fno-objc-arc','-fobjc-exceptions','-fexceptions','-isysroot',sysroot,'-target',target,*includes,'-c',source,'-o',directory/'Fini.o'])
            run(['xcrun','libtool','-static','-o',directory/'libFini.a',directory/'Fini.o'])
            slices[(sdk,arch)]=directory/'libFini.a'
        simulator=out/'libFini-simulator.a'
        run(['xcrun','lipo','-create',slices[('iphonesimulator','arm64')],slices[('iphonesimulator','x86_64')],'-output',simulator])
        final=out/'Fini.xcframework';pending=out/'pending.xcframework'
        if pending.exists():shutil.rmtree(pending)
        run(['xcodebuild','-create-xcframework','-library',slices[('iphoneos','arm64')],'-headers',headers,'-library',simulator,'-headers',headers,'-output',pending])
        if final.exists():shutil.rmtree(final)
        pending.rename(final)
        sys.path.insert(0,str(root.parent/'_DeLib/Encryption/Mobile'))
        from apple_artifacts import stage
        sources=['Extensions/iOS/Classes/Extensions/CRunFini.mm','Extensions/iOS/Classes/Extensions/CRunFini.h','Extensions/Fini/Mobile/Core.h','Extensions/Fini/IniCore.h','Extensions/Fini/Mobile/build_ios.py','Extensions/_DeLib/Encryption/CAPI.h','Extensions/_DeLib/StrNum.h','Extensions/_DeLib/UnicodeConversion.h','Extensions/_3rdLib/Base64.h','Extensions/_3rdLib/Compress/zlibInterface.h','Extensions/_3rdLib/simpleini/SimpleIni.h','Extensions/_3rdLib/nlohmann/single_include/nlohmann/json.hpp','Extensions/_DeLib/StringUtilities.h','Extensions/_DeLib/StringTraits.h','Extensions/_DeLib/ArithmeticTraits.h','Extensions/_DeLib/PathUtilities.h','Extensions/_DeLib/GeneralMacros.h','Extensions/_3rdLib/Compress/zlib/zlib.h','Extensions/_3rdLib/Compress/zlib/zconf.h']
        stage(final,'Fini',sources,root/'iOS/Prebuilt',requires=['OFEEncryption/1'])
        print('Built',final,'Link the common OFEEncryption.xcframework once, plus libc++, Foundation and libz. No crypto implementation is included in Fini.')
if __name__=='__main__':main()
