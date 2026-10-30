"""Publish FindTheWay; invoked by FindTheWay.bat. Standard library only."""
from __future__ import annotations
import argparse
import datetime
import hashlib
import json
import logging
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import urllib.request
import zipfile

EXTENSIONS = Path(__file__).resolve().parent.parent
ANDROID = EXTENSIONS / 'Android_Gradle'
JAVA = ANDROID / 'app/src/main/java'
NATIVE = ANDROID / 'app/src/main/cpp'
CORE = EXTENSIONS / '_DeLib'
THIRD = EXTENSIONS / '_3rdLib'
ICONV = THIRD / 'iconv-android'
SEEDS = ['Extensions/CRunFindTheWay.java', '_DeLib/FindTheWayClass.java',
         '_DeLib/AOEClass.java', '_DeLib/Coord.java', '_DeLib/FusionUtilities.java',
         '_3rdLib/ObjectCreationClass.java', '_3rdLib/ObjectSelectionClass.java']

def copy(source: Path, target: Path) -> None:
    if not source.is_file():
        raise FileNotFoundError(f'Missing package dependency: {source}')
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, target)

def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def fusion_root() -> Path:
    return Path(os.environ.get('FusionRoot', r'C:\Steam\steamapps\common\Clickteam Fusion 2.5'))

ABIS = {'arm64-v8a': 'aarch64-linux-android',
        'armeabi-v7a': 'arm-linux-androideabi',
        'x86': 'i686-linux-android', 'x86_64': 'x86_64-linux-android'}
NDK_VERSION = '28.2.13676358'
CMAKE_VERSION = '3.22.1'

def run(command: list[str]) -> None:
    logging.info('Running: %s', subprocess.list2cmdline(command))
    with subprocess.Popen(command, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True, encoding='utf-8', errors='replace') as process:
        for line in process.stdout:
            logging.info('%s', line.rstrip())
        if process.wait():
            raise RuntimeError(f'Native build command failed (exit {process.returncode}); see log above')

def native_tools() -> tuple[Path, Path, Path]:
    roots = [Path(os.environ[key]) for key in ('ANDROID_HOME', 'ANDROID_SDK_ROOT')
             if os.environ.get(key)]
    if os.environ.get('LOCALAPPDATA'):
        roots.append(Path(os.environ['LOCALAPPDATA']) / 'Android/Sdk')
    sdk = next((p for p in roots if p.is_dir()), None)
    if sdk is None:
        raise FileNotFoundError('Android SDK not found. Set ANDROID_HOME to the SDK directory configured in Fusion.')
    executable = '.exe' if os.name == 'nt' else ''
    ndk = sdk / 'ndk' / NDK_VERSION
    cmake_dir = sdk / 'cmake' / CMAKE_VERSION / 'bin'
    cmake = cmake_dir / ('cmake' + executable)
    ninja = cmake_dir / ('ninja' + executable)
    if not cmake.is_file() or not ninja.is_file():
        installed = installed_cmake(sdk)
        if installed:
            cmake, ninja = installed
    packages = []
    if not (ndk / 'build/cmake/android.toolchain.cmake').is_file():
        packages.append('ndk;' + NDK_VERSION)
    if not cmake.is_file() or not ninja.is_file():
        packages.append('cmake;' + CMAKE_VERSION)
    if packages:
        manager_name = 'sdkmanager.bat' if os.name == 'nt' else 'sdkmanager'
        managers = list((sdk / 'cmdline-tools').glob('*/bin/' + manager_name))
        if (sdk / 'tools/bin' / manager_name).is_file():
            managers.append(sdk / 'tools/bin' / manager_name)
        managers.sort(key=lambda p: (p.parent.parent.name != 'latest', str(p)))
        if not managers:
            raise FileNotFoundError(f'Install Android SDK Command-line Tools in {sdk}; required packages: {packages}')
        logging.info('Preparing missing Android native tools: %s', ', '.join(packages))
        run([str(managers[0]), '--sdk_root=' + str(sdk), '--install', *packages])
    if not cmake.is_file() or not ninja.is_file() or not (ndk / 'build/cmake/android.toolchain.cmake').is_file():
        raise FileNotFoundError(f'Android NDK {NDK_VERSION} / CMake {CMAKE_VERSION} installation incomplete in {sdk}')
    return ndk, cmake, ninja

