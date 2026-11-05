#!/usr/bin/env python3
"""Test the actual JNI wrapper around the shared encryption core without Android/SDL fixtures."""
import argparse,os,subprocess
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--cmake',default='cmake');parser.add_argument('--build',type=Path,default=Path('/tmp/ofe-media-crypto-tests'))
args=parser.parse_args();repo=Path(__file__).resolve().parents[4];build=args.build.resolve();build.mkdir(parents=True,exist_ok=True)
def run(command):subprocess.run(list(map(str,command)),check=True)
run([args.cmake,'-S',repo/'Extensions/SDL_Mixer/Android/cpp/crypto','-B',build,'-DCMAKE_BUILD_TYPE=Debug'])
run([args.cmake,'--build',build,'--parallel','2'])
classes=build/'classes';classes.mkdir(exist_ok=True);jdk=Path(os.environ['JAVA_HOME'])/'bin'
run([jdk/'javac','--release','8','-Xlint:-options','-d',classes,repo/'Extensions/Android_Gradle/app/src/main/java/_DeLib/MediaCrypto.java',Path(__file__).with_name('NativeTest.java')])
fixtures=repo/'Extensions/NeoPic/ToInstall/Files/Examples/NeoPic'
run([jdk/'java','-Xcheck:jni','-Djava.library.path='+str(build/'lib'),'-cp',classes,'_DeLib.NativeTest',fixtures/'Test_Encrypt.dat',fixtures/'Test_Origin.jpg'])
