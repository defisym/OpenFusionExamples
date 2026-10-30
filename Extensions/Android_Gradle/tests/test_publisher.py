"""Packaging regression tests; no SDK, NDK or network required."""
import hashlib
import importlib.util
import io
import json
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

    def test_complete_recursive_dependencies(self):
        publish.stage_android(self.stage)
        for name in ['app/src/main/java/_DeLib/Helper.java', 'app/src/main/java/_DeLib/Common.java',
                     'app/src/main/cpp/extra/Additional.cpp', 'app/src/main/cpp/extra/Local.hpp',
                     'app/src/main/cpp/shared/_DeLib/Core.h', 'app/src/main/cpp/shared/_3rdLib/Base64.h',
                     'app/src/main/cpp/third_party/iconv/source.tar.gz']:
            self.assertTrue((self.stage / name).is_file(), name)
        manifest = json.loads((self.stage / 'FindTheWay-package-manifest.json').read_text())
        self.assertTrue(all(publish.digest(self.stage / p) == h for p, h in manifest.items()))

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
