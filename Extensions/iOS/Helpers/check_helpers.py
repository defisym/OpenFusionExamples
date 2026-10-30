#!/usr/bin/env python3
"""Syntax-check owned helpers against an exported iOS Runtime on Apple Silicon."""
import argparse
import pathlib
import platform
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime-project', type=pathlib.Path, required=True)
    args = parser.parse_args()
    if platform.system() != 'Darwin' or platform.machine() != 'arm64' or not shutil.which('xcrun'):
        parser.error('Requires Apple Silicon macOS and selected Xcode')
    project = args.runtime_project.resolve()
    classes = project / 'Classes'
    if not (classes / 'RunLoop' / 'CRun.h').is_file():
        parser.error('Expected an exported project with Classes/RunLoop/CRun.h')
    sdk = subprocess.check_output(['xcrun', '--sdk', 'iphonesimulator', '--show-sdk-path'], text=True).strip()
    includes = sorted({p.parent for p in classes.rglob('*.h')})
    source = pathlib.Path(__file__).resolve().with_name('FTWObjectHelpers.mm')
    command = ['xcrun', '--sdk', 'iphonesimulator', 'clang++', '-std=c++11',
               '-arch', 'arm64', '-isysroot', sdk, '-fno-objc-arc', '-fsyntax-only',
               '-F', str(project), '-I', str(classes)]
    prefixes = list(project.glob('*_Prefix.pch'))
    if len(prefixes) == 1:
        command += ['-include', str(prefixes[0])]
    for directory in includes:
        command += ['-I', str(directory)]
    subprocess.run(command + [str(source)], check=True)


if __name__ == '__main__':
    main()
