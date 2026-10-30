#!/usr/bin/env python3
"""Run the actual Runtime adapter against own API fixtures and Foundation.

Requires a host-built FindTheWayCore static library; this does not exercise the
private Fusion Runtime or replace Apple/exporter/device integration checks.
"""
import argparse
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--core-library', type=Path, required=True)
    parser.add_argument('--compiler', default='clang')
    args = parser.parse_args()
    if not args.core_library.is_file():
        parser.error('Build the core for this host first')
    if not shutil.which(args.compiler):
        parser.error('Objective-C compiler is missing')
    tests = Path(__file__).resolve().parent
    project = tests.parent / 'FindTheWay'
    flags = []
    if platform.system() == 'Darwin':
        flags = ['-framework', 'Foundation', '-lc++', '-liconv']
        compile_flags = ['-fno-objc-arc', '-fobjc-exceptions']
    else:
        if not shutil.which('gnustep-config'):
            parser.error('Linux checks require GNUstep Base development headers/libraries')
        compile_flags = shlex.split(subprocess.check_output(['gnustep-config', '--objc-flags'], text=True))
        objc_headers = subprocess.check_output(['gcc', '-print-file-name=include'], text=True).strip()
        compile_flags += ['-I', objc_headers]
        flags = shlex.split(subprocess.check_output(['gnustep-config', '--base-libs'], text=True)) + ['-lstdc++']
    with tempfile.TemporaryDirectory(prefix='ftw-runtime-test-') as folder:
        output = Path(folder) / 'test'
        subprocess.run([args.compiler, *compile_flags,
                        '-I', str(tests / 'runtime_fixture'), '-I', str(project / 'Runtime'),
                        '-I', str(project / 'Core/include'),
                        str(project / 'Runtime/CRunFindTheWay.m'),
                        str(tests / 'runtime_fixture/RuntimeAdapterTest.m'),
                        str(args.core_library.resolve()), *flags, '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True)


if __name__ == '__main__':
    main()
