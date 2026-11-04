"""Fini native tool preparation, following FTW's installed-tool/sdkmanager flow.
No dependency on FindTheWay.py, its core, package names, or build outputs.
"""
import hashlib
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import urllib.request
from urllib.parse import urlsplit
import xml.etree.ElementTree as ET
import zipfile

REPOSITORY = 'https://dl.google.com/android/repository/'


def sdk_root(explicit, repo):
    if explicit:
        return Path(explicit).expanduser().resolve()
    candidates = [Path(os.environ[k]).expanduser() for k in ('ANDROID_SDK_ROOT', 'ANDROID_HOME') if os.environ.get(k)]
    local = repo / 'Extensions/Android_Gradle/local.properties'
    if local.is_file():
        match = re.search(r'^\s*sdk\.dir\s*=\s*(.+)$', local.read_text(), re.M)
        if match:
            candidates.append(Path(match[1].replace('\\:', ':').replace('\\\\', '\\')))
    if os.environ.get('LOCALAPPDATA'):
        candidates.append(Path(os.environ['LOCALAPPDATA']) / 'Android/Sdk')
    candidates += [Path.home() / 'Library/Android/sdk', Path.home() / 'Android/Sdk']
    return next((p.resolve() for p in candidates if p.is_dir()), repo / 'work/toolchains/android-sdk')


def executable(name):
    return name + ('.exe' if os.name == 'nt' else '')


def capture(command):
    return subprocess.check_output(list(map(str, command)), text=True, encoding='utf-8', errors='replace').strip()


