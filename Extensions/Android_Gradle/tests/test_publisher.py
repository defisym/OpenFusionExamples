"""Packaging regression tests; no SDK, NDK or network required."""
import hashlib
import importlib.util
import io
import json
import subprocess
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('ftw_publish', Path(__file__).resolve().parents[2] / '_Script/FindTheWay.py')
publish = importlib.util.module_from_spec(spec)
spec.loader.exec_module(publish)

class PublisherTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        ext = self.root / 'Extensions'
        android = ext / 'Android_Gradle'
        java = android / 'app/src/main/java'
        native = android / 'app/src/main/cpp'
        core = ext / '_DeLib'
        third = ext / '_3rdLib'
        iconv = third / 'iconv-android'
        self.stage = self.root / 'stage'
        self.iconv = iconv
        self.payload = b'corresponding source'
        files = {
            java / 'Extensions/Main.java': 'package Extensions; import static _DeLib.Helper.save; class Main {}',
            java / '_DeLib/Helper.java': 'package _DeLib; class Helper { Common value; }',
            java / '_DeLib/Common.java': 'package _DeLib; class Common {}',
            native / 'FindTheWayJNI.cpp': '#include "Core.h"\n',
            native / 'extra/Additional.cpp': '#include "Local.hpp"\n',
            native / 'extra/Local.hpp': '// local header\n',
            native / 'CMakeLists.txt': '# sources\n',
            core / 'Core.h': '#include "Base64.h"\n',
            third / 'Base64.h': '// external header\n',
            android / 'cmake/DeLibHelpers.cmake': '# configuration\n',
            android / 'release.gradle': '// gradle\n',
            android / 'ReleaseSettings.gradle': '// settings\n',
            android / 'app/proguard-rules.pro': '# rules\n',
            android / 'FindTheWay-JNI.pro': '-keep class _DeLib.FindTheWayClass { *; }\n',
            android / 'FindTheWay-JNI.md': '# notes\n',
            self.root / 'LICENSE': 'license\n',
            iconv / 'arm64-v8a/lib/libiconv.a': 'library',
        }
        for path, text in files.items():
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
        self.lib = iconv / 'arm64-v8a/lib/libiconv.a'
        manifest = {'files': {'arm64-v8a/lib/libiconv.a': publish.digest(self.lib)},
                    'source_url': 'https://example.invalid/pinned-source',
                    'source_sha256': hashlib.sha256(self.payload).hexdigest()}
        (iconv / 'manifest.json').write_text(json.dumps(manifest))
        (iconv / 'source.tar.gz').write_bytes(self.payload)
        self.patcher = patch.multiple(publish, EXTENSIONS=ext, ANDROID=android, JAVA=java,
                                     NATIVE=native, CORE=core, THIRD=third, ICONV=iconv,
                                     SEEDS=['Extensions/Main.java'])
        self.patcher.start()
        self.addCleanup(self.patcher.stop)
        root_patch = patch.object(publish, 'fusion_root', return_value=self.root / 'Fusion')
        root_patch.start()
        self.addCleanup(root_patch.stop)

        def fake_build(stage):
            for abi in publish.ABIS:
                for name in ('libfindtheway.so', 'libc++_shared.so'):
                    target = stage / 'app/src/main/jniLibs' / abi / name
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(b'fixture library')
        native_patch = patch.object(publish, 'build_native', side_effect=fake_build)
        native_patch.start()
        self.addCleanup(native_patch.stop)

    def test_package_leaves_default_gradle_and_runtime_untouched(self):
        runtime = publish.fusion_root() / 'Data/Runtime/Android/RuntimeAndroid.zip'
        runtime.parent.mkdir(parents=True)
        runtime.write_bytes(b'untouched runtime')
        publish.stage_android(self.stage)
        self.assertEqual(runtime.read_bytes(), b'untouched runtime')
        self.assertFalse((self.stage / 'app/build.gradle').exists())
        self.assertFalse((self.stage / 'settings.gradle').exists())
        self.assertFalse((self.stage / 'app/proguard-rules.txt').exists())
        self.assertFalse((self.stage / 'app/FindTheWay-native.gradle').exists())
        with publish.zipfile.ZipFile(self.stage / 'app/libs/FindTheWay-JNI-rules.jar') as rules:
            self.assertIn(b'-keep class _DeLib.FindTheWayClass',
                          rules.read('META-INF/proguard/FindTheWay.pro'))
        for abi in publish.ABIS:
            for name in ('libfindtheway.so', 'libc++_shared.so'):
                self.assertTrue((self.stage / 'app/src/main/jniLibs' / abi / name).is_file())

    def test_complete_recursive_dependencies(self):
        publish.stage_android(self.stage)
        for name in ['app/src/main/java/_DeLib/Helper.java', 'app/src/main/java/_DeLib/Common.java',
                     'app/src/main/cpp/extra/Additional.cpp', 'app/src/main/cpp/extra/Local.hpp',
                     'app/src/main/cpp/shared/_DeLib/Core.h', 'app/src/main/cpp/shared/_3rdLib/Base64.h',
                     'app/src/main/cpp/third_party/iconv/source.tar.gz']:
            self.assertTrue((self.stage / name).is_file(), name)
        manifest = json.loads((self.stage / 'FindTheWay-package-manifest.json').read_text())
        self.assertTrue(all(publish.digest(self.stage / p) == h for p, h in manifest.items()))

    def test_other_installed_cmake_works_without_sdkmanager(self):
        sdk = self.root / 'Sdk'
        toolchain = sdk / 'ndk' / publish.NDK_VERSION / 'build/cmake/android.toolchain.cmake'
        toolchain.parent.mkdir(parents=True)
        toolchain.touch()
        cmake = sdk / 'cmake/3.31.8/bin/cmake'
        cmake.parent.mkdir(parents=True)
        cmake.touch()
        ninja = cmake.parent / 'ninja'
        ninja.touch()
        result = subprocess.CompletedProcess([], 0, 'cmake version 3.31.8\n', '')
        with patch.dict(publish.os.environ, {'ANDROID_HOME': str(sdk)}, clear=True), \
             patch.object(publish.shutil, 'which', return_value=None), \
             patch.object(publish.subprocess, 'run', return_value=result), \
             patch.object(publish, 'run', side_effect=AssertionError('Must not invoke sdkmanager')):
            self.assertEqual(publish.native_tools(), (toolchain.parents[2], cmake, ninja))
            result.stdout = 'cmake version 3.20.0\n'
            with self.assertRaisesRegex(FileNotFoundError, 'Command-line Tools'):
                publish.native_tools()

    def test_visual_studio_cmake_and_sibling_ninja_are_discovered(self):
        programs = self.root / 'Program Files (x86)'
        vswhere = programs / 'Microsoft Visual Studio/Installer/vswhere.exe'
        vs = self.root / 'Visual Studio'
        base = vs / 'Common7/IDE/CommonExtensions/Microsoft/CMake'
        cmake = base / 'CMake/bin/cmake.exe'
        ninja = base / 'Ninja/ninja.exe'
        for path in (vswhere, cmake, ninja):
            path.parent.mkdir(parents=True, exist_ok=True)
            path.touch()
        outputs = [subprocess.CompletedProcess([], 0, str(vs) + '\n', ''),
                   subprocess.CompletedProcess([], 0, 'cmake version 3.31.8\n', '')]
        with patch.object(publish, 'Path', type(self.root)), \
             patch.object(publish.os, 'name', 'nt'), \
             patch.dict(publish.os.environ, {'ProgramFiles(x86)': str(programs)}), \
             patch.object(publish.shutil, 'which', return_value=None), \
             patch.object(publish.subprocess, 'run', side_effect=outputs):
            self.assertEqual(publish.installed_cmake(self.root / 'Sdk'), (cmake, ninja))

    def test_missing_library_preserves_previous_stage(self):
        self.stage.mkdir()
        (self.stage / 'old').write_text('old')
        self.lib.unlink()
        with self.assertRaises(ValueError):
            publish.stage_android(self.stage)
        self.assertEqual((self.stage / 'old').read_text(), 'old')

    def test_modified_library_is_rejected(self):
        self.lib.write_text('changed')
        with self.assertRaises(ValueError):
            publish.stage_android(self.stage)

    def test_cold_source_download_is_verified(self):
        (self.iconv / 'source.tar.gz').unlink()
        with patch.object(publish.urllib.request, 'urlopen', return_value=io.BytesIO(self.payload)):
            self.assertEqual(publish.source_archive().read_bytes(), self.payload)

    def test_windows_crlf_headers_are_accepted_but_edits_are_rejected(self):
        header = self.iconv / 'arm64-v8a/include/iconv.h'
        header.parent.mkdir(parents=True)
        original = b'/* iconv */\nextern int iconv_open();\n'
        manifest_path = self.iconv / 'manifest.json'
        manifest = json.loads(manifest_path.read_text())
        manifest['files']['arm64-v8a/include/iconv.h'] = hashlib.sha256(original).hexdigest()
        manifest_path.write_text(json.dumps(manifest))
        header.write_bytes(original.replace(b'\n', b'\r\n'))
        publish.stage_android(self.stage)
        self.assertTrue((self.stage / 'app/src/main/cpp/third_party/iconv/arm64-v8a/include/iconv.h').is_file())
        header.write_bytes(header.read_bytes() + b'/* edited */\r\n')
        with self.assertRaisesRegex(ValueError, 'Modified iconv dependency:.*expected SHA-256'):
            publish.source_archive()

    def test_binary_line_endings_are_never_normalized(self):
        manifest_path = self.iconv / 'manifest.json'
        manifest = json.loads(manifest_path.read_text())
        manifest['files']['arm64-v8a/lib/libiconv.a'] = hashlib.sha256(b'archive\n').hexdigest()
        manifest_path.write_text(json.dumps(manifest))
        self.lib.write_bytes(b'archive\r\n')
        with self.assertRaisesRegex(ValueError, 'Modified iconv dependency'):
            publish.source_archive()

    def test_bad_download_does_not_remain_in_cache(self):
        (self.iconv / 'source.tar.gz').unlink()
        with patch.object(publish.urllib.request, 'urlopen', return_value=io.BytesIO(b'wrong')):
            with self.assertRaises(ValueError):
                publish.source_archive()
        self.assertFalse((self.iconv / 'source.tar.gz').exists())
        self.assertFalse((self.iconv / 'source.tar.download').exists())

    def test_missing_cpp_header_fails(self):
        (publish.NATIVE / 'extra/Local.hpp').unlink()
        with self.assertRaises(FileNotFoundError):
            publish.stage_android(self.stage)

if __name__ == '__main__':
    unittest.main()
