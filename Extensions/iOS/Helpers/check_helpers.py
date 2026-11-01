#!/usr/bin/env python3
"""Syntax-check generic helpers and optional extension sources against an iOS Runtime."""
import argparse
import pathlib
import platform
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime-project', type=pathlib.Path, required=True)
    parser.add_argument('--minimum-ios', default='17.0')
    parser.add_argument('--runtime-source', type=pathlib.Path,
                        help='Optional extension Runtime source directory')
    parser.add_argument('--include', type=pathlib.Path, action='append', default=[],
                        help='Additional extension include directory; repeat as needed')
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
    sources = sorted(helpers.glob('Object*.h'))
    if args.runtime_source:
        if not args.runtime_source.is_dir():
            parser.error('runtime-source must be an existing directory')
        sources += [*sorted(args.runtime_source.glob('*.m')), *sorted(args.runtime_source.glob('*.mm'))]
    command = ['xcrun', '--sdk', 'iphonesimulator', 'clang++', '-std=c++11',
               '-target', f'arm64-apple-ios{args.minimum_ios}-simulator', '-isysroot', sdk, '-fno-objc-arc', '-fsyntax-only',
               '-x', 'objective-c++', '-F', str(project), '-I', str(classes),
               '-I', str(helpers)]
    if args.runtime_source:
        command += ['-I', str(args.runtime_source.resolve())]
    for directory in args.include:
        command += ['-I', str(directory.resolve())]
    prefixes = list(project.glob('*_Prefix.pch'))
    if len(prefixes) == 1:
        command += ['-include', str(prefixes[0])]
    for directory in includes:
        command += ['-I', str(directory)]
    subprocess.run(command + [str(source) for source in sources], check=True)


if __name__ == '__main__':
    main()
