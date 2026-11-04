"""Owned native/archive fixtures; validates packaging, never an Apple build."""
import importlib.util
import contextlib
import io
from pathlib import Path
import plistlib
import sys
import tempfile
import unittest
import zipfile
from unittest.mock import patch

repo=Path(__file__).resolve().parents[4]
sys.path.insert(0,str(repo/'Extensions/_Script'))
import FiniIOS
import Fini as publisher
spec=importlib.util.spec_from_file_location('fini_configure',repo/'Extensions/iOS/Fini/configure_export.py')
configure=importlib.util.module_from_spec(spec);spec.loader.exec_module(configure)
sys.path.insert(0,str(repo/'Extensions/iOS/Fini'))
import generate_project
from xcode_project import Project,render


def artifact(second_header=b'// interface\r\n'):
    output=io.BytesIO()
    libs=[dict(LibraryIdentifier='ios-arm64',LibraryPath='libFini.a',HeadersPath='Headers',SupportedPlatform='ios',SupportedArchitectures=['arm64']),dict(LibraryIdentifier='ios-arm64-simulator',LibraryPath='libFini.a',HeadersPath='Headers',SupportedPlatform='ios',SupportedPlatformVariant='simulator',SupportedArchitectures=['arm64'])]
    with zipfile.ZipFile(output,'w') as zipped:
        zipped.writestr('Fini.xcframework/Info.plist',plistlib.dumps({'AvailableLibraries':libs}))
        for index,lib in enumerate(libs):
            prefix='Fini.xcframework/'+lib['LibraryIdentifier']+'/'
            zipped.writestr(prefix+'libFini.a',b'!<arch>\nowned binary fixture\r\n')
            zipped.writestr(prefix+'Headers/CRunFini.h',second_header if index else b'// interface\n')
    return {'owner':'Fini'},output.getvalue()


class ExportTests(unittest.TestCase):
    def test_real_ext_framing_and_current_configurator_are_packaged(self):
        with tempfile.TemporaryDirectory() as temp:
            path=Path(temp)/'Fini.ext';FiniIOS.write(artifact(),path)
            contents=FiniIOS.codec().read_extension(path.read_bytes())
            self.assertIn('Classes/Extensions/CRunFini.m',contents)
            self.assertIn('libFini_iphoneos.a',contents);self.assertIn('libFini_iphonesimulator.a',contents)
            self.assertIn('configure_fini_export.py',contents);self.assertIn('xcode_project.py',contents)
            self.assertEqual(len(contents['project.txt'].split(b'\0')),7)
            self.assertFalse(any('OFEEncryption.framework' in key for key in contents))
            self.assertIn(b'.linker_option \\"-lFini_iphoneos\\"',contents['Classes/Extensions/CRunFini.m'])
    def test_changed_header_does_not_replace_previous_ext(self):
        with tempfile.TemporaryDirectory() as temp:
            output=Path(temp)/'Fini.ext';output.write_bytes(b'previous')
            with self.assertRaisesRegex(ValueError,'headers differ'):FiniIOS.write(artifact(b'// changed interface\n'),output)
            self.assertEqual(output.read_bytes(),b'previous')
    def test_actual_publisher_preserves_all_outputs_on_bad_native_headers(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);extension=root/'Extensions';config=extension/'iOS/Fini';config.mkdir(parents=True)
            (config/'Config').mkdir();(config/'Config/FiniConsumer.xcconfig').write_text('// owned fixture')
            (config/'README.md').write_text('owned fixture')
            runtime=extension/'Fini/ToInstall/Files/Data/Runtime/iPhone';runtime.mkdir(parents=True)
            previous=runtime/'Fini-iOS.zip';previous.write_bytes(b'known good client package')
            common_output=extension/'_DeLib/Encryption/Mobile/ToInstall';common_output.mkdir(parents=True)
            previous_common=common_output/'OFEEncryption-iOS.zip';previous_common.write_bytes(b'known good common package')
            bad=artifact(b'// different interface\n')
            manifest=dict(bad[0],file='Fini.xcframework.zip',requires=['OFEEncryption/1'])
            common=(dict(owner='OFEEncryption',file='OFEEncryption.xcframework.zip'),b'owned common fixture')
            with patch.object(publisher,'EXT',extension),patch.object(publisher,'REPO',root),patch.object(publisher.apple_artifacts,'validate',side_effect=[(manifest,bad[1]),common]):
                with self.assertRaisesRegex(ValueError,'headers differ'):publisher.sync_apple()
            self.assertEqual(previous.read_bytes(),b'known good client package')
            self.assertEqual(previous_common.read_bytes(),b'known good common package')

    def test_export_configuration_dry_run_preservation_and_idempotency(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);bundle=root/'Renamed.xcodeproj';bundle.mkdir()
            model=generate_project.build_model();objects=model['objects'];app=objects[generate_project.uid('Fini')]
            app.update(name='App',productType='com.apple.product-type.application')
            objects[generate_project.uid('product')]['path']='App.app'
            for key in objects[app['buildConfigurationList']]['buildConfigurations']:
                objects[key]['buildSettings'].update(DEVELOPMENT_TEAM='keep-team',IPHONEOS_DEPLOYMENT_TARGET='18.0',OTHER_LDFLAGS=['-custom'])
            project=bundle/'project.pbxproj';project.write_text(render(model));before=project.read_bytes()
            for sdk in ('iphoneos','iphonesimulator'):(root/f'libFini_{sdk}.a').write_bytes(b'!<arch>\nfixture')
            common=root/'Common.xcframework';common.mkdir()
            (common/'Info.plist').write_bytes(plistlib.dumps({'AvailableLibraries':[dict(SupportedPlatform='ios',SupportedArchitectures=['arm64']),dict(SupportedPlatform='ios',SupportedPlatformVariant='simulator',SupportedArchitectures=['arm64'])]}))
            with contextlib.redirect_stdout(io.StringIO()) as preview:
                self.assertGreater(configure.configure(bundle,common,dry_run=True),0)
            self.assertIn('OTHER_LDFLAGS',preview.getvalue())
            self.assertEqual(project.read_bytes(),before);self.assertFalse((root/'Frameworks').exists())
            self.assertGreater(configure.configure(bundle,common),0)
            first=project.read_bytes();self.assertEqual(configure.configure(bundle,common),0);self.assertEqual(project.read_bytes(),first)
            parsed=Project(first.decode());settings=[parsed.plain(key)['buildSettings'] for key in objects[app['buildConfigurationList']]['buildConfigurations']]
            for value in settings:
                self.assertEqual(value['DEVELOPMENT_TEAM'],'keep-team');self.assertEqual(value['IPHONEOS_DEPLOYMENT_TARGET'],'18.0')
                self.assertIn('-custom',value['OTHER_LDFLAGS[sdk=iphoneos*]'])
            self.assertEqual(sum(parsed.plain(key).get('isa')=='PBXCopyFilesBuildPhase' for key in parsed.objects),1)
            header=common/'public.h';header.write_bytes(b'interface\n')
            destination=root/'Frameworks/OFEEncryption.xcframework/public.h';destination.write_bytes(b'interface\r\n')
            self.assertEqual(configure.configure(bundle,common),0)
            (common/'modified').write_text('different')
            with self.assertRaisesRegex(ValueError,'overwrite'):configure.configure(bundle,common)
            self.assertEqual(project.read_bytes(),first)

if __name__=='__main__':unittest.main()
