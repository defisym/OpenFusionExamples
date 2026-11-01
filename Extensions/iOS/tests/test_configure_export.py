"""Owned project fixtures; no private Fusion project metadata is redistributed."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'FindTheWay/configure_export.py'
spec = importlib.util.spec_from_file_location('ftw_configure_export', SCRIPT)
module = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = module
spec.loader.exec_module(module)


class ExportTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.bundle = self.root / 'RenamedGame.xcodeproj'
        self.bundle.mkdir()
        self.ids = {name: f'{index:024X}' for index, name in enumerate(
            ['project', 'app', 'physics', 'appconfigs', 'physicsconfigs', 'projectconfigs',
             'appdebug', 'apprelease', 'physicsdebug', 'projectdebug', 'product', 'physicsproduct',
             'frameworks', 'link'], 1)}
        i = self.ids
        objects = {
            i['project']: dict(isa='PBXProject', buildConfigurationList=i['projectconfigs']),
            i['app']: dict(isa='PBXNativeTarget', name='RenamedGame', productType='com.apple.product-type.application', productReference=i['product'], buildConfigurationList=i['appconfigs'], buildPhases=[i['frameworks']], dependencies=[]),
            i['physics']: dict(isa='PBXNativeTarget', name='Physics', productType='com.apple.product-type.library.static', productReference=i['physicsproduct'], buildConfigurationList=i['physicsconfigs']),
            i['appconfigs']: dict(isa='XCConfigurationList', buildConfigurations=[i['appdebug'], i['apprelease']]),
            i['physicsconfigs']: dict(isa='XCConfigurationList', buildConfigurations=[i['physicsdebug']]),
            i['projectconfigs']: dict(isa='XCConfigurationList', buildConfigurations=[i['projectdebug']]),
            i['product']: dict(isa='PBXFileReference', path='RenamedGame.app'),
            i['physicsproduct']: dict(isa='PBXFileReference', path='libPhysics.a'),
            i['frameworks']: dict(isa='PBXFrameworksBuildPhase', files=[i['link']]),
            i['link']: dict(isa='PBXBuildFile', fileRef=i['physicsproduct'], settings={'ATTRIBUTES': ['Weak', 'OtherAttribute']}),
        }
        for config in ('appdebug', 'apprelease', 'physicsdebug', 'projectdebug'):
            objects[i[config]] = dict(isa='XCBuildConfiguration', name=config, buildSettings={
                'ARCHS': 'x86_64', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]': 'arm64',
                'IPHONEOS_DEPLOYMENT_TARGET': '17.6' if config=='appdebug' else '12.0',
                'OTHER_LDFLAGS': ['$(inherited)', '-framework', 'UIKit', '-framework', 'AVFoundation'],
                'CODE_SIGN_STYLE': 'Manual', 'DEVELOPMENT_TEAM': 'AUTHORTEAM',
                'LIBRARY_SEARCH_PATHS': ['$(inherited)', '$(SRCROOT)/Other'],
                'CUSTOM_SETTING': 'Preserved'})
        self.path = self.bundle / 'project.pbxproj'
        self.path.write_text('// !$*UTF8*$!\n' + module.render(dict(objects=objects, rootObject=i['project'])))
        schemes = self.bundle / 'xcshareddata/xcschemes'
        schemes.mkdir(parents=True)
        self.scheme = schemes / 'Old.xcscheme'
        self.scheme.write_text('<Scheme><LaunchAction><BuildableReference BlueprintIdentifier="'+i['app']+'" ReferencedContainer="container:Old.xcodeproj" BuildableName="Old.app" BlueprintName="Old">\n</BuildableReference></LaunchAction></Scheme>')
        for platform in ('iphoneos', 'iphonesimulator'):
            (self.root / f'libFindTheWayCore_{platform}.a').write_bytes(b'!<arch>\nfixture')

    def apply(self):
        results, floor = module.prepare(self.bundle)
        for path, data in results.items():
            module.write_atomic(path, data)
        return results, floor

    def test_renamed_targets_architectures_linkage_signing_and_schemes(self):
        changes, floor = self.apply()
        self.assertEqual(floor, '17.6')
        project = module.Project(self.path.read_text())
        for config in ('appdebug', 'apprelease', 'physicsdebug', 'projectdebug'):
            settings = project.plain(self.ids[config])['buildSettings']
            self.assertEqual(settings['ARCHS'], 'arm64')
            self.assertNotIn('EXCLUDED_ARCHS[sdk=iphonesimulator*]', settings)
            self.assertEqual(settings['IPHONEOS_DEPLOYMENT_TARGET'], '17.6')
            self.assertEqual(settings['CUSTOM_SETTING'], 'Preserved')
            self.assertEqual(settings['DEVELOPMENT_TEAM'], 'AUTHORTEAM')
        settings = project.plain(self.ids['appdebug'])['buildSettings']
        self.assertIn('$(SRCROOT)/Other', settings['LIBRARY_SEARCH_PATHS'])
        self.assertIn('-lFindTheWayCore_iphonesimulator', settings['OTHER_LDFLAGS[sdk=iphonesimulator*]'])
        self.assertEqual(settings['OTHER_LDFLAGS[sdk=iphonesimulator*]'].count('-framework'), 2)
        self.assertEqual(project.plain(self.ids['link'])['settings']['ATTRIBUTES'], ['OtherAttribute'])
        dep = project.plain(self.ids['app'])['dependencies'][0]
        self.assertEqual(project.plain(dep)['target'], self.ids['physics'])
        self.assertIn('container:RenamedGame.xcodeproj', self.scheme.read_text())
        self.assertIn('BuildableName="RenamedGame.app"', self.scheme.read_text())

    def test_second_run_is_byte_identical(self):
        self.apply()
        before = self.path.read_bytes(), self.scheme.read_bytes()
        changes, _ = self.apply()
        self.assertFalse(changes)
        self.assertEqual(before, (self.path.read_bytes(), self.scheme.read_bytes()))

    def test_missing_core_preserves_project(self):
        before = self.path.read_bytes()
        (self.root / 'libFindTheWayCore_iphoneos.a').unlink()
        with self.assertRaisesRegex(ValueError, 'archive'):
            self.apply()
        self.assertEqual(before, self.path.read_bytes())

    def test_cli_dry_run_preserves_files(self):
        before = self.path.read_bytes(), self.scheme.read_bytes()
        result = subprocess.run([sys.executable, str(SCRIPT), str(self.root), '--dry-run'], capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preview: arm64', result.stdout)
        self.assertEqual(before, (self.path.read_bytes(), self.scheme.read_bytes()))


if __name__ == '__main__':
    unittest.main()
