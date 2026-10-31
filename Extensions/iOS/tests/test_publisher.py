import importlib.util
import json
import sys
import io
from contextlib import redirect_stdout
from unittest.mock import patch
from pathlib import Path
import plistlib
import tempfile
import unittest
import zipfile

SCRIPT = Path(__file__).resolve().parents[2] / '_Script/FindTheWayIOS.py'
spec = importlib.util.spec_from_file_location('ios_publisher', SCRIPT)
publisher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(publisher)


class PublisherTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / 'Extensions'
        for name in ('iOS/FindTheWay/Runtime/CRunFindTheWay.h',
                     'iOS/FindTheWay/Runtime/CRunFindTheWay.m',
                     'iOS/FindTheWay/Runtime/FTWObjectAdapter.mm',
                     'iOS/FindTheWay/Runtime/FTWPlatformAdapter.mm',
                     'iOS/Helpers/FTWObjectHelpers.h',
                     'iOS/Helpers/FTWObjectHelpers.mm',
                     'iOS/FindTheWay/Core/CMakeLists.txt',
                     'iOS/FindTheWay/Core/build_xcframework.py'):
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text('// source fixture')
        self.bundle = Path(self.temp.name) / 'bundle'
        self.bundle.mkdir()
        # Synthetic packaging fixtures, not executable extension/binary evidence.
        (self.bundle / 'FindTheWay.ext').write_bytes(b'extension fixture')
        (self.bundle / 'Integration.md').write_text('integration fixture\n')
        self.libraries = []
        for variant in ('device', 'simulator'):
            name = 'ios-arm64' + ('-simulator' if variant == 'simulator' else '')
            library = self.bundle / 'FindTheWayCore.xcframework' / name / 'libFindTheWayCore.a'
            library.parent.mkdir(parents=True)
            library.write_bytes(b'!<arch>\narchive fixture')
            headers = library.parent / 'Headers'
            headers.mkdir()
            (headers / 'FindTheWayCore.h').write_text('// ABI fixture\n')
            entry = dict(LibraryIdentifier=name, LibraryPath=library.name, HeadersPath='Headers',
                         SupportedPlatform='ios', SupportedArchitectures=['arm64'])
            if variant == 'simulator':
                entry['SupportedPlatformVariant'] = variant
            self.libraries.append(entry)
        self.info = self.bundle / 'FindTheWayCore.xcframework/Info.plist'
        self.info.write_bytes(plistlib.dumps({'AvailableLibraries': self.libraries}))
        self.record()

    def record(self):
        files = publisher.payload_files(self.bundle)
        (self.bundle / 'manifest.json').write_text(json.dumps({
            'schema': 1, 'sources': publisher.source_hashes(self.root),
            'files': {name: publisher.sha(path) for name, path in files.items()}}))

    def test_valid_staging_and_support_archive(self):
        publisher.stage(self.bundle, self.root)
        install = self.root / 'FindTheWay/ToInstall/Files'
        contents = publisher.extension_codec().read_extension((install / 'Data/Runtime/iPhone/FindTheWay.ext').read_bytes())
        self.assertIn('libFindTheWayCore_iphoneos.a', contents)
        self.assertIn('Classes/Extensions/FTWObjectAdapter.mm', contents)
        self.assertIn('Classes/Extensions/FTWPlatformAdapter.mm', contents)
        self.assertIn('Classes/Extensions/FTWObjectHelpers.mm', contents)
        self.assertEqual(contents['project.txt'].count(b'in Sources */ ='), 4)
        self.assertIn('libFindTheWayCore_iphonesimulator.a', contents)
        self.assertEqual(contents['Classes/Extensions/CRunFindTheWay.m'], b'// source fixture')
        with zipfile.ZipFile(install / 'Tools/FindTheWay-iOS.zip') as archive:
            self.assertIn('FindTheWayCore.xcframework/Info.plist', archive.namelist())
            self.assertIn('Integration.md', archive.namelist())
            self.assertNotIn('FindTheWay.ext', archive.namelist())

    def test_stale_source_preserves_previous_artifact(self):
        publisher.stage(self.bundle, self.root)
        output = self.root / 'FindTheWay/ToInstall/Files/Data/Runtime/iPhone/FindTheWay.ext'
        before = output.read_bytes()
        (self.root / 'iOS/FindTheWay/Runtime/CRunFindTheWay.m').write_text('// changed')
        with self.assertRaisesRegex(ValueError, 'stale'):
            publisher.stage(self.bundle, self.root)
        self.assertEqual(output.read_bytes(), before)

    def test_native_bundle_builds_extension_without_rebuilding_core(self):
        (self.bundle / 'FindTheWay.ext').unlink()
        files = publisher.payload_files(self.bundle, require_extension=False)
        (self.bundle / 'manifest.json').write_text(json.dumps({
            'schema': 1, 'kind': 'native', 'sources': publisher.source_hashes(self.root),
            'files': {name: publisher.sha(path) for name, path in files.items()}}))
        publisher.verify(self.bundle, self.root)
        with patch.object(publisher, 'EXTENSIONS', self.root), patch.object(
                sys, 'argv', ['publisher', '--verify-only', '--artifacts', str(self.bundle)]), redirect_stdout(io.StringIO()) as output:
            publisher.main()
        self.assertIn('Verified native artifacts', output.getvalue())
        self.assertFalse((self.root / 'FindTheWay/ToInstall').exists())
        # Runtime-only changes must not require another Apple core build.
        runtime = self.root / 'iOS/FindTheWay/Runtime/CRunFindTheWay.m'
        runtime.write_text('// updated Runtime adapter')
        publisher.stage(self.bundle, self.root)
        output = self.root / 'FindTheWay/ToInstall/Files/Data/Runtime/iPhone/FindTheWay.ext'
        contents = publisher.extension_codec().read_extension(output.read_bytes())
        self.assertEqual(contents['Classes/Extensions/CRunFindTheWay.m'], b'// updated Runtime adapter')

    def test_windows_line_endings_do_not_invalidate_sources(self):
        source = self.root / 'iOS/FindTheWay/Runtime/CRunFindTheWay.m'
        source.write_bytes(b'// source\n// fixture\n')
        self.record()
        source.write_bytes(b'// source\r\n// fixture\r\n')
        publisher.verify(self.bundle, self.root)

    def test_windows_line_endings_do_not_invalidate_artifact_text(self):
        # Mac-generated XML/header/docs are Git text, unlike static libraries.
        for path in self.bundle.rglob('*'):
            if path.is_file() and (path.suffix in {'.h', '.plist'} or path.name == 'Integration.md'):
                data = path.read_bytes()
                self.assertIn(b'\n', data)
                path.write_bytes(data.replace(b'\n', b'\r\n'))
        publisher.verify(self.bundle, self.root)
        publisher.stage(self.bundle, self.root)

    def test_mixed_header_line_endings_are_equivalent(self):
        header = next(self.bundle.rglob('*.h'))
        header.write_bytes(header.read_bytes().replace(b'\n', b'\r\n'))
        publisher.stage(self.bundle, self.root)
        file = self.root / 'FindTheWay/ToInstall/Files/Data/Runtime/iPhone/FindTheWay.ext'
        contents = publisher.extension_codec().read_extension(file.read_bytes())
        self.assertEqual(contents['Classes/Extensions/FindTheWayCore.h'], b'// ABI fixture\n')

    def test_artifact_text_edits_are_still_rejected(self):
        header = next(self.bundle.rglob('*.h'))
        header.write_bytes(header.read_bytes().replace(b'fixture', b'changed'))
        with self.assertRaisesRegex(ValueError, 'SHA-256 mismatch.*FindTheWayCore.h'):
            publisher.verify(self.bundle, self.root)

    def test_binary_line_endings_are_not_normalized(self):
        library = next(self.bundle.rglob('*.a'))
        library.write_bytes(library.read_bytes() + b'\r\n')
        with self.assertRaisesRegex(ValueError, 'SHA-256 mismatch.*libFindTheWayCore.a'):
            publisher.verify(self.bundle, self.root)

    def test_producer_project_changes_invalidate_bundle(self):
        project = self.root / 'iOS/FindTheWay'
        project.mkdir(parents=True, exist_ok=True)
        (project / 'CMakeLists.txt').write_text('# producer fixture')
        self.record()
        (project / 'CMakeLists.txt').write_text('# changed configuration')
        with self.assertRaisesRegex(ValueError, 'stale'):
            publisher.verify(self.bundle, self.root)

    def test_install_requires_real_runtime_directory(self):
        with self.assertRaisesRegex(ValueError, 'Fusion iPhone runtime directory'):
            publisher.stage(self.bundle, self.root, Path(self.temp.name) / 'MissingFusion')
        fusion = Path(self.temp.name) / 'Fusion'
        target = fusion / 'Data/Runtime/iPhone'
        target.mkdir(parents=True)
        publisher.stage(self.bundle, self.root, fusion)
        self.assertTrue((target / 'FindTheWay.ext').is_file())
        self.assertIn('project.txt', publisher.extension_codec().read_extension((target / 'FindTheWay.ext').read_bytes()))

    def test_tampered_binary_rejected(self):
        library = next(self.bundle.rglob('*.a'))
        library.write_bytes(library.read_bytes() + b'changed')
        with self.assertRaisesRegex(ValueError, 'SHA-256'):
            publisher.verify(self.bundle, self.root)

    def test_intel_slice_rejected(self):
        self.libraries[1]['SupportedArchitectures'] = ['arm64', 'x86_64']
        self.info.write_bytes(plistlib.dumps({'AvailableLibraries': self.libraries}))
        with self.assertRaisesRegex(ValueError, 'arm64'):
            publisher.verify(self.bundle, self.root)

    def test_sdk_payload_rejected(self):
        (self.bundle / 'CRun.m').write_text('private SDK fixture')
        with self.assertRaisesRegex(ValueError, 'Unexpected artifact'):
            publisher.verify(self.bundle, self.root)


if __name__ == '__main__':
    unittest.main()
