#!/usr/bin/env python3
"""Audit real Android binaries and package ownership; not an APK/device test."""
import importlib.util
import json
from pathlib import Path
import subprocess
import tempfile
import zipfile

repo=Path(__file__).resolve().parents[4]
root=repo/'Extensions/Android_Gradle/cmake/Encryption'
spec=importlib.util.spec_from_file_location('common_encryption',root/'android_runtime.py')
common=importlib.util.module_from_spec(spec);spec.loader.exec_module(common)
tools=json.loads((repo/'work/fini-android/toolchain.json').read_text())
llvm=next((Path(tools['ndk'])/'toolchains/llvm/prebuilt').iterdir())/'bin'
fini=repo/'Extensions/Fini/ToInstall/Files/Data/Runtime/Android/Fini.zip'
archive=root/'ToInstall/OFEEncryption.zip'

def inspect(tool,*args):
    return subprocess.check_output([str(llvm/tool),*map(str,args)],text=True)

def check(value,message):
    if not value:raise AssertionError(message)

with zipfile.ZipFile(fini) as plugin,zipfile.ZipFile(archive) as shared:
    names=plugin.namelist();shared_names=shared.namelist()
    check(not any(name.endswith(('libofe_encryption.so','libc++_shared.so','libcrypto.a','libcrypto.so')) for name in names),'Plugin contains common runtime binaries')
    check(not any('/Encryption/' in name and name.endswith(('CAPI.cpp','Core.h','KeyGen.h')) for name in names),'Plugin contains a private encryption implementation')
    check(json.loads(plugin.read('Fini-manifest.json'))['requires']==['OFEEncryption/1'],'Missing runtime dependency')
    for abi in common.ABIS:
        client=repo/'work/fini-android'/abi/'libfini.so'
        library=repo/'work/mobile-encryption/android'/abi/'libofe_encryption.so'
        common.validate_elf(client,abi);common.validate_elf(library,abi)
        dynamic=inspect('llvm-readelf','-d',client)
        check('libofe_encryption.so' in dynamic,'Client does not reference common SONAME: '+abi)
        defined=inspect('llvm-nm','-D','--defined-only',client)
        check('EVP_' not in defined and 'Encryption_EncryptBuffer' not in defined,'Crypto embedded in client: '+abi)
        exported=inspect('llvm-nm','-D','--defined-only',library)
        check('Encryption_EncryptBuffer' in exported and 'Encryption_DeriveKeyAndIv' in exported,'Missing shared C ABI: '+abi)
        check('EVP_' not in exported,'EVP leaked from shared ABI: '+abi)
        key=f'app/src/main/jniLibs/{abi}/libofe_encryption.so'
        check(shared.read(key)==library.read_bytes(),'Packaged shared runtime differs: '+abi)
        key=f'app/src/main/jniLibs/{abi}/libfini.so'
        check(plugin.read(key)==client.read_bytes(),'Packaged client differs: '+abi)
    manifest=json.loads(shared.read('OFEEncryption-manifest.json'))
    check(manifest['abiVersion']==1 and len(manifest['abis'])==4,'Common runtime ABI manifest')

with tempfile.TemporaryDirectory(prefix='fini-shared-audit-',dir=repo/'work') as temp:
    project=Path(temp);(project/'app').mkdir()
    common.install_once(archive,project)
    common.install_once(archive,project)
    files=list((project/'app/src/main/jniLibs').rglob('*.so'))
    check(len(files)==8,'Expected one encryption and one STL per ABI')
    conflict=project/'app/src/main/jniLibs/arm64-v8a/libofe_encryption.so';conflict.write_bytes(b'conflicting runtime')
    try:common.install_once(archive,project)
    except RuntimeError:pass
    else:raise AssertionError('Conflicting common runtime was silently overwritten')
    check(conflict.read_bytes()==b'conflicting runtime','Existing runtime was overwritten')
print('PASS: four real ABI builds, 64-bit 16 KiB alignment, common SONAME/C exports, no crypto in client, separate packages and one-time staging')
