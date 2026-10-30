"""Command orchestration fixtures; no Apple compilation is claimed by these tests."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / 'FindTheWayCore/build_xcframework.py'
spec = importlib.util.spec_from_file_location('ftw_build_core', SCRIPT)
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)


class BuildScriptTests(unittest.TestCase):
    def test_arm64_cli_build_and_framework_handoff(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            core = root / 'shared'
            core.mkdir()
            (core / 'FindTheWayFusion.h').touch()
            commands = []
            def run(command, **kwargs):
                commands.append(command)
                if command[:2] == ['cmake', '--build']:
                    archive = Path(command[2]) / 'archives/libFindTheWayCore.a'
                    archive.parent.mkdir(parents=True)
                    archive.write_bytes(b'!<arch>\nfixture')
                elif command[:2] == ['xcodebuild', '-create-xcframework']:
                    Path(command[-1]).mkdir()
                return subprocess.CompletedProcess(command, 0)
            argv = [str(SCRIPT), '--core-include', str(core), '--build-dir', str(root / 'Build'),
                    '--output', str(root / 'Result.xcframework'), '--minimum-ios', '17.0']
            with patch.object(sys, 'argv', argv), \
                 patch.object(builder.platform, 'system', return_value='Darwin'), \
                 patch.object(builder.platform, 'machine', return_value='arm64'), \
                 patch.object(builder.shutil, 'which', return_value='/fixture/tool'), \
                 patch.object(builder.subprocess, 'check_output', side_effect=['/SDK/device\n', '/SDK/simulator\n']), \
                 patch.object(builder.subprocess, 'run', side_effect=run):
                builder.main()
            configurations = [c for c in commands if c[:2] == ['cmake', '-S']]
            self.assertEqual(len(configurations), 2)
            for command in configurations:
                self.assertEqual(command[command.index('-G')+1], 'Ninja')
                self.assertIn('-DCMAKE_OSX_ARCHITECTURES=arm64', command)
                self.assertIn('-DBUILD_TESTING=OFF', command)
            self.assertIn('-DCMAKE_OSX_SYSROOT=/SDK/device', configurations[0])
            self.assertIn('-DCMAKE_OSX_SYSROOT=/SDK/simulator', configurations[1])
            self.assertTrue((root / 'Result.xcframework').is_dir())
            self.assertEqual(commands[-1].count('-library'), 2)

    def test_failed_build_preserves_existing_framework(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            core = root / 'shared'
            core.mkdir()
            (core / 'FindTheWayFusion.h').touch()
            output = root / 'Good.xcframework'
            output.mkdir()
            sentinel = output / 'sentinel'
            sentinel.write_bytes(b'previous build')
            argv = [str(SCRIPT), '--core-include', str(core), '--build-dir', str(root / 'Build'),
                    '--output', str(output), '--minimum-ios', '17.0']
            with patch.object(sys, 'argv', argv), \
                 patch.object(builder.platform, 'system', return_value='Darwin'), \
                 patch.object(builder.platform, 'machine', return_value='arm64'), \
                 patch.object(builder.shutil, 'which', return_value='/fixture/tool'), \
                 patch.object(builder.subprocess, 'run') as run, \
                 patch.object(sys, 'stderr'):
                with self.assertRaises(SystemExit):
                    builder.main()
                run.assert_not_called()
            self.assertEqual(sentinel.read_bytes(), b'previous build')


if __name__ == '__main__':
    unittest.main()
