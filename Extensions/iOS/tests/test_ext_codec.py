import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import zipfile

SCRIPT = Path(__file__).resolve().parents[1] / 'Tools/make_ext.py'
spec = importlib.util.spec_from_file_location('make_ext', SCRIPT)
codec = importlib.util.module_from_spec(spec)
spec.loader.exec_module(codec)


class ExtCodecTests(unittest.TestCase):
    def setUp(self):
        self.files = {'Classes/Extensions/CRunProbe.h': b'// owned header fixture\n',
                      'Classes/Extensions/CRunProbe.m': b'// owned source fixture\n',
                      'Classes/Extensions/ProbeHelper.mm': b'// owned helper fixture\n',
                      'libProbe_iphoneos.a': b'!<arch>\nowned fixture'}

    def test_standard_zip_reader_accepts_encryption_and_payload(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary) / 'Probe.ext'
            codec.write_extension('Probe', self.files, output)
            self.assertNotEqual(output.read_bytes()[:4], b'PK\x03\x04')
            with zipfile.ZipFile(io.BytesIO(codec.transform(output.read_bytes()))) as archive:
                archive.setpassword(codec.ZIP_PASSWORD)
                self.assertIsNone(archive.testzip())
                for name, data in self.files.items():
                    self.assertEqual(archive.read(name), data)
            metadata = codec.read_extension(output.read_bytes())['project.txt'].split(b'\0')
            self.assertEqual(len(metadata), 7)
            self.assertEqual(metadata[-1], b'')
            self.assertIn(b'CRunProbe.m in Sources', metadata[0])
            self.assertIn(b'ProbeHelper.mm in Sources', metadata[3])
            self.assertIn(b'CRunProbe.h', metadata[4])

    def test_bad_input_preserves_previous_package(self):
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary) / 'Probe.ext'
            output.write_bytes(b'previous package')
            for name, files in [('bad name', self.files), ('Probe', {'../outside': b'bad'})]:
                with self.assertRaises(ValueError):
                    codec.write_extension(name, files, output)
                self.assertEqual(output.read_bytes(), b'previous package')

    def test_cli_collects_only_the_named_runtime_pair(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            source = folder / 'RuntimeIPhone.zip'
            with zipfile.ZipFile(source, 'w') as archive:
                for name, data in self.files.items():
                    archive.writestr('RuntimeIPhone/' + name, data)
                archive.writestr('RuntimeIPhone/Classes/PrivateSDK.m', b'private fixture')
            output = folder / 'Probe.ext'
            subprocess.run([sys.executable, str(SCRIPT), '--name', 'Probe',
                            '--project-zip', str(source), '--output', str(output)], check=True, capture_output=True)
            files = codec.read_extension(output.read_bytes())
            self.assertEqual(set(files), {'Classes/Extensions/CRunProbe.h',
                                          'Classes/Extensions/CRunProbe.m', 'project.txt'})


if __name__ == '__main__':
    unittest.main()
