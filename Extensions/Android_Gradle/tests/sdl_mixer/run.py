#!/usr/bin/env python3
"""Run real Mixer X playback with SDL's dummy output and exercise production JNI."""
import argparse
import os
from pathlib import Path
import subprocess

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build',required=True,type=Path)
parser.add_argument('--sdl-source',required=True,type=Path)
parser.add_argument('--android-jar',required=True,type=Path)
args=parser.parse_args()
repo=Path(__file__).resolve().parents[4]
java=repo/'Extensions/Android_Gradle/app/src/main/java'
tests=Path(__file__).resolve().parent
build=args.build.resolve();fixtures=build/'fixtures';fixtures.mkdir(exist_ok=True)
jdk=Path(os.environ['JAVA_HOME'])/'bin'
def run(command,env=None):
    print('+',' '.join(map(str,command)),flush=True)
    subprocess.run(list(map(str,command)),check=True,env=env)
wave=fixtures/'音频🎵.wav'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','sine=frequency=440:sample_rate=44100:duration=2','-ac','2',wave])
files=[wave]
for extension in ['mp3','ogg','flac','opus','wv']:
    target=fixtures/('tone.'+extension);run(['ffmpeg','-v','error','-y','-i',wave,target]);files.append(target)
env=dict(os.environ,SDL_AUDIODRIVER='dummy')
run([build/'ofe_pcm_test'])
run([build/'ofe_mixer_test',*files],env)
classes=build/'java-classes';classes.mkdir(exist_ok=True)
owned=[java/'_DeLib'/name for name in ['SDLMixer.java','SDLAudioRuntime.java','MediaSource.java']]
vendor=list((args.sdl_source/'android-project/app/src/main/java/org/libsdl/app').glob('*.java'))
run([jdk/'javac','--release','8','-Xlint:-options','-cp',args.android_jar,'-d',classes,*owned,*vendor,tests/'NativeTest.java'])
run([jdk/'java','-Xcheck:jni','-Djava.library.path='+str(build/'lib'),'-cp',str(classes)+os.pathsep+str(args.android_jar),'_DeLib.NativeTest',wave],env)
