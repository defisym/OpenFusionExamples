#!/usr/bin/env python3
"""Syntax-check owned helpers and the complete adapter against an exported iOS Runtime."""
import argparse
import pathlib
import platform
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime-project', type=pathlib.Path, required=True)
    parser.add_argument('--minimum-ios', default='17.0')
    args = parser.parse_args()
    if platform.system() != 'Darwin' or platform.machine() != 'arm64' or not shutil.which('xcrun'):
        parser.error('Requires Apple Silicon macOS and selected Xcode')
    if not args.minimum_ios or any(c not in '0123456789.' for c in args.minimum_ios):
        parser.error('minimum-ios must be a numeric version')
    project = args.runtime_project.resolve()
    classes = project / 'Classes'
    if not (classes / 'RunLoop' / 'CRun.h').is_file():
        parser.error('Expected an exported project with Classes/RunLoop/CRun.h')
    sdk = subprocess.check_output(['xcrun', '--sdk', 'iphonesimulator', '--show-sdk-path'], text=True).strip()
    includes = sorted({p.parent for p in classes.rglob('*.h')})
    helpers = pathlib.Path(__file__).resolve().parent
    plugin = helpers.parent / 'FindTheWay'
    sources = [helpers / 'FTWObjectHelpers.mm', *sorted((plugin / 'Runtime').glob('*.m')),
               *sorted((plugin / 'Runtime').glob('*.mm'))]
    command = ['xcrun', '--sdk', 'iphonesimulator', 'clang++', '-std=c++11',
               '-target', f'arm64-apple-ios{args.minimum_ios}-simulator', '-isysroot', sdk, '-fno-objc-arc', '-fsyntax-only',
               '-x', 'objective-c++', '-F', str(project), '-I', str(classes),
               '-I', str(helpers), '-I', str(plugin / 'Runtime'), '-I', str(plugin / 'Core/include')]
    prefixes = list(project.glob('*_Prefix.pch'))
    if len(prefixes) == 1:
        command += ['-include', str(prefixes[0])]
    for directory in includes:
        command += ['-I', str(directory)]
    subprocess.run(command + [str(source) for source in sources], check=True)


if __name__ == '__main__':
    main()
