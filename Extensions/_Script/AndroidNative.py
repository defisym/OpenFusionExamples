"""Shared Android ABI checks and SDK/NDK discovery for native extension publishers."""
from pathlib import Path
import importlib.util
import re
import subprocess

ABIS = {'arm64-v8a': ('aarch64-linux-android', 'AArch64'),
        'armeabi-v7a': ('arm-linux-androideabi', 'ARM'),
        'x86': ('i686-linux-android', 'Intel 80386'), 'x86_64': ('x86_64-linux-android', 'Advanced Micro Devices X86-64')}

def check_elf(path, readelf, machine):
    result = subprocess.run([str(readelf), '-h', '-l', '-d', str(path)], check=True, capture_output=True, text=True).stdout
    match = re.search(r'Machine:\s*(.*)', result)
    if not match or match.group(1).strip() != machine: raise ValueError(f'Wrong ABI: {path}')
    loads = [line.split()[-1] for line in result.splitlines() if line.strip().startswith('LOAD ')]
    # Android's 16 KB page-size requirement applies to 64-bit ABIs. NDK's 32-bit libc++ uses 4 KB.
    alignment = 16384 if machine in ('AArch64', 'Advanced Micro Devices X86-64') else 4096
    if not loads or any(int(value, 16) < alignment for value in loads): raise ValueError(f'Invalid ELF load alignment ({alignment} required): {path}')
    return re.findall(r'\(NEEDED\).*?\[(.*?)\]', result)

def native_tools():
    # Reuse FTW's established SDK discovery/installation behavior.
    spec=importlib.util.spec_from_file_location('ofe_ftw_tools',Path(__file__).with_name('FindTheWay.py'))
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    return module.native_tools()


def copy_audio_sources(sources, source_cache, port, stage):
    """Identical shared runtime sources/notices in either standalone extension ZIP."""
    import hashlib
    import json
    import shutil
    guide=port.parents[1]/'Android_Gradle/Media-JNI.md'
    if guide.is_file():shutil.copy2(guide,stage/'Android-Media.md')
    shutil.copytree(port/'cpp', stage/'Sources/SDL_Mixer/cpp')
    for name in ['build_android.py','dependencies.json','README.md']:
        target=stage/'Sources/SDL_Mixer'/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(port/name,target)
    for name,item in json.loads((port/'dependencies.json').read_text()).items():
        archive=source_cache/(name+'-'+item['revision']+'.tar.gz')
        if hashlib.sha256(archive.read_bytes()).hexdigest()!=item['sha256']:raise ValueError('Source archive hash mismatch: '+name)
        target=stage/'Sources/third_party'/archive.name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(archive,target)
        for license in (sources[name].rglob('*') if name=='AudioCodecs' else sources[name].iterdir()):
            if license.is_file() and license.name.upper().startswith(('LICENSE','COPYING')):
                target=stage/'Sources/third_party'/name/license.relative_to(sources[name]);target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(license,target)
