#!/usr/bin/env python3
"""Single-entry Fini Android publisher. Fusion exports use default Gradle templates."""
import argparse,hashlib,io,json,os,platform,shutil,struct,subprocess,tempfile,zipfile,sys,traceback
from pathlib import Path
REPO=Path(__file__).resolve().parents[2]
EXT=REPO/'Extensions';JAVA=EXT/'Android_Gradle/app/src/main/java'
sys.path.insert(0,str(Path(__file__).resolve().parent))
import FiniNativeTools
TOOLCHAIN=json.loads((EXT/'Android_Gradle/cmake/Fini/toolchain.json').read_text())
sys.path.insert(0,str(EXT/'_DeLib/Encryption/Mobile'))
import android_runtime as encryption_runtime
import apple_artifacts
import FiniIOS
ABIS=encryption_runtime.ABIS
validate_elf=encryption_runtime.validate_elf
OWNED_JAVA=['Extensions/CRunFini.java','_DeLib/FiniNative.java']
RULES='-keep class Extensions.CRunFini { *; }\n-keep class _DeLib.FiniNative { *; }\n'


def source_files():
    changed=['Fini/IniCore.h','Fini/Func.h','Fini/Main.cpp','_DeLib/Fini/Core.h','_DeLib/Fini/CMakeLists.txt','Android_Gradle/Fini-JNI.md','_DeLib/Fini/ACE.csv','Android_Gradle/cmake/Fini/toolchain.json','iOS/Fini/build.py','_DeLib/Encryption/Mobile/prepare_openssl.py','_DeLib/Encryption/Mobile/CMakeLists.txt','_DeLib/Encryption/Mobile/build_ios.py','_DeLib/Encryption/Mobile/android_runtime.py','_DeLib/Encryption/Mobile/README.md','_DeLib/Encryption/Mobile/apple_artifacts.py',
        '_3rdLib/Base64.h','_3rdLib/Compress/zlibInterface.h','_3rdLib/Compress/zlib/zlib.h','_3rdLib/Compress/zlib/zconf.h','_3rdLib/simpleini/SimpleIni.h','_3rdLib/nlohmann/single_include/nlohmann/json.hpp',
        '_DeLib/PathUtilities.h','_DeLib/GeneralMacros.h','_DeLib/StringTraits.h','_DeLib/StrNum.h','_DeLib/StringUtilities.h','_DeLib/ArithmeticTraits.h','_DeLib/UnicodeConversion.h','_DeLib/Encryption/Core.h','_DeLib/Encryption/KeyGen.h','_DeLib/Encryption/CAPI.h','_DeLib/Encryption/CAPI.cpp',
        'Android_Gradle/cmake/Fini/CMakeLists.txt','Android_Gradle/app/src/main/cpp/FiniJNI.cpp',
        'iOS/Fini/Runtime/CRunFini.h','iOS/Fini/Runtime/CRunFini.mm','_Script/Fini.py','_Script/Fini.bat','_Script/FiniNativeTools.py','_Script/FiniIOS.py','iOS/Tools/make_ext.py','iOS/Tools/xcode_project.py']
    return [EXT/p for p in changed]+[JAVA/p for p in OWNED_JAVA]+[p for p in (EXT/'Android_Gradle/tests/fini').glob('*') if p.is_file() and p.suffix in ('.py','.cpp','.java')]+[p for p in (EXT/'iOS/Fini').rglob('*') if p.is_file() and 'xcuserdata' not in p.parts and '__pycache__' not in p.parts and p.name not in ('Local.xcconfig','.gitignore') and not any(name in p.parts for name in ('Prebuilt','Runtime','Dependencies','SDK')) and p.suffix in ('.py','.md','.xcconfig','.pbxproj','.xcscheme','.xcworkspacedata')]

