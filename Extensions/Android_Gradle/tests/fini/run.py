#!/usr/bin/env python3
"""Host core/JNI/ACE checks. Requires JDK and JSON-java 20240303 source or JAR.
SDK doubles validate boundary behavior, not Android APK or device integration.
"""
import argparse,os,subprocess
from pathlib import Path
from fixture import create
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--cmake',default='cmake');p.add_argument('--json',type=Path,required=True,help='JSON-java source root or JAR (test-only)')
p.add_argument('--build',type=Path,default=Path('work/fini-host'))
a=p.parse_args();repo=Path(__file__).resolve().parents[4];tests=Path(__file__).resolve().parent;build=a.build.resolve();java=repo/'Extensions/Android_Gradle/app/src/main/java'
def run(cmd):subprocess.run(list(map(str,cmd)),cwd=repo,check=True)
run([a.cmake,'-S',repo/'Extensions/Android_Gradle/cmake/Fini','-B',build,'-DFINI_BUILD_TESTS=ON'])
run([a.cmake,'--build',build,'--parallel','2']);run([build/'core/fini_default_ini']);run([build/'core/fini_tests'])
fixture=build/'fixture';create(fixture);classes=build/'classes';classes.mkdir(exist_ok=True)
source=list(fixture.rglob('*.java'))+[java/'Expressions/CValue.java',java/'Extensions/CRunFini.java',java/'_DeLib/FiniNative.java',tests/'IntegrationTest.java']
cp=str(a.json.resolve()) if a.json.is_file() else str(classes)
if a.json.is_dir():source+=list(a.json.resolve().rglob('*.java'))
run(['javac','--release','8','-cp',cp,'-d',classes,*source])
run(['java','-Xcheck:jni','-Djava.library.path='+str(build),'-cp',str(classes)+os.pathsep+cp,'Extensions.IntegrationTest'])
