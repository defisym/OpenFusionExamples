#!/usr/bin/env python3
"""Compile owned Java against Android APIs and narrow Fusion API fixtures.
The fixtures match the supplied RuntimeAndroid.zip API; this is not an APK test.
"""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--android-jar', required=True, type=Path)
args=parser.parse_args()
repo=Path(__file__).resolve().parents[4]
java=repo/'Extensions/Android_Gradle/app/src/main/java'
# Only Fusion is substituted. SurfaceTexture, EGL, GLES and AudioTrack compile against real Android APIs.
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from fusion_compile_fixture import FIXTURES as fixtures
with tempfile.TemporaryDirectory(prefix='ffmpeg-java-') as temp:
    root=Path(temp); sources=[]
    for name,body in fixtures.items():
        package,cls=name.split('/'); path=root/(name+'.java'); path.parent.mkdir(parents=True,exist_ok=True)
        path.write_text('package '+package+';\n'+body); sources.append(path)
    sources += [java/'Extensions/CRunFFMpeg.java']+list((java/'_DeLib').glob('FFMpeg*.java'))
    sources.append(java/'_DeLib/MediaSource.java')
    sources += [java/'_DeLib'/name for name in ['SDLAudioRuntime.java','FusionStorage.java','FusionFiles.java','FFMpegStorage.java']]
    sources.append(Path(__file__).with_name('AdapterTest.java'))
    subprocess.run([str(Path(os.environ['JAVA_HOME'])/'bin/javac'),'--release','8','-Xlint:-options',
        '-cp',str(args.android_jar),'-d',str(root/'classes'),*map(str,sources)],check=True)
    subprocess.run([str(Path(os.environ['JAVA_HOME'])/'bin/java'), '-cp', str(root/'classes')+os.pathsep+str(args.android_jar), 'Extensions.AdapterTest'],check=True)
print('PASS: Android API Java compilation (Fusion boundary fixtures; no APK/device coverage)')
