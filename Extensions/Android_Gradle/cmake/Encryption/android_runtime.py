"""Shared Android encryption build/package/install functions for any plugin.
Consumers link libofe_encryption.so; common binaries never belong in plugin ZIPs.
"""
import hashlib,json,os,platform,shutil,struct,zipfile
from pathlib import Path
EXT=Path(__file__).resolve().parents[3]
ABIS={'armeabi-v7a':(1,40,'arm-linux-androideabi'),'arm64-v8a':(2,183,'aarch64-linux-android'),'x86':(1,3,'i686-linux-android'),'x86_64':(2,62,'x86_64-linux-android')}
def validate_elf(path,abi):
    data=Path(path).read_bytes();bits,machine,_=ABIS[abi]
    if len(data)<64 or data[:4]!=b'\x7fELF' or data[4]!=bits or data[5]!=1 or struct.unpack_from('<H',data,18)[0]!=machine:
        raise RuntimeError(f'Wrong ABI or invalid ELF: {path} ({abi})')
    if bits==2:off=struct.unpack_from('<Q',data,32)[0];size,count=struct.unpack_from('<HH',data,54);alignment=48;fmt='<Q'
    else:off=struct.unpack_from('<I',data,28)[0];size,count=struct.unpack_from('<HH',data,42);alignment=28;fmt='<I'
    loads=0
    for i in range(count):
        p=off+i*size
        if p+size>len(data):raise RuntimeError(f'Truncated ELF: {path}')
        if struct.unpack_from('<I',data,p)[0]==1:
            loads+=1
            required_alignment=16384 if bits==2 else 4096
            if struct.unpack_from(fmt,data,p+alignment)[0]<required_alignment:raise RuntimeError(f'ELF alignment is below {required_alignment} bytes: {path}')
    if not loads:raise RuntimeError(f'ELF has no load segments: {path}')


def publish(abi_dirs,licenses):
    destination=Path(__file__).resolve().parent/'ToInstall/OFEEncryption.zip'
    destination.parent.mkdir(parents=True,exist_ok=True)
    payload=dict(licenses)
    encryption=EXT/'_DeLib/Encryption'
    sources=[encryption/name for name in ('CAPI.h','CAPI.cpp','Core.h','KeyGen.h')]
    sources += [encryption.parent/'UnicodeConversion.h',Path(__file__).resolve(),
        EXT/'Android_Gradle/app/src/main/cpp/Encryption/CMakeLists.txt',
        Path(__file__).with_name('README.md'),
        EXT/'_Script/Encryption/prepare_openssl.py',EXT/'_Script/Encryption/README.md']
    for path in sources:
        payload['source/Extensions/'+path.relative_to(EXT).as_posix()]=path.read_bytes()
    for abi,directory in abi_dirs.items():
        for name in ('libofe_encryption.so','libc++_shared.so'):
            path=directory/name;validate_elf(path,abi);payload[f'app/src/main/jniLibs/{abi}/{name}']=path.read_bytes()
    payload['OFEEncryption-manifest.json']=json.dumps({'runtime':'OFEEncryption','abiVersion':1,'abis':list(abi_dirs),'sha256':{name:hashlib.sha256(data).hexdigest() for name,data in payload.items()}},indent=2).encode()
    temporary=destination.with_suffix('.partial')
    try:
        with zipfile.ZipFile(temporary,'w',zipfile.ZIP_DEFLATED) as archive:
            for name,data in payload.items():archive.writestr(name,data)
        os.replace(temporary,destination)
    finally:temporary.unlink(missing_ok=True)
    print('Common encryption runtime:',destination)
    return destination

def install_once(archive,project):
    if not (project/'app').is_dir():raise RuntimeError('Not an exported Android project: '+str(project))
    operations=[]
    with zipfile.ZipFile(archive) as zipped:
        manifest=json.loads(zipped.read('OFEEncryption-manifest.json'))
        if manifest.get('runtime')!='OFEEncryption' or manifest.get('abiVersion')!=1:
            raise RuntimeError('Unsupported shared encryption runtime package')
        for name in zipped.namelist():
            if not name.startswith('app/src/main/jniLibs/'):continue
            parts=Path(name).parts
            if len(parts)!=6 or parts[4] not in ABIS or parts[5] not in ('libofe_encryption.so','libc++_shared.so'):
                raise RuntimeError('Unexpected common runtime path: '+name)
            data=zipped.read(name)
            if hashlib.sha256(data).hexdigest()!=manifest['sha256'].get(name):
                raise RuntimeError('Common runtime hash mismatch: '+name)
            destination=project/name
            if destination.exists() and destination.read_bytes()!=data:
                raise RuntimeError('Conflicting shared runtime/STL in exported app: '+str(destination))
            operations.append((destination,data))
    # Check every conflict/hash before changing the exported project.
    for destination,data in operations:
        destination.parent.mkdir(parents=True,exist_ok=True)
        if not destination.exists():destination.write_bytes(data)


def build(abis,root,openssl,ndk,cmake,ninja,api,run):
    root=Path(root).resolve();result={};source=EXT/'Android_Gradle/app/src/main/cpp/Encryption'
    host={'Windows':'windows-x86_64','Linux':'linux-x86_64','Darwin':'darwin-x86_64'}[platform.system()]
    for abi in abis:
        stage=root/abi;directory=stage/'native'
        run([cmake,'-S',source,'-B',directory,'-G','Ninja',f'-DCMAKE_MAKE_PROGRAM={ninja}',f'-DCMAKE_TOOLCHAIN_FILE={ndk}/build/cmake/android.toolchain.cmake',f'-DANDROID_ABI={abi}',f'-DANDROID_PLATFORM=android-{api}','-DANDROID_STL=c++_shared','-DCMAKE_BUILD_TYPE=Release',f'-DOFE_OPENSSL_ROOT={(openssl/abi).resolve()}'])
        run([cmake,'--build',directory,'--parallel'])
        stl=ndk/'toolchains/llvm/prebuilt'/host/'sysroot/usr/lib'/ABIS[abi][2]/'libc++_shared.so'
        shutil.copy2(directory/'libofe_encryption.so',stage/'libofe_encryption.so')
        shutil.copy2(stl,stage/'libc++_shared.so');result[abi]=stage
    return result

if __name__=='__main__':
    import argparse
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive',type=Path,required=True);parser.add_argument('--project',type=Path,required=True)
    args=parser.parse_args();install_once(args.archive,args.project)