def find_build_tools(sdk, cmake=None, ninja=None):
    """Reuse SDK, VS-bundled or PATH tools; VS Ninja may be in a sibling tree."""
    if cmake and not Path(cmake).is_file():
        raise RuntimeError('Explicit --cmake does not exist: ' + str(cmake))
    if ninja and not Path(ninja).is_file():
        raise RuntimeError('Explicit --ninja does not exist: ' + str(ninja))
    candidates = [Path(cmake)] if cmake else []
    if not cmake:
        candidates += sorted((sdk / 'cmake').glob('*/bin/' + executable('cmake')), reverse=True)
        if os.name == 'nt':
            vswhere = Path(os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
            if vswhere.is_file():
                for line in capture([vswhere, '-all', '-products', '*', '-property', 'installationPath', '-utf8']).splitlines():
                    if line.strip():
                        candidates.append(Path(line.strip()) / 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe')
        if shutil.which('cmake'):
            candidates.append(Path(shutil.which('cmake')))
    for candidate in candidates:
        if not candidate.is_file():
            continue
        try:
            version = re.search(r'cmake version (\d+)\.(\d+)', capture([candidate, '--version']))
        except (OSError, subprocess.CalledProcessError):
            continue
        if not version or tuple(map(int, version.groups())) < (3, 22):
            continue
        ninjas = [Path(ninja)] if ninja else [candidate.parent / executable('ninja'), candidate.parent.parent.parent / 'Ninja' / executable('ninja')]
        if not ninja:
            ninjas += sorted((sdk/'cmake').glob('*/bin/'+executable('ninja')),reverse=True)
            if shutil.which('ninja'):ninjas.append(Path(shutil.which('ninja')))
        for build in ninjas:
            if build.is_file():
                try:
                    capture([build, '--version'])
                    return candidate.resolve(), build.resolve()
                except (OSError, subprocess.CalledProcessError):
                    continue
    return None


def find_sdkmanager(sdk):
    name = 'sdkmanager.bat' if os.name == 'nt' else 'sdkmanager'
    managers = list((sdk / 'cmdline-tools').glob('*/bin/' + name))
    managers.sort(key=lambda p: (p.parent.parent.name != 'latest', str(p)))
    managers += [sdk / 'tools/bin' / name]
    if shutil.which(name):
        managers.append(Path(shutil.which(name)))
    return next((p for p in managers if p.is_file()), None)


def command_line_archive(xml, version, host):
    """Select an exact tools revision and its Google-published archive checksum."""
    root = ET.fromstring(xml)
    for package in root.iter():
        if package.tag.rsplit('}', 1)[-1] != 'remotePackage' or package.attrib.get('path') != 'cmdline-tools;' + version:
            continue
        for archive in package.findall('./archives/archive'):
            if archive.findtext('host-os') != host:
                continue
            complete = archive.find('complete')
            checksum = complete.find('checksum')
            url = complete.findtext('url')
            algorithm = checksum.attrib.get('type', 'sha1')
            if algorithm not in ('sha1', 'sha256') or not re.fullmatch(r'commandlinetools-[\w.-]+\.zip', url or ''):
                raise RuntimeError('Unsupported SDK tools archive/checksum in Google metadata')
            return url, algorithm, checksum.text.strip()
    raise RuntimeError('Google repository metadata has no cmdline-tools;' + version + ' for ' + host)


def bootstrap_sdkmanager(sdk, version):
    host = {'Windows': 'windows', 'Linux': 'linux', 'Darwin': 'macosx'}[platform.system()]
    print('Preparing Android Command-line Tools ' + version + ' from Google repository metadata', flush=True)
    try:
        with urllib.request.urlopen(REPOSITORY + 'repository2-3.xml', timeout=30) as response:
            metadata = response.read()
        url, algorithm, expected = command_line_archive(metadata, version, host)
        sdk.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='fini-sdk-', dir=sdk) as temp:
            temp = Path(temp)
            archive = temp / 'tools.zip'
            digest = hashlib.new(algorithm)
            with urllib.request.urlopen(REPOSITORY + url, timeout=30) as source, archive.open('wb') as output:
                while True:
                    block = source.read(1024 * 1024)
                    if not block:
                        break
                    digest.update(block)
                    output.write(block)
            if digest.hexdigest().lower() != expected.lower():
                raise RuntimeError('Android Command-line Tools checksum mismatch')
            unpacked = temp / 'unpacked'
            with zipfile.ZipFile(archive) as zipped:
                for entry in zipped.infolist():
                    path = Path(entry.filename)
                    if path.is_absolute() or '..' in path.parts or '\\' in entry.filename:
                        raise RuntimeError('Unsafe SDK archive path')
                zipped.extractall(unpacked)
            target = sdk / 'cmdline-tools' / version
            if target.exists():
                raise RuntimeError('Incomplete tools already exist at ' + str(target) + '; repair that directory explicitly')
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.move(str(unpacked / 'cmdline-tools'), target)
            if os.name != 'nt':
                for path in (target / 'bin').iterdir():
                    path.chmod(path.stat().st_mode | 0o111)
        manager = find_sdkmanager(sdk)
        if not manager:
            raise RuntimeError('Command-line Tools extraction did not produce sdkmanager')
        return manager
    except OSError as error:
        raise RuntimeError('Cannot prepare Android Command-line Tools: ' + str(error) +
                           '. Use an existing SDK via --sdk/ANDROID_SDK_ROOT or repair network access; no NDK build was performed.') from error


def prepare(repo, config, run, sdk=None, ndk=None, cmake=None, ninja=None, install=True):
    sdk = sdk_root(sdk, repo)
    explicit_ndk = ndk or os.environ.get('ANDROID_NDK_HOME')
    ndk = Path(explicit_ndk).expanduser().resolve() if explicit_ndk else sdk / 'ndk' / config['ndk']
    ready_ndk = (ndk / 'build/cmake/android.toolchain.cmake').is_file()
    if explicit_ndk and not ready_ndk:
        raise RuntimeError('Explicit Android NDK is invalid: ' + str(ndk))
    tools = find_build_tools(sdk, cmake, ninja)
    packages = []
    if not ready_ndk:
        packages.append('ndk;' + config['ndk'])
    if not tools:
        packages.append('cmake;' + config['cmake'])
    if packages:
        if not install:
            raise RuntimeError('Missing Android packages: ' + ', '.join(packages) + '; allow preparation or install them with sdkmanager')
        manager = find_sdkmanager(sdk) or bootstrap_sdkmanager(sdk, config['command_line_tools'])
        # Existing accepted licenses are respected. Never feed automatic license acceptance.
        proxy=[]
        proxy_url=os.environ.get('HTTPS_PROXY') or os.environ.get('https_proxy') or os.environ.get('HTTP_PROXY') or os.environ.get('http_proxy')
        if proxy_url:
            address=urlsplit(proxy_url)
            if address.hostname:
                proxy=['--proxy=http','--proxy_host='+address.hostname,'--proxy_port='+str(address.port or 80)]
        run([manager, '--sdk_root=' + str(sdk), *proxy, '--install', *packages])
        tools = find_build_tools(sdk, cmake, ninja)
    if not (ndk / 'build/cmake/android.toolchain.cmake').is_file() or not tools:
        raise RuntimeError('Native tools remain incomplete after sdkmanager. Inspect publisher.log, JAVA_HOME, and sdkmanager --licenses for the selected SDK.')
    properties = ndk / 'source.properties'
    if properties.is_file():
        match = re.search(r'Pkg.Revision\s*=\s*(\S+)', properties.read_text())
        if not match or int(match[1].split('.')[0]) < 28:
            raise RuntimeError('Fini requires NDK 28+ for its native/STL build contract: ' + str(ndk))
    return {'sdk': sdk, 'ndk': ndk, 'cmake': tools[0], 'ninja': tools[1]}
