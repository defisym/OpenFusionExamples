#!/usr/bin/env python3
"""Build the pinned LGPL FFmpeg Android SDK with an installed NDK and bash/make."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import urllib.request

# One version selection for Windows and Android; the ZIP carries a snapshot of
# the same CMake file next to this reproduction recipe.
VERSION_FILE = Path(__file__).resolve().parents[2] / '_3rdLib/FFmpeg-build/FFmpegVersion.cmake'
if not VERSION_FILE.is_file(): VERSION_FILE = Path(__file__).with_name('FFmpegVersion.cmake')
def version_settings(path=VERSION_FILE):
    text = path.read_text()
    def setting(name):
        match = re.search(r'^set\(' + name + r'\s+"([^"\n]+)"\)', text, re.MULTILINE)
        if not match: raise ValueError('Missing literal ' + name + ' in ' + str(path))
        return match.group(1)
    version, sha = setting('OFE_FFMPEG_VERSION'), setting('OFE_FFMPEG_SOURCE_SHA256')
    if not re.fullmatch(r'[0-9]+(?:\.[0-9]+)+', version) or not re.fullmatch(r'[0-9a-fA-F]{64}', sha):
        raise ValueError('Invalid FFmpeg version/source SHA256 in ' + str(path))
    return version, sha.lower()
VERSION, SHA256 = version_settings()
URL = f'https://codeload.github.com/FFmpeg/FFmpeg/tar.gz/refs/tags/n{VERSION}'
ABIS = {
    'arm64-v8a': ('aarch64', 'aarch64-linux-android', 'aarch64-linux-android'),
    'armeabi-v7a': ('arm', 'armv7a-linux-androideabi', 'arm-linux-androideabi'),
    'x86': ('x86', 'i686-linux-android', 'i686-linux-android'),
    'x86_64': ('x86_64', 'x86_64-linux-android', 'x86_64-linux-android'),
}
COMPONENTS = ('avcodec', 'avformat', 'avutil', 'swscale', 'swresample', 'avfilter')

def sdk_matches(output, abis):
    try:
        manifest = json.loads((output/'manifest.json').read_text())
        return (manifest['version'] == VERSION and manifest['source_sha256'] == SHA256
            and all(abi in manifest['abis'] and (output/abi/'include/libavcodec/avcodec.h').is_file()
                and all((output/abi/'lib'/('lib'+c+'.so')).is_file() for c in COMPONENTS) for abi in abis))
    except (OSError, ValueError, KeyError, TypeError): return False


def run(args, cwd=None):
    print('+', subprocess.list2cmdline(list(map(str, args))), flush=True)
    subprocess.run(list(map(str, args)), cwd=cwd, check=True)

def source(cache: Path) -> Path:
    cache.mkdir(parents=True, exist_ok=True)
    archive = cache / f'ffmpeg-n{VERSION}.tar.gz'
    if not archive.is_file() or hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        temporary = archive.with_suffix('.part')
        with urllib.request.urlopen(URL, timeout=120) as response, temporary.open('wb') as out:
            shutil.copyfileobj(response, out)
        if hashlib.sha256(temporary.read_bytes()).hexdigest() != SHA256:
            temporary.unlink(); raise ValueError('FFmpeg source SHA-256 mismatch')
        temporary.replace(archive)
    root = cache / f'FFmpeg-n{VERSION}'
    # Always verify the archive; extract into a clean staging directory once.
    if not root.exists():
        with tarfile.open(archive) as tar:
            tar.extractall(cache, filter='data')
    return root

def build(ndk: Path, output: Path, cache: Path, abis, jobs: int):
    if sys.version_info < (3, 12):
        raise RuntimeError('FFmpeg SDK source preparation requires Python 3.12 or newer')
    if os.name == 'nt':
        raise RuntimeError('Build the FFmpeg SDK under Linux/WSL with its Linux NDK, then set OFE_FFMPEG_ROOT to that SDK for Windows publishing.')
    previous_abis = []
    try:
        previous = json.loads((output/'manifest.json').read_text())
        if previous['version'] == VERSION and previous['source_sha256'] == SHA256: previous_abis = previous['abis']
    except (OSError, ValueError, KeyError): pass
    src = source(cache)
    hosts = list((ndk / 'toolchains/llvm/prebuilt').glob('*'))
    if len(hosts) != 1: raise ValueError('NDK must contain one host LLVM toolchain')
    toolchain = hosts[0] / 'bin'
    suffix = ''
    for abi in abis:
        arch, cc, triple = ABIS[abi]
        work = cache / ('build-' + abi); work.mkdir(exist_ok=True)
        prefix = (output / abi).resolve()
        configuration = [
            str(src / 'configure'), '--target-os=android', '--enable-cross-compile', '--arch=' + arch,
            '--cc=' + str(toolchain / (cc + '21-clang')),
            '--cxx=' + str(toolchain / (cc + '21-clang++')),
            '--ar=' + str(toolchain / ('llvm-ar' + suffix)), '--ranlib=' + str(toolchain / ('llvm-ranlib' + suffix)),
            '--strip=' + str(toolchain / ('llvm-strip' + suffix)), '--nm=' + str(toolchain / ('llvm-nm' + suffix)),
            '--prefix=' + str(prefix), '--disable-autodetect', '--enable-shared', '--disable-static',
            '--disable-programs', '--disable-doc', '--disable-avdevice', '--enable-avfilter', '--disable-filters',
            '--enable-filter=abuffer,abuffersink,atempo,aformat,aresample',
            '--disable-encoders', '--disable-muxers', '--enable-zlib',
            '--enable-jni', '--enable-mediacodec', '--disable-x86asm',
            '--extra-ldflags=-Wl,-z,max-page-size=16384',
        ]
        if abi == 'armeabi-v7a': configuration += ['--cpu=armv7-a', '--enable-neon']
        # Android forbids the absolute text relocations emitted by some 32-bit x86 inline assembly.
        if abi == 'x86': configuration += ['--disable-inline-asm']
        signature = json.dumps({'configure': configuration,
            'ndk': (ndk/'source.properties').read_text(), 'source': SHA256}, sort_keys=True)
        stamp = work/'ofe-build-signature.json'
        if (work/'Makefile').exists() and (not stamp.exists() or stamp.read_text() != signature):
            run(['make', 'distclean'], cwd=work)
        run(['bash', *configuration], cwd=work)
        stamp.write_text(signature)
        run(['make', '-j' + str(jobs)], cwd=work)
        run(['make', 'install'], cwd=work)
        for name in COMPONENTS:
            if not (prefix / 'lib' / f'lib{name}.so').is_file(): raise FileNotFoundError(name)
        shutil.copy2(work / 'config.h', prefix / 'config.h')
        shutil.copy2(work / 'ffbuild/config.mak', prefix / 'config.mak')
    licenses = output / 'licenses'; licenses.mkdir(parents=True, exist_ok=True)
    for item in src.glob('COPYING*'): shutil.copy2(item, licenses / item.name)
    shutil.copy2(cache / f'ffmpeg-n{VERSION}.tar.gz', output / f'ffmpeg-n{VERSION}.tar.gz')
    (output / 'manifest.json').write_text(json.dumps({'version': VERSION, 'source_url': URL,
        'source_sha256': SHA256, 'abis': [abi for abi in ABIS if abi in set(previous_abis) | set(abis) and all((output/abi/'lib'/('lib'+c+'.so')).is_file() for c in COMPONENTS)], 'license': 'LGPL-2.1-or-later', 'api': 21}, indent=2) + '\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ndk', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--cache', type=Path, default=Path(__file__).parent / 'out')
    parser.add_argument('--abi', action='append', choices=list(ABIS))
    parser.add_argument('--jobs', type=int, default=min(os.cpu_count() or 2, 8))
    args = parser.parse_args()
    build(args.ndk.resolve(), args.output.resolve(), args.cache.resolve(), args.abi or list(ABIS), args.jobs)
