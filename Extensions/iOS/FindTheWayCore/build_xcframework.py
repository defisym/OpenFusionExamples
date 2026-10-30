#!/usr/bin/env python3
"""Build separate arm64 iOS/device and simulator archives on Apple Silicon."""
import argparse
import pathlib
import platform
import shutil
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--core-include', type=pathlib.Path,
                        default=pathlib.Path(__file__).resolve().parents[2] / '_DeLib')
    parser.add_argument('--project-source', type=pathlib.Path,
                        default=pathlib.Path(__file__).resolve().parent,
                        help='CMake entry project; defaults to the standalone core')
    parser.add_argument('--build-dir', type=pathlib.Path, required=True)
    parser.add_argument('--output', type=pathlib.Path, required=True)
    parser.add_argument('--minimum-ios', required=True,
                        help='Explicit minimum OS; must satisfy actual libc++ availability')
    args = parser.parse_args()
    if platform.system() != 'Darwin' or platform.machine() != 'arm64':
        parser.error('Run on an Apple Silicon Mac with Xcode selected')
    for tool in ('xcodebuild', 'xcrun', 'cmake'):
        if not shutil.which(tool):
            parser.error(f'Missing tool: {tool}')
    source = pathlib.Path(__file__).resolve().parent
    core = args.core_include.resolve()
    if not (core / 'FindTheWayFusion.h').is_file():
        parser.error('Shared headers must include FindTheWayFusion.h from current master')
    output = args.output.resolve()
    if output.suffix != '.xcframework' or output.exists():
        parser.error('Output must be a new .xcframework path; existing output is preserved')
    if not args.minimum_ios or any(c not in '0123456789.' for c in args.minimum_ios):
        parser.error('minimum-ios must be a numeric version')
    archives = []
    generator = 'Ninja' if shutil.which('ninja') else 'Unix Makefiles'
    for sdk in ('iphoneos', 'iphonesimulator'):
        sdk_path = subprocess.check_output(
            ['xcrun', '--sdk', sdk, '--show-sdk-path'], text=True).strip()
        build = args.build_dir.resolve() / sdk
        subprocess.run(['cmake', '-S', str(args.project_source.resolve()), '-B', str(build), '-G', generator,
                        '-DCMAKE_BUILD_TYPE=Release',
                        '-DCMAKE_SYSTEM_NAME=iOS', '-DCMAKE_OSX_ARCHITECTURES=arm64',
                        '-DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY',
                        f'-DCMAKE_OSX_SYSROOT={sdk_path}',
                        f'-DCMAKE_OSX_DEPLOYMENT_TARGET={args.minimum_ios}',
                        f'-DFTW_CORE_INCLUDE={core}', f'-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY={build / "archives"}',
                        '-DBUILD_TESTING=OFF'], check=True)
        subprocess.run(['cmake', '--build', str(build), '--parallel'], check=True)
        archive = build / 'archives/libFindTheWayCore.a'
        if not archive.is_file():
            raise RuntimeError(f'Expected Release archive not produced: {archive}')
        archives.append(archive)
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='ftw-stage-', dir=output.parent) as stage:
        staged = pathlib.Path(stage) / output.name
        command = ['xcodebuild', '-create-xcframework']
        for archive in archives:
            command += ['-library', str(archive), '-headers', str(source / 'include')]
        subprocess.run(command + ['-output', str(staged)], check=True)
        staged.rename(output)
    print(output)


if __name__ == '__main__':
    try:
        main()
    except (subprocess.CalledProcessError, RuntimeError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
