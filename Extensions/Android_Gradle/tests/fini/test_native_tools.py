"""Exercise discovery/installation decisions without downloading an SDK in tests."""
import importlib.util
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch,Mock

repo=Path(__file__).resolve().parents[4]
spec=importlib.util.spec_from_file_location('fini_tools',repo/'Extensions/_Script/FiniNativeTools.py')
tools=importlib.util.module_from_spec(spec);spec.loader.exec_module(tools)
config=json.loads((repo/'Extensions/Fini/Mobile/android-toolchain.json').read_text())

def touch(path):
    path.parent.mkdir(parents=True,exist_ok=True);path.write_text('fixture');return path

class NativeToolsTest(unittest.TestCase):
    def test_ready_sdk_never_installs_or_downloads(self):
        with tempfile.TemporaryDirectory() as temp:
            sdk=Path(temp);ndk=sdk/'ndk'/config['ndk'];touch(ndk/'build/cmake/android.toolchain.cmake')
            run=Mock()
            with patch.object(tools,'find_build_tools',return_value=(sdk/'cmake',sdk/'ninja')),patch.object(tools,'bootstrap_sdkmanager') as bootstrap:
                result=tools.prepare(repo,config,run,sdk=sdk)
            self.assertEqual(result['ndk'],ndk);run.assert_not_called();bootstrap.assert_not_called()
    def test_missing_ndk_uses_sdkmanager_and_rechecks_install(self):
        with tempfile.TemporaryDirectory() as temp:
            sdk=Path(temp);manager=touch(sdk/'cmdline-tools/latest/bin/sdkmanager')
            commands=[]
            def run(command):
                commands.append(list(map(str,command)))
                touch(sdk/'ndk'/config['ndk']/'build/cmake/android.toolchain.cmake')
            with patch.object(tools,'find_build_tools',return_value=(sdk/'cmake',sdk/'ninja')),patch.object(tools,'find_sdkmanager',return_value=manager):
                result=tools.prepare(repo,config,run,sdk=sdk)
            self.assertIn('ndk;'+config['ndk'],commands[0]);self.assertNotIn('cmake;'+config['cmake'],commands[0]);self.assertTrue(result['ndk'].is_dir())
    def test_success_exit_without_actual_install_is_failure(self):
        with tempfile.TemporaryDirectory() as temp:
            sdk=Path(temp)
            with patch.object(tools,'find_build_tools',return_value=(sdk/'cmake',sdk/'ninja')),patch.object(tools,'find_sdkmanager',return_value=sdk/'sdkmanager'):
                with self.assertRaisesRegex(RuntimeError,'incomplete'):
                    tools.prepare(repo,config,lambda _:None,sdk=sdk)
    def test_vs_sibling_ninja_is_found(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);cmake=touch(root/'Microsoft/CMake/CMake/bin/cmake');ninja=touch(root/'Microsoft/CMake/Ninja/ninja')
            with patch.object(tools,'executable',side_effect=lambda n:n),patch.object(tools,'capture',side_effect=lambda cmd:'cmake version 3.31.0' if str(cmd[0])==str(cmake) else '1.12'):
                self.assertEqual(tools.find_build_tools(root,cmake=cmake),(cmake,ninja))
    def test_no_install_reports_exact_missing_packages(self):
        with tempfile.TemporaryDirectory() as temp:
            with patch.object(tools,'find_build_tools',return_value=None):
                with self.assertRaisesRegex(RuntimeError,'ndk;'+config['ndk']):
                    tools.prepare(repo,config,Mock(),sdk=Path(temp),install=False)
    def test_selects_pinned_commandline_tools_for_host(self):
        xml='<sdk-repository><remotePackage path="cmdline-tools;19.0"><archives><archive><host-os>linux</host-os><complete><url>commandlinetools-linux-13114758_latest.zip</url><checksum type="sha1">abc</checksum></complete></archive></archives></remotePackage></sdk-repository>'
        self.assertEqual(tools.command_line_archive(xml,'19.0','linux'),('commandlinetools-linux-13114758_latest.zip','sha1','abc'))
        with self.assertRaises(RuntimeError):tools.command_line_archive(xml,'19.0','macosx')
    def test_explicit_invalid_ndk_is_not_silently_replaced(self):
        with tempfile.TemporaryDirectory() as temp:
            with self.assertRaisesRegex(RuntimeError,'Explicit Android NDK'):
                tools.prepare(repo,config,Mock(),sdk=Path(temp),ndk=Path(temp)/'typo')

if __name__=='__main__':unittest.main()
