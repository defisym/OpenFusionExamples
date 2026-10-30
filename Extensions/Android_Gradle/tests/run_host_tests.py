#!/usr/bin/env python3
"""Build and exercise the real shared C++ core/JNI with a desktop JDK.
The Fusion adapter uses explicit runtime API test doubles; this does not build an APK.
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--core',type=Path,help='Shared header directory (can be an isolated review proposal)')
parser.add_argument('--build-dir',type=Path,default=Path('work/findtheway-host'))
parser.add_argument('--cmake',default='cmake')
parser.add_argument('--cxx',default='g++')
parser.add_argument('--iconv-library', action='append', default=[], help='Extra linker input for a non-libc iconv, e.g. -liconv')
parser.add_argument('--sanitize',action='store_true',help='Also run direct C++ checks under ASan/UBSan; leak detection disabled for ptrace hosts')
args=parser.parse_args()
repo=Path(__file__).resolve().parents[3]
tests=Path(__file__).resolve().parent
java=repo/'Extensions/Android_Gradle/app/src/main/java'
build=args.build_dir.resolve();build.mkdir(parents=True,exist_ok=True)
core=(args.core or repo/'Extensions/_DeLib').resolve()
classes=build/'classes';classes.mkdir(exist_ok=True)
native=build/'native'

def run(command,env=None):
    print('+ '+' '.join(map(str,command)),flush=True)
    subprocess.run(list(map(str,command)),check=True,cwd=repo,env=env)

run([args.cmake,'-S',repo/'Extensions/Android_Gradle/app/src/main/cpp','-B',native,
     '-DCMAKE_BUILD_TYPE=Release','-DFTW_CORE_INCLUDE='+str(core)])
run([args.cmake,'--build',native,'--parallel','2'])
reference=build/'core-reference'
compile_core=[args.cxx,'-std=c++20','-DNDEBUG','-funsigned-char',
              '-I',core,'-I',core.parent/'_3rdLib',tests/'CoreReference.cpp'] + args.iconv_library
run(compile_core+['-o',reference])
run([reference,build/'reference.txt'])
run(['javac','--release','8','-Xlint:-options','-h',build,'-d',classes,
     java/'_DeLib/Coord.java',java/'_DeLib/FindTheWayClass.java',java/'_DeLib/AOEClass.java',tests/'NativeIntegrationTest.java'])
run(['java','-Xcheck:jni','-Djava.library.path='+str(native),'-cp',classes,
     'NativeIntegrationTest',build/'reference.txt'])
fixture=build/'runtime-fixture'
if fixture.exists(): shutil.rmtree(fixture)
run(['python3',tests/'make_runtime_fixture.py',fixture])
sources=build/'runtime-sources.txt'
sources.write_text('\n'.join('"'+str(path)+'"' for path in sorted(fixture.rglob('*.java')))+'\n')
run(['javac','--release','8','-Xlint:-options','-cp',classes,'-d',classes,'@'+str(sources),
     java/'Extensions/CRunFindTheWay.java',tests/'AdapterDispatchTest.java'])
run(['java','-Xcheck:jni','-Djava.library.path='+str(native),'-cp',classes,'Extensions.AdapterDispatchTest'])
run(['python3',tests/'audit_ace.py'])
if args.sanitize:
    sanitized=build/'core-sanitized'
    run(compile_core+['-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-o',sanitized])
    env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0')
    run([sanitized,build/'reference-sanitized.txt'],env=env)
print('PASS: host verification complete. Android SDK/NDK, APK packaging and device behavior require separate validation.')