def package(destination,abi_dirs,include_sources=True,licenses=None):
    payload={f'app/src/main/java/{p}':(JAVA/p).read_bytes() for p in OWNED_JAVA}
    for abi,directory in abi_dirs.items():
        for name in ('libfini.so',):
            path=directory/name;validate_elf(path,abi);payload[f'app/src/main/jniLibs/{abi}/{name}']=path.read_bytes()
    rules=io.BytesIO()
    with zipfile.ZipFile(rules,'w',zipfile.ZIP_DEFLATED) as z:z.writestr('META-INF/proguard/Fini.pro',RULES)
    payload['app/libs/Fini-JNI-rules.jar']=rules.getvalue()
    if include_sources:
        for path in source_files():
            # Runtime packages carry only the shared encryption API, not its implementation.
            if abi_dirs and (EXT/'_DeLib/Encryption') in path.parents and path.name!='CAPI.h':continue
            payload['source/'+str(path.relative_to(REPO))]=path.read_bytes()
        # Owned code and third-party license material only, never the exporter SDK.
        for relative in ('LICENSE','LICENSE.md','LICENSE.txt'):
            if (REPO/relative).is_file():payload['source/'+relative]=(REPO/relative).read_bytes()
        for folder in ('_3rdLib/simpleini','_3rdLib/nlohmann','_3rdLib/Compress/zlib'):
            for path in (EXT/folder).glob('*'):
                if path.is_file() and ('license' in path.name.lower() or path.name=='README'):
                    payload['source/Extensions/'+str(path.relative_to(EXT))]=path.read_bytes()
    payload.update(licenses or {})
    payload['Fini-manifest.json']=json.dumps({'extension':'Fini','requires':['OFEEncryption/1'],'artifactKind':'runtime' if abi_dirs else 'source-review','abis':list(abi_dirs),'nativeAPI':26,'format':'Win32 assets; UTF-8 INI; AES-128-CBC legacy KeyGen',
        'sha256':{name:hashlib.sha256(data).hexdigest() for name,data in payload.items()}},indent=2).encode()
    destination.parent.mkdir(parents=True,exist_ok=True)
    fd,temporary=tempfile.mkstemp(prefix='Fini-',suffix='.zip',dir=destination.parent);os.close(fd)
    try:
        with zipfile.ZipFile(temporary,'w',zipfile.ZIP_DEFLATED) as z:
            for name,data in sorted(payload.items()):z.writestr(name,data)
        os.replace(temporary,destination)
    finally:
        if Path(temporary).exists():Path(temporary).unlink()


