"""Exercise the actual SDK stager with local archives, without Windows/network."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[3]
SCRIPTS = ROOT / '_3rdLib' / 'Encrypt-build'
CMAKE = os.environ.get('CMAKE', shutil.which('cmake') or 'cmake')

class SDKTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.sdk = self.root / 'SDK with spaces'
        self.work = self.root / 'work'
    def archive(self, arch='x86', version='3.6.5', missing=None):
        files = {
            'include/openssl/evp.h': b'/* test header */',
            'include/openssl/opensslv.h': ('#define OPENSSL_VERSION_STR "'+version+'"\n').encode(),
            'lib/libcrypto.lib': b'!<arch>\nfixture',
            'debug/lib/libcrypto.lib': b'!<arch>\nfixture-debug',
            'share/openssl/copyright': b'Apache-2.0 fixture',
            'ofe-encryption-sdk.json': json.dumps(dict(arch=arch, version=version, linkage='static', crt='dynamic')).encode(),
        }
        if missing: files.pop(missing)
        path = self.root / (arch+'-'+version+'-'+str(len(list(self.root.glob('*.zip'))))+'.zip')
        with zipfile.ZipFile(path, 'w') as archive:
            for name, content in files.items(): archive.writestr(name, content)
        return path
    def prepare(self, archive, sha=None):
        digest = sha or hashlib.sha256(archive.read_bytes()).hexdigest()
        return subprocess.run([CMAKE, '-DOFE_ARCH=x86', '-DOFE_OPENSSL_ROOT='+str(self.sdk),
            '-DOFE_WORK_DIR='+str(self.work), '-DOFE_PREBUILT_ARCHIVE='+str(archive),
            '-DOFE_PREBUILT_SHA256='+digest, '-P', str(SCRIPTS/'EnsureSDK.cmake')],
            text=True, capture_output=True)
    def test_install_reuse_and_repair(self):
        archive = self.archive()
        result = self.prepare(archive)
        self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
        stamp = self.sdk/'.ofe-sdk.stamp'
        before = stamp.stat().st_mtime_ns
        result = self.prepare(archive)
        self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
        self.assertIn('up to date', result.stdout)
        self.assertEqual(stamp.stat().st_mtime_ns, before)
        (self.sdk/'debug/lib/libcrypto.lib').unlink()
        self.assertEqual(self.prepare(archive).returncode, 0)
        self.assertTrue((self.sdk/'debug/lib/libcrypto.lib').exists())
    def test_url_download_and_cache(self):
        archive = self.archive()
        sha = hashlib.sha256(archive.read_bytes()).hexdigest()
        result = subprocess.run([CMAKE, '-DOFE_ARCH=x86', '-DOFE_OPENSSL_ROOT='+str(self.sdk),
            '-DOFE_WORK_DIR='+str(self.work), '-DOFE_PREBUILT_URL='+archive.as_uri(),
            '-DOFE_PREBUILT_SHA256='+sha, '-P', str(SCRIPTS/'EnsureSDK.cmake')],
            text=True, capture_output=True)
        self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
        self.assertEqual((self.work/'downloads'/(sha+'.zip')).read_bytes(), archive.read_bytes())

    def test_default_release_by_architecture_and_offline_repair(self):
        import re
        scripts = self.root / 'scripts'
        scripts.mkdir()
        for name in ['EnsureSDK.cmake', 'ValidateSDK.cmake', 'EncryptVersion.cmake', 'vcpkg.json']:
            shutil.copyfile(SCRIPTS/name, scripts/name)
        version = (scripts/'EncryptVersion.cmake').read_text()
        mirror = self.root/'release'
        mirror.mkdir()
        version = re.sub(r'set\(OFE_ENCRYPT_SDK_URL "[^"]+"\)',
            'set(OFE_ENCRYPT_SDK_URL "'+mirror.as_uri()+'")', version)
        digests = {}
        for arch in ['x86', 'x64']:
            archive = self.archive(arch)
            shutil.copyfile(archive, mirror/('openssl-'+arch+'.zip'))
            digests[arch] = hashlib.sha256(archive.read_bytes()).hexdigest()
            version = re.sub(r'set\(OFE_ENCRYPT_SDK_'+arch.upper()+r'_SHA256 "[^"]+"\)',
                'set(OFE_ENCRYPT_SDK_'+arch.upper()+'_SHA256 "'+digests[arch]+'")', version)
        (scripts/'EncryptVersion.cmake').write_text(version)

        for arch in ['x86', 'x64']:
            sdk = self.sdk/arch
            command = [CMAKE, '-DOFE_ARCH='+arch, '-DOFE_PLATFORM_TOOLSET=v143',
                '-DOFE_OPENSSL_ROOT='+str(sdk), '-DOFE_WORK_DIR='+str(self.work),
                '-P', str(scripts/'EnsureSDK.cmake')]
            result = subprocess.run(command, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            metadata = json.loads((sdk/'ofe-encryption-sdk.json').read_text())
            self.assertEqual(metadata['arch'], arch)
            self.assertTrue((sdk/'lib/libcrypto.lib').exists())
            self.assertTrue((sdk/'debug/lib/libcrypto.lib').exists())
            self.assertFalse((self.work/'vcpkg').exists())

            # After the first install, the prepared SDK and cached archive work offline.
            (mirror/('openssl-'+arch+'.zip')).unlink()
            result = subprocess.run(command, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            self.assertIn('up to date', result.stdout)
            (sdk/'debug/lib/libcrypto.lib').unlink()
            result = subprocess.run(command, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            self.assertTrue((sdk/'debug/lib/libcrypto.lib').exists())

    def test_source_build_is_explicit_and_toolset_is_checked(self):
        command = [CMAKE, '-DOFE_ARCH=x86', '-DOFE_OPENSSL_ROOT='+str(self.sdk),
            '-DOFE_WORK_DIR='+str(self.work), '-DOFE_PLATFORM_TOOLSET=v142']
        result = subprocess.run(command+['-P', str(SCRIPTS/'EnsureSDK.cmake')], text=True, capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('OfeEncryptBuildFromSource=true', result.stderr)
        if os.name != 'nt':
            result = subprocess.run(command+['-DOFE_BUILD_FROM_SOURCE=true', '-P',
                str(SCRIPTS/'EnsureSDK.cmake')], text=True, capture_output=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('Source preparation requires Windows', result.stderr)

    def test_rejections_preserve_previous_sdk(self):
        good = self.archive()
        self.assertEqual(self.prepare(good).returncode, 0)
        marker = self.sdk/'keep.txt'
        marker.write_text('previous SDK')
        original = (self.sdk/'.ofe-sdk.stamp').read_bytes()
        for archive, sha in [(good, '0'*64), (self.archive('x64'), None),
                (self.archive(version='3.0.0'), None),
                (self.archive(missing='debug/lib/libcrypto.lib'), None),
                (self.archive(missing='share/openssl/copyright'), None)]:
            result = self.prepare(archive, sha)
            self.assertNotEqual(result.returncode, 0, result.stdout)
            self.assertEqual(marker.read_text(), 'previous SDK')
            self.assertEqual((self.sdk/'.ofe-sdk.stamp').read_bytes(), original)
    def test_version_pin_and_projects(self):
        import re
        import xml.etree.ElementTree as ET
        cmake = (SCRIPTS/'EncryptVersion.cmake').read_text()
        baseline = json.loads((SCRIPTS/'vcpkg.json').read_text())['builtin-baseline']
        self.assertIn(baseline, cmake)
        self.assertRegex(baseline, r'^[0-9a-f]{40}$')
        self.assertIn('defisym/OpenFusionExamples/releases/download/encrypt-sdk-', cmake)
        for arch in ['X86', 'X64']:
            self.assertRegex(cmake, 'OFE_ENCRYPT_SDK_'+arch+r'_SHA256 "[0-9a-f]{64}"')
        ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
        props = ET.parse(ROOT/'_DeLib/Encryption.props')
        source_mode = props.find('.//m:OfeEncryptBuildFromSource', ns)
        self.assertEqual(source_mode.text, 'false')
        reference = props.find('.//m:ProjectReference', ns)
        self.assertIsNotNone(reference)
        self.assertIn('EncryptDependencies.vcxproj', reference.attrib['Include'])
        count = 0
        for project in ROOT.glob('*/*.vcxproj'):
            text = project.read_text(encoding='utf-8-sig')
            if 'Include="..\\_DeLib\\Encryption.cpp"' not in text: continue
            count += 1
            self.assertIn('Encryption.props', text)
            solution = project.with_suffix('.sln').read_text(encoding='utf-8-sig')
            self.assertEqual(solution.count(' = "EncryptDependencies"'), 1)
            self.assertRegex(solution, r'\.Debug Unicode\|(?:Win32|x86)\.ActiveCfg = Release\|Win32')
            self.assertNotIn('ActiveCfg = Release|x86', solution)
        self.assertEqual(count, 9)
        ET.parse(SCRIPTS/'EncryptDependencies.vcxproj')
        header = (ROOT/'_DeLib/Encryption.h').read_text()
        self.assertNotRegex(header, r'#include\s*[<"](?:windows|bcrypt)\.h')
        self.assertNotRegex(header, r'\b(?:BYTE|DWORD|PBYTE|LPCWSTR|BCRYPT_[A-Z_]+)\b')

if __name__ == '__main__': unittest.main()
