#!/usr/bin/env python3
"""Linux/GCC verification for the header-only helpers and build-mode compatibility."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-dir', type=Path, default=Path('work/delib-helper-tests'))
parser.add_argument('--cmake', default='cmake')
parser.add_argument('--cxx', default='g++')
parser.add_argument('--sanitize', action='store_true')
parser.add_argument('--iconv-library', action='append', default=[])
args = parser.parse_args()
tests = Path(__file__).resolve().parent
core = tests.parents[2] / '_DeLib'
build = args.build_dir.resolve()
build.mkdir(parents=True, exist_ok=True)

def run(command, env=None):
    print('+ ' + ' '.join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), check=True, env=env)

run([args.cmake, '-S', tests, '-B', build / 'cmake', '-DCMAKE_BUILD_TYPE=Release', '-DDELIB_ACP_ENCODING=UTF-8'])
run([args.cmake, '--build', build / 'cmake', '--parallel', '2'])
run([build / 'cmake/general_definition_test'])
run([build / 'cmake/acp_override_test'])
run([build / 'cmake/path_owner_thread_test'])
run([build / 'cmake/map_data_test'])
run([build / 'cmake/event_adapter_test'])
include = ['-I', core, '-I', core.parent / '_3rdLib']
for name, flags, expected in [('default', [], 0), ('debug', ['-D_DEBUG'], 0),
                               ('release', ['-DNDEBUG'], 1), ('both', ['-D_DEBUG', '-DNDEBUG'], 0)]:
    binary = build / ('mode-' + name)
    run([args.cxx, '-std=c++20', *include, *flags, '-DEXPECT_STASH=' + str(expected),
         tests / 'BuildModeTest.cpp', '-o', binary, *args.iconv_library])
    run([binary])
print('PASS: all four _DEBUG/NDEBUG combinations (_DEBUG takes precedence)', flush=True)
if args.sanitize:
    binary = build / 'helpers-sanitized'
    run([args.cxx, '-std=c++20', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
         '-funsigned-char', *include, tests / 'GeneralDefinitionTest.cpp', tests / 'HelperOtherTU.cpp',
         '-o', binary, *args.iconv_library])
    run([binary], env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
    map_data = build / 'map-data-sanitized'
    run([args.cxx, '-std=c++20', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
         '-funsigned-char', *include, tests / 'MapDataTest.cpp', '-o', map_data, *args.iconv_library])
    run([map_data], env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
    threaded = build / 'path-owner-thread-sanitized'
    run([args.cxx, '-std=c++20', '-g', '-pthread', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
         *include, tests / 'PathOwnerThreadTest.cpp', '-o', threaded, *args.iconv_library])
    run([threaded], env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
run(['python3', tests / 'audit_includes.py', '--cxx', args.cxx])
print('PASS: helper verification complete; Windows/Android/iOS toolchains require separate validation.')
