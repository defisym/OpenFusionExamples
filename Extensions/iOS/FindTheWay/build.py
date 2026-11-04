#!/usr/bin/env python3
"""Build FindTheWay's arm64 iOS core and prepare a verified Windows handoff."""
from __future__ import annotations
import argparse
import importlib.util
import json
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import tempfile

PROJECT = Path(__file__).resolve().parent
REPO = PROJECT.parents[2]
EXTENSIONS = REPO / 'Extensions'


def publisher_module():
    spec = importlib.util.spec_from_file_location('ftw_ios_publisher',
        EXTENSIONS / '_Script/FindTheWayIOS.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def build(args):
    if platform.system() != 'Darwin' or platform.machine() != 'arm64':
        raise RuntimeError('Run on an Apple Silicon Mac with Xcode selected')
    publisher = publisher_module()
    before = publisher.source_hashes(EXTENSIONS)
    destination = args.output.resolve()
    # Prevent an output replacement from removing source/build inputs.
    if destination == REPO or REPO.is_relative_to(destination) or (destination.is_relative_to(REPO) and not destination.is_relative_to(PROJECT / 'Artifacts')):
        raise ValueError('Output inside the repository must be under this plugin Artifacts directory')
    if destination == PROJECT or PROJECT.is_relative_to(destination):
        raise ValueError('Output must not contain the iOS project')
    build_dir = args.build_dir.resolve()
    if build_dir == destination or build_dir.is_relative_to(destination) or destination.is_relative_to(build_dir):
        raise ValueError('Build and artifact directories must be separate')
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.build-stage-', dir=destination.parent) as temporary:
        bundle = Path(temporary) / 'bundle'
        bundle.mkdir()
        command = [sys.executable, str(EXTENSIONS / 'iOS/FindTheWay/Core/build_xcframework.py'),
                   '--project-source', str(PROJECT), '--build-dir', str(build_dir),
                   '--output', str(bundle / 'FindTheWayCore.xcframework'),
                   '--minimum-ios', args.minimum_ios]
        subprocess.run(command, check=True)
        if publisher.source_hashes(EXTENSIONS) != before:
            raise RuntimeError('Sources changed during compilation; previous artifacts are preserved')
        kind = 'native'
        if args.extension_package:
            shutil.copyfile(args.extension_package, bundle / 'FindTheWay.ext')
            kind = 'release'
        (bundle / 'Integration.md').write_text(
            '# FindTheWay iOS integration\n\n'
            'Build host: Apple Silicon. Slices: iOS arm64 and iOS Simulator arm64.\n'
            f'Minimum iOS selected for this build: {args.minimum_ios}.\n\n'
            'Transfer Artifacts/Current to Windows. RunOnly verifies the core and\n'
            'packages the current Runtime sources into FindTheWay.ext automatically.\n'
            'The extension autolinks the correct device/simulator archive, libc++\n'
            'and iconv using the supplied exporter SRCROOT library search path.\n'
            'The Runtime sources require C++11; shared C++20 stays precompiled.\n\n'
            'This command verifies source/file hashes and packaging metadata.\n'
            'It does not establish Fusion export or final App/device validation.\n',
            encoding='utf-8')
        files = publisher.payload_files(bundle, require_extension=kind == 'release')
        manifest = {'schema': 1, 'kind': kind, 'sources': before,
                    'files': {name: publisher.sha(path) for name, path in files.items()},
                    'build': {'minimum_ios': args.minimum_ios,
                              'xcode': subprocess.check_output(['xcodebuild', '-version'], text=True).strip()}}
        (bundle / 'manifest.json').write_text(json.dumps(manifest, indent=2, sort_keys=True), encoding='utf-8')
        publisher.verify(bundle, EXTENSIONS)
        # Transfer directory replacement has rollback; do not discard the last good build.
        backup = Path(temporary) / 'previous'
        if destination.exists():
            if destination.is_symlink() or not destination.is_dir():
                raise ValueError('Existing output must be a regular artifact directory')
            if not (destination / 'manifest.json').is_file():
                raise ValueError('Refusing to replace a directory without an artifact manifest')
            destination.rename(backup)
        try:
            bundle.rename(destination)
        except BaseException:
            if backup.exists():
                backup.rename(destination)
            raise
    print(f'{kind} artifacts ready: {destination}')
    if kind == 'native':
        print('Core ready; Windows RunOnly can package and install the current iOS Runtime adapter.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--minimum-ios', required=True)
    parser.add_argument('--build-dir', type=Path, default=PROJECT / 'Build')
    parser.add_argument('--output', type=Path, default=PROJECT / 'Artifacts/Current')
    parser.add_argument('--extension-package', type=Path,
                        help='Optional real .ext produced from the same sources; never generated by this command')
    args = parser.parse_args()
    try:
        build(args)
    except (OSError, RuntimeError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'ERROR: {error}\n')


if __name__ == '__main__':
    main()