def sync_apple(fusion=None):
    plugin=apple_artifacts.validate(EXT/'iOS/Fini/Prebuilt','Fini')
    common=apple_artifacts.validate(EXT/'_DeLib/Encryption/Mobile/Prebuilt/iOS','OFEEncryption')
    if plugin and not common:raise ValueError('Fini Apple handoff requires the separately committed common encryption artifact')
    if not plugin and not common:
        print('Apple libraries not staged yet; build/push on Mac, then pull and rebuild on Windows.')
        return []
    if fusion and not (fusion/'Data/Runtime/iPhone').is_dir():raise ValueError('Fusion iPhone runtime directory is missing; set FusionRoot to the installed Fusion directory')
    scratch=REPO/'work';scratch.mkdir(exist_ok=True)
    prepared=[]
    # Validate/generate every result before replacing any known-good output.
    with tempfile.TemporaryDirectory(prefix='fini-apple-package-',dir=scratch) as temp:
        temp=Path(temp)
        if common:
            path=EXT/'_DeLib/Encryption/Mobile/ToInstall/OFEEncryption-iOS.zip'
            staged=temp/path.name;apple_artifacts.package(common,staged);prepared.append((path,staged.read_bytes()))
        if plugin:
            path=EXT/'Fini/ToInstall/Files/Data/Runtime/iPhone/Fini-iOS.zip'
            staged=temp/path.name
            apple_artifacts.package(plugin,staged,{'FiniConsumer.xcconfig':EXT/'iOS/Fini/Config/FiniConsumer.xcconfig','README.md':EXT/'iOS/Fini/README.md'})
            prepared.append((path,staged.read_bytes()))
            extension=path.with_name('Fini.ext');staged=temp/extension.name
            FiniIOS.write(plugin,staged);prepared.append((extension,staged.read_bytes()))
    for path,data in prepared:
        apple_artifacts.atomic_write(path,data)
        if fusion:apple_artifacts.atomic_write(fusion/'Data/Runtime/iPhone'/path.name,data)
        print('Updated pulled Apple package:',path)
    return [path for path,_ in prepared]


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--sdk',type=Path);p.add_argument('--ndk',type=Path);p.add_argument('--openssl',type=Path,help='Target OpenSSL prefixes: <root>/<ABI>/{include,lib/libcrypto.a}')
    p.add_argument('--android-project',type=Path,help='Stage the common runtime once in this exported Android project; does not alter its Gradle files')
    p.add_argument('--openssl-license',type=Path,help='OpenSSL distribution license if not stored under its prefix')
    p.add_argument('--cmake');p.add_argument('--ninja');p.add_argument('--abis',nargs='+',choices=ABIS,default=list(ABIS));p.add_argument('--fusion',type=Path)
    p.add_argument('--source-bundle',type=Path,help='Write a review source bundle; not an installable runtime')
    p.add_argument('--ios-only',action='store_true',help='Package pulled Mac artifacts and archive/install without Android tools or Xcode')
    p.add_argument('--prepare-only',action='store_true',help='Discover/prepare SDK, NDK, CMake and Ninja without compiling or packaging')
    p.add_argument('--no-install-tools',action='store_true',help='Only use installed Android tools')
    p.add_argument('--build',type=Path,default=REPO/'work/fini-android');a=p.parse_args()
    if a.source_bundle:
        package(a.source_bundle,{});print('Source review bundle:',a.source_bundle);return
    fusion=a.fusion or Path(os.environ.get('FusionRoot',r'C:\Steam\steamapps\common\Clickteam Fusion 2.5'))
    if not fusion.is_dir():fusion=None
    sync_apple(fusion)
    if a.ios_only:
        archive_release();return
    a.build=a.build.resolve();a.build.mkdir(parents=True,exist_ok=True)
    log_path=a.build/'publisher.log'
    def run(cmd):
        command=list(map(str,cmd));print('+',' '.join(command),flush=True)
        with log_path.open('a',encoding='utf-8') as log:
            log.write('+ '+' '.join(command)+'\n');log.flush()
            environment=os.environ.copy()
            environment.setdefault('ANDROID_USER_HOME',str(a.build/'android-user'))
            Path(environment['ANDROID_USER_HOME']).mkdir(parents=True,exist_ok=True)
            result=subprocess.run(command,stdin=subprocess.DEVNULL,stdout=log,stderr=subprocess.STDOUT,env=environment)
        if result.returncode:raise RuntimeError(f'Native tool/build command failed ({result.returncode}); see {log_path}')
    log_path.write_text('Fini publisher: native tool preparation\n',encoding='utf-8')
    try:
        tools=FiniNativeTools.prepare(REPO,TOOLCHAIN,run,sdk=a.sdk,ndk=a.ndk,cmake=a.cmake,ninja=a.ninja,install=not a.no_install_tools)
    except Exception:
        with log_path.open('a',encoding='utf-8') as log:traceback.print_exc(file=log)
        raise
    sdk,ndk,cmake,ninja=(tools[k] for k in ('sdk','ndk','cmake','ninja'))
    (a.build/'toolchain.json').write_text(json.dumps({k:str(v) for k,v in tools.items()},indent=2))
    if a.prepare_only:
        print('Native tools ready:',a.build/'toolchain.json');return
    openssl=a.openssl or (Path(os.environ['FINI_OPENSSL_ROOT']) if os.environ.get('FINI_OPENSSL_ROOT') else None)
    if openssl is None:
        openssl=REPO/'work/mobile-encryption/openssl/android'
        run([sys.executable,EXT/'_DeLib/Encryption/Mobile/prepare_openssl.py','--platform','android','--ndk',ndk,'--abis',*a.abis,'--api',str(TOOLCHAIN['native_api']),'--output',openssl,'--work',REPO/'work/mobile-encryption/openssl-build'])
    license_path=a.openssl_license or next((candidate for candidate in [openssl/'LICENSE.txt',openssl/'LICENSE',openssl/'LICENSE.md'] if candidate.is_file()),None)
    if license_path is None:raise RuntimeError('OpenSSL license missing; pass --openssl-license for redistribution')
    licenses={'licenses/OpenSSL.txt':license_path.read_bytes()}
    ndk_notice=ndk/'NOTICE'
    if not ndk_notice.is_file():raise RuntimeError(f'Missing NDK redistribution notice: {ndk_notice}')
    licenses['licenses/NDK-NOTICE.txt']=ndk_notice.read_bytes()
    if (ndk/'NOTICE.toolchain').is_file():licenses['licenses/NDK-TOOLCHAIN-NOTICE.txt']=(ndk/'NOTICE.toolchain').read_bytes()
    host={'Windows':'windows-x86_64','Linux':'linux-x86_64','Darwin':'darwin-x86_64'}[platform.system()]
    common_stages=encryption_runtime.build(a.abis,REPO/'work/mobile-encryption/android',openssl,ndk,cmake,ninja,TOOLCHAIN['native_api'],run)
    abi_dirs={}
    for abi in a.abis:
        prefix=openssl/abi
        if not (prefix/'include/openssl/evp.h').is_file() or not (prefix/'lib/libcrypto.a').is_file():raise RuntimeError(f'Missing target OpenSSL: {prefix}')
        build=a.build/abi
        run([cmake,'-S',EXT/'Android_Gradle/cmake/Fini','-B',build,'-G','Ninja',f'-DCMAKE_MAKE_PROGRAM={ninja}',f'-DCMAKE_TOOLCHAIN_FILE={ndk}/build/cmake/android.toolchain.cmake',f'-DANDROID_ABI={abi}',f'-DANDROID_PLATFORM=android-{TOOLCHAIN["native_api"]}','-DANDROID_STL=c++_shared','-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON','-DCMAKE_BUILD_TYPE=Release',f'-DOFE_ENCRYPTION_LIBRARY={(common_stages[abi]/"libofe_encryption.so").resolve()}'])
        run([cmake,'--build',build,'--parallel'])
        stage=a.build/'stage'/abi;stage.mkdir(parents=True,exist_ok=True)
        shutil.copy2(build/'libfini.so',stage/'libfini.so')
        abi_dirs[abi]=stage
    destination=EXT/'Fini/ToInstall/Files/Data/Runtime/Android/Fini.zip'
    package(destination,abi_dirs)
    common=encryption_runtime.publish(common_stages,licenses)
    if a.android_project:encryption_runtime.install_once(common,a.android_project)
    if fusion:
        install=fusion/'Data/Runtime/Android/Fini.zip';install.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(destination,install)
    archive_release()
    print('Published:',destination)

def archive_release():
    archive=EXT/'Fini_Release.zip';temp=archive.with_suffix('.tmp')
    with zipfile.ZipFile(temp,'w',zipfile.ZIP_DEFLATED) as z:
        for path in (EXT/'Fini/ToInstall/Files').rglob('*'):
            if path.is_file():z.write(path,path.relative_to(EXT/'Fini/ToInstall/Files'))
    os.replace(temp,archive);print('Release archive:',archive)
if __name__=='__main__':
    try:main()
    except (RuntimeError,OSError,ValueError) as e:raise SystemExit('Fini publish failed: '+str(e))
