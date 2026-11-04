"""Project generation checks; never presented as an Apple compiler/link test."""
import importlib.util
from pathlib import Path
import tempfile
import unittest
import xml.etree.ElementTree as ET

root=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('fini_xcode',root/'generate_project.py')
generator=importlib.util.module_from_spec(spec);spec.loader.exec_module(generator)

class ProjectTest(unittest.TestCase):
    def test_model_targets_paths_and_references(self):
        model=generator.build_model();objects=model['objects']
        self.assertIn(model['rootObject'],objects)
        references={'mainGroup','productRefGroup','buildConfigurationList','productReference','baseConfigurationReference','fileRef'}
        reference_lists={'children','targets','buildPhases','buildConfigurations','files'}
        for obj in objects.values():
            for key,value in obj.items():
                if key in references:self.assertIn(value,objects)
                if key in reference_lists:
                    for ref in value:self.assertIn(ref,objects)
            if obj['isa']=='PBXFileReference' and obj['sourceTree']=='SOURCE_ROOT':
                self.assertTrue((root/obj['path']).is_file(),obj['path'])
        native=objects[generator.uid('Fini')]
        self.assertEqual(native['productType'],'com.apple.product-type.library.static')
        self.assertEqual(objects[generator.uid('FiniXCFramework')]['isa'],'PBXAggregateTarget')
        script=objects[generator.uid('package')]['shellScript']
        self.assertIn('"$SRCROOT/build.py"',script);self.assertTrue((root/'build.py').is_file())
        self.assertIn('FINI_OPENSSL_BASE',script)
    def test_generation_is_deterministic_and_schemes_match(self):
        with tempfile.TemporaryDirectory() as temp:
            output=Path(temp);project=generator.generate(output)
            first=(project/'project.pbxproj').read_bytes();generator.generate(output)
            self.assertEqual(first,(project/'project.pbxproj').read_bytes())
            self.assertEqual(first,(root/'Fini.xcodeproj/project.pbxproj').read_bytes())
            for target in ('Fini','FiniXCFramework'):
                scheme=ET.parse(project/f'xcshareddata/xcschemes/{target}.xcscheme')
                ref=scheme.find('.//BuildableReference')
                self.assertEqual(ref.attrib['BlueprintIdentifier'],generator.uid(target))
                self.assertEqual(ref.attrib['ReferencedContainer'],'container:Fini.xcodeproj')
    def test_local_path_validation(self):
        with self.assertRaises(ValueError):generator.xcpath(Path('/tmp/$(command)'))

if __name__=='__main__':unittest.main()
