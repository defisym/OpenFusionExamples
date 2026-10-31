#!/usr/bin/env python3
"""Exercise the actual player/JNI against FFmpeg output; Android/device tests are separate."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--ffmpeg-sdk', required=True, type=Path)
parser.add_argument('--audio-root', required=True, type=Path)
parser.add_argument('--android-jar', required=True, type=Path)
parser.add_argument('--cmake', default='cmake')
parser.add_argument('--build', type=Path, default=Path('/tmp/ofe-ffmpeg-tests'))
args = parser.parse_args()
repo = Path(__file__).resolve().parents[4]
cpp = repo/'Extensions/FFMpeg/Android/cpp'
java = repo/'Extensions/Android_Gradle/app/src/main/java'
tests = Path(__file__).resolve().parent
build = args.build.resolve(); build.mkdir(parents=True, exist_ok=True)
jdk = Path(os.environ['JAVA_HOME'])/'bin'
def run(command):
    print('+', ' '.join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), check=True)
run([args.cmake, '-S', cpp, '-B', build/'native', '-DOFE_FFMPEG_ROOT='+str(args.ffmpeg_sdk.resolve()), '-DOFE_AUDIO_ROOT='+str(args.audio_root.resolve()), '-DCMAKE_BUILD_TYPE=Debug'])
run([args.cmake, '--build', build/'native', '-j2'])
media = build/'sample-视频-🎬.mp4'; raw = build/'reference.rgba'
run(['ffmpeg', '-v', 'error', '-y', '-f', 'lavfi', '-i', 'testsrc2=size=64x48:rate=10:duration=1.2',
     '-f', 'lavfi', '-i', 'sine=frequency=1000:sample_rate=48000:duration=1.2', '-c:v', 'mpeg4', '-bf', '2', '-c:a', 'aac', '-shortest', media])
run(['ffmpeg', '-v', 'error', '-y', '-i', media, '-f', 'rawvideo', '-pix_fmt', 'rgba', raw])
image=build/'still.png'
run(['ffmpeg', '-v', 'error', '-y', '-f', 'lavfi', '-i', 'color=c=red@0.5:s=16x16,format=rgba', '-frames:v', '1', image])
classes=build/'classes'; classes.mkdir(exist_ok=True)
run([jdk/'javac', '--release', '8', '-cp', args.android_jar, '-d', classes,
     java/'_DeLib/FFMpegPlayer.java', java/'_DeLib/SDLAudioRuntime.java', java/'_DeLib/SDLMixer.java', java/'_DeLib/FFMpegSource.java', java/'_DeLib/MediaSource.java', tests/'DecodeTest.java', tests/'AudioTest.java'])
run([jdk/'java', '-Xcheck:jni', '-Djava.library.path='+str(build/'native'), '-cp',str(classes)+os.pathsep+str(args.android_jar), '_DeLib.DecodeTest', media, raw, image])
wave=build/'music.wav'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','sine=frequency=440:sample_rate=44100:duration=3','-ac','2',wave])
subprocess.run([str(jdk/'java'),'-Xcheck:jni','-Djava.library.path='+str(build/'native')+os.pathsep+str(args.audio_root/'lib'),'-cp',str(classes)+os.pathsep+str(args.android_jar),'_DeLib.AudioTest',str(media),str(wave)],check=True,env=dict(os.environ,SDL_AUDIODRIVER='dummy'))
shared=repo/'Extensions/_3rdLib'
utility=repo/'Extensions/_DeLib'
run(['c++', '-std=c++20', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
     '-I'+str(shared), '-I'+str(utility), '-I'+str(args.ffmpeg_sdk/'include'),
     '-I'+str(args.audio_root/'codecs-sdk/include/SDL2'), tests/'SharedCoreTest.cpp', shared/'FFMpegAdapter.cpp',
     '-L'+str(args.ffmpeg_sdk/'lib'), '-L'+str(args.audio_root/'lib'),
     '-lavformat','-lavcodec','-lavfilter','-lavutil','-lswscale','-lswresample','-lSDL2-2.0',
     '-Wl,-rpath,'+str(args.audio_root/'lib'), '-o',build/'shared-core-test'])
delayed=build/'delayed-audio.mp4'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','testsrc2=size=64x48:rate=10:duration=1.2','-itsoffset','0.4','-f','lavfi','-i','sine=frequency=1000:sample_rate=48000:duration=0.8','-c:v','mpeg4','-c:a','aac',delayed])
subprocess.run([str(build/'shared-core-test'), str(media), str(delayed)], check=True, env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
