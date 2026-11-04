#!/usr/bin/env python3
"""Run generic helpers or the Runtime adapter against owned fixtures and Foundation.

The extension mode requires a host-built FindTheWayCore static library; the
helpers-only mode has no plugin dependency. These owned models do not exercise
the private Fusion Runtime or replace Apple/exporter/device integration checks.
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
    parser.add_argument('--helpers-only', action='store_true',
                        help='Test generic helpers across two translation units, without any plugin/core')
    parser.add_argument('--core-library', type=Path)
    parser.add_argument('--compiler', default='clang')
    args = parser.parse_args()
    if not args.helpers_only and (not args.core_library or not args.core_library.is_file()):
        parser.error('Build the core for this host first')
    if not shutil.which(args.compiler):
        parser.error('Objective-C compiler is missing')
    tests = Path(__file__).resolve().parent
    project = tests.parent / 'FindTheWay'
    sources = [tests / 'runtime_fixture/RuntimeModel.mm']
    includes = [tests / 'runtime_fixture', tests.parent / 'Helpers']
    libraries = []
    if args.helpers_only:
        sources += [tests / 'object_helpers/ObjectHelpersTest.mm', tests / 'object_helpers/ObjectHelpersPeer.mm']
    else:
        sources += [project / 'Runtime/CRunFindTheWay.m', tests / 'runtime_fixture/RuntimeAdapterTest.m',
                    project / 'Runtime/FTWObjectAdapter.mm', project / 'Runtime/FTWPlatformAdapter.mm']
        includes += [project / 'Runtime', project / 'Core/include']
        libraries = [str(args.core_library.resolve())]
    flags = []
    if platform.system() == 'Darwin':
        flags = ['-framework', 'Foundation', '-lc++'] + ([] if args.helpers_only else ['-liconv'])
        compile_flags = ['-fno-objc-arc', '-fobjc-exceptions', '-fexceptions']
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
                        '-x', 'objective-c++', '-std=c++11',
                        *[arg for directory in includes for arg in ('-I', str(directory))],
                        *map(str, sources), *(['-x', 'none', *libraries] if libraries else []),
                        *flags, '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True)


if __name__ == '__main__':
    main()