def installed_cmake(sdk: Path) -> tuple[Path, Path] | None:
    executable = '.exe' if os.name == 'nt' else ''
    candidates = list((sdk / 'cmake').glob('*/bin/cmake' + executable))
    if os.name == 'nt':
        vswhere = Path(os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        if vswhere.is_file():
            result = subprocess.run([str(vswhere), '-all', '-products', '*', '-property',
                                     'installationPath', '-utf8'], capture_output=True,
                                    text=True, encoding='utf-8', errors='replace', check=True)
            for line in result.stdout.splitlines():
                if line.strip():
                    candidates.append(Path(line.strip()) / 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe')
    if shutil.which('cmake'):
        candidates.append(Path(shutil.which('cmake')))
    for cmake in candidates:
        ninjas = [cmake.parent / ('ninja' + executable),
                  cmake.parent.parent.parent / 'Ninja' / ('ninja' + executable)]
        if shutil.which('ninja'):
            ninjas.append(Path(shutil.which('ninja')))
        ninja = next((p for p in ninjas if p.is_file()), None)
        if not cmake.is_file() or ninja is None:
            continue
        try:
            result = subprocess.run([str(cmake), '--version'], capture_output=True, text=True,
                                    encoding='utf-8', errors='replace', check=True)
        except (OSError, subprocess.CalledProcessError):
            continue
        version = re.search(r'cmake version (\d+)\.(\d+)', result.stdout)
        if version and tuple(map(int, version.groups())) >= (3, 22):
            logging.info('Using installed CMake and Ninja: %s; %s', cmake, ninja)
            return cmake, ninja
    return None

def build_native(stage: Path) -> None:
    ndk, cmake, ninja = native_tools()
    host = {'win32': 'windows-x86_64', 'linux': 'linux-x86_64', 'darwin': 'darwin-x86_64'}[sys.platform]
    toolchain = ndk / 'toolchains/llvm/prebuilt' / host
    strip = toolchain / 'bin' / ('llvm-strip.exe' if os.name == 'nt' else 'llvm-strip')
    for abi, triple in ABIS.items():
        build = ANDROID / '_NativeBuild' / abi
        output = build / 'out'
        run([str(cmake), '-S', str(stage / 'app/src/main/cpp'), '-B', str(build), '-G', 'Ninja',
             '-DCMAKE_MAKE_PROGRAM=' + ninja.as_posix(),
             '-DCMAKE_TOOLCHAIN_FILE=' + (ndk / 'build/cmake/android.toolchain.cmake').as_posix(),
             '-DCMAKE_BUILD_TYPE=Release', '-DANDROID_ABI=' + abi, '-DANDROID_PLATFORM=android-21',
             '-DANDROID_STL=c++_shared', '-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON',
             '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + output.as_posix()])
        run([str(cmake), '--build', str(build), '--parallel', str(min(os.cpu_count() or 2, 8))])
        target = stage / 'app/src/main/jniLibs' / abi
        copy(output / 'libfindtheway.so', target / 'libfindtheway.so')
        copy(toolchain / 'sysroot/usr/lib' / triple / 'libc++_shared.so', target / 'libc++_shared.so')
        for library in target.glob('*.so'):
            run([str(strip), '--strip-unneeded', str(library)])
        logging.info('Prepared JNI libraries for %s', abi)

def source_archive() -> Path:
    manifest = json.loads((ICONV / 'manifest.json').read_text())
    for relative, expected in manifest['files'].items():
        path = ICONV / relative
        if not path.is_file():
            raise ValueError(f'Missing iconv dependency: {path}; update/check your repository checkout')
        data = path.read_bytes()
        # Existing Windows checkouts may already have converted headers to CRLF.
        # Accept only that text transformation; libraries remain byte-exact.
        if path.suffix == '.h':
            data = data.replace(b'\r\n', b'\n')
        actual = hashlib.sha256(data).hexdigest()
        if actual != expected:
            raise ValueError(f'Modified iconv dependency: {path}; expected SHA-256 {expected}, got {actual}')
    archive = ICONV / 'source.tar.gz'
    if not archive.exists() or digest(archive) != manifest['source_sha256']:
        temporary = archive.with_suffix('.download')
        try:
            logging.info('Downloading pinned iconv source and license material...')
            with urllib.request.urlopen(manifest['source_url'], timeout=120) as response:
                with temporary.open('wb') as output:
                    shutil.copyfileobj(response, output)
            if digest(temporary) != manifest['source_sha256']:
                raise ValueError('Downloaded iconv source hash mismatch')
            temporary.replace(archive)
        finally:
            temporary.unlink(missing_ok=True)
    return archive

def java_sources(stage: Path) -> None:
    pending = [JAVA / name for name in SEEDS]
    seen: set[Path] = set()
    seeds = set(pending)
    def owned(source: Path) -> bool:
        # The development SDK also contains Runtime, Banks, Objects, etc. Those
        # belong to Fusion's installed Runtime, never to this extension package.
        return source in seeds or source.relative_to(JAVA).parts[0] in {'_DeLib', '_3rdLib'}
    # Resolve project imports and same-package helpers, including static imports.
    while pending:
        source = pending.pop()
        if source in seen:
            continue
        seen.add(source)
        copy(source, stage / 'app/src/main/java' / source.relative_to(JAVA))
        text = source.read_text(encoding='utf-8-sig')
        for name in re.findall(r'\bimport\s+(?:static\s+)?([\w.]+)\s*;', text, re.M):
            parts = name.split('.')
            while parts:
                candidate = JAVA.joinpath(*parts).with_suffix('.java')
                if candidate.is_file():
                    if owned(candidate):
                        pending.append(candidate)
                    break
                parts.pop()
        for candidate in source.parent.glob('*.java'):
            if owned(candidate) and re.search(r'\b' + re.escape(candidate.stem) + r'\b', text):
                pending.append(candidate)

def shared_headers(stage: Path) -> None:
    # The entire native source directory is copied, so additional C/C++ sources,
    # local headers and CMake files are not maintained in a second manual list.
    shutil.copytree(NATIVE, stage / 'app/src/main/cpp')
    pending = [p for p in NATIVE.rglob('*') if p.suffix in {'.cpp', '.c', '.h', '.hpp', '.cc', '.cxx', '.hxx'}]
    seen: set[Path] = set()
    while pending:
        source = pending.pop().resolve()
        if source in seen:
            continue
        seen.add(source)
        if source.is_relative_to(CORE):
            copy(source, stage / 'app/src/main/cpp/shared/_DeLib' / source.relative_to(CORE))
        elif source.is_relative_to(THIRD):
            copy(source, stage / 'app/src/main/cpp/shared/_3rdLib' / source.relative_to(THIRD))
        elif not source.is_relative_to(NATIVE):
            raise ValueError(f'C/C++ dependency outside supported roots: {source}')
        for include in re.findall(r'^\s*#\s*include\s*"([^"]+)"', source.read_text(encoding='utf-8-sig'), re.M):
            roots = [source.parent, NATIVE, CORE, THIRD]
            match = next((r / include for r in roots if (r / include).is_file()), None)
            if match is None:
                raise FileNotFoundError(f'{source}: unresolved include "{include}"')
            pending.append(match)

def stage_android(stage: Path) -> None:
    # Validate/download before removing the previous staging directory.
    archive = source_archive()
    if stage.resolve() in {EXTENSIONS, ANDROID, NATIVE, CORE, THIRD, ICONV}:
        raise ValueError('Unsafe staging directory')
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)
    java_sources(stage)
    shared_headers(stage)
    shutil.copytree(ANDROID / 'cmake', stage / 'cmake')
    # R8 discovers JAR consumer rules without replacing Fusion's ProGuard file.
    rules = stage / 'app/libs/FindTheWay-JNI-rules.jar'
    rules.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(rules, 'w', zipfile.ZIP_DEFLATED) as archive_rules:
        archive_rules.writestr('META-INF/proguard/FindTheWay.pro',
                              (ANDROID / 'FindTheWay-JNI.pro').read_bytes())
    for source, relative in [(ANDROID / 'FindTheWay-JNI.md', 'FindTheWay-JNI.md'),
                             (EXTENSIONS.parent / 'LICENSE', 'FindTheWay-LICENSE.txt')]:
        copy(source, stage / relative)
    target = stage / 'app/src/main/cpp/third_party/iconv'
    for source in ICONV.rglob('*'):
        if source.is_file() and source.name not in {'.gitignore', 'source.tar.gz'}:
            copy(source, target / source.relative_to(ICONV))
    copy(archive, target / 'source.tar.gz')
    build_native(stage)
    entries = {str(p.relative_to(stage)).replace('\\', '/'): digest(p)
               for p in sorted(stage.rglob('*')) if p.is_file()}
    (stage / 'FindTheWay-package-manifest.json').write_text(json.dumps(entries, indent=2), encoding='utf-8')
    logging.info('Staged %d verified package files', len(entries))

def make_zip(folder: Path, output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_suffix('.partial')
    try:
        with zipfile.ZipFile(temporary, 'w', zipfile.ZIP_DEFLATED) as archive:
            for path in sorted(folder.rglob('*')):
                if path.is_file() and path.suffix != '.001' and path.name not in {'.DS_Store', '.keep'}:
                    archive.write(path, path.relative_to(folder))
        temporary.replace(output)
    finally:
        temporary.unlink(missing_ok=True)
    logging.info('Created %s', output)

def publish(stage: Path) -> None:
    install = EXTENSIONS / 'FindTheWay/ToInstall/Files'
    android_zip = install / 'Data/Runtime/Android/FindTheWay.zip'
    make_zip(stage, android_zip)
    html = EXTENSIONS / 'HTML5'
    inputs = ['_3rdLib/ObjectCreationClass.js', '_3rdLib/ObjectSelectionClass.js',
              '_DeLib/FusionUtilities.js', '_DeLib/Coord.js', '_DeLib/FindTheWayClass.js',
              '_DeLib/AOEClass.js', 'FindTheWay.js']
    data = b'\n'.join((html / name).read_bytes() for name in inputs)
    html_output = install / 'Data/Runtime/Html5/FindTheWay.js'
    html_output.parent.mkdir(parents=True, exist_ok=True)
    html_output.write_bytes(data)
    fusion = fusion_root()
    if (fusion / 'Data/Runtime/Android').is_dir():
        copy(android_zip, fusion / 'Data/Runtime/Android/FindTheWay.zip')
    else:
        logging.info('Fusion install directory not found; ToInstall output is available')
    if (fusion / 'Data/Runtime/Html5').is_dir():
        copy(html_output, fusion / 'Data/Runtime/Html5/FindTheWay.js')
    dated = EXTENSIONS / ('FindTheWay_B' + datetime.date.today().strftime('%Y%m%d') + '.zip')
    make_zip(install, dated)
    copy(dated, EXTENSIONS / 'FindTheWay_Release.zip')

def main() -> int:
    if sys.version_info < (3, 10):
        print('ERROR: Python 3.10 or newer is required.', file=sys.stderr)
        return 1
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--stage-only', action='store_true', help='Validation only: do not publish/install')
    parser.add_argument('--stage-dir', type=Path, default=ANDROID / '_Release')
    args = parser.parse_args()
    log = ANDROID / 'FindTheWay-package.log'
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s',
                        handlers=[logging.StreamHandler(), logging.FileHandler(log, encoding='utf-8')])
    try:
        stage_android(args.stage_dir)
        if not args.stage_only:
            publish(args.stage_dir)
        return 0
    except Exception:
        logging.exception('Packaging failed. See %s; release artifacts may be from an earlier run.', log)
        return 1

if __name__ == '__main__':
    sys.exit(main())
