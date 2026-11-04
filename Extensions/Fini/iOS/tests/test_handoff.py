"""Mac/Windows handoff fixtures, not Apple compilation or exporter validation."""
import importlib.util
import json
from pathlib import Path
import plistlib
import tempfile
import unittest
import zipfile

repo=Path(__file__).resolve().parents[4]
spec=importlib.util.spec_from_file_location('apple_artifacts',repo/'Extensions/_DeLib/Encryption/Mobile/apple_artifacts.py')
apple=importlib.util.module_from_spec(spec);spec.loader.exec_module(apple)

def framework(root,owner):
    path=root/(owner+'.xcframework');path.mkdir()
    libraries=[{'LibraryIdentifier':'ios-arm64','LibraryPath':'lib'+owner+'.a','SupportedPlatform':'ios','SupportedArchitectures':['arm64']},{'LibraryIdentifier':'ios-arm64_x86_64-simulator','LibraryPath':'lib'+owner+'.a','SupportedPlatform':'ios','SupportedPlatformVariant':'simulator','SupportedArchitectures':['arm64','x86_64']}]
    (path/'Info.plist').write_bytes(plistlib.dumps({'AvailableLibraries':libraries}))
    for lib in libraries:
        folder=path/lib['LibraryIdentifier'];folder.mkdir();(folder/lib['LibraryPath']).write_bytes(b'fixture archive, not a compiled library')
    return path

class HandoffTest(unittest.TestCase):
    def test_mac_stage_and_windows_package_without_xcode(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);(root/'source.cpp').write_text('source\n')
            native=framework(root,'Fini');prebuilt=root/'Prebuilt'
            apple.stage(native,'Fini',['source.cpp'],prebuilt,requires=['OFEEncryption/1'],repo=root)
            validated=apple.validate(prebuilt,'Fini',root)
            output=root/'Fini-iOS.zip';apple.package(validated,output)
            with zipfile.ZipFile(output) as zipped:
                self.assertEqual(set(zipped.namelist()),{'Fini.xcframework.zip','manifest.json'})
                self.assertEqual(json.loads(zipped.read('manifest.json'))['requires'],['OFEEncryption/1'])
            (root/'source.cpp').write_bytes(b'source\r\n')
            self.assertIsNotNone(apple.validate(prebuilt,'Fini',root))
    def test_stale_and_corrupt_artifacts_do_not_replace_output(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);source=root/'source.cpp';source.write_text('version1')
            prebuilt=root/'Prebuilt';apple.stage(framework(root,'Fini'),'Fini',['source.cpp'],prebuilt,repo=root)
            output=root/'existing.zip';output.write_bytes(b'known good')
            source.write_text('version2')
            with self.assertRaisesRegex(ValueError,'stale'):apple.validate(prebuilt,'Fini',root)
            self.assertEqual(output.read_bytes(),b'known good')
            source.write_text('version1');(prebuilt/'Fini.xcframework.zip').write_bytes(b'bad')
            with self.assertRaisesRegex(ValueError,'Corrupt'):apple.validate(prebuilt,'Fini',root)
    def test_missing_and_incomplete_handoff(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);self.assertIsNone(apple.validate(root,'Fini'))
            (root/'manifest.json').write_text('{}')
            with self.assertRaisesRegex(ValueError,'Incomplete'):apple.validate(root,'Fini')
    def test_public_and_client_artifacts_are_separate(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);(root/'source.cpp').write_text('source')
            for owner in ('Fini','OFEEncryption'):
                apple.stage(framework(root,owner),owner,['source.cpp'],root/owner,repo=root)
                output=root/(owner+'.zip');apple.package(apple.validate(root/owner,owner,root),output)
                with zipfile.ZipFile(output) as zipped:
                    self.assertIn(owner+'.xcframework.zip',zipped.namelist())
                    other='OFEEncryption' if owner=='Fini' else 'Fini'
                    self.assertNotIn(other+'.xcframework.zip',zipped.namelist())

if __name__=='__main__':unittest.main()
