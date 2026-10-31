#!/usr/bin/env python3
"""Build a standalone Android smoke APK against the real Java bridge and native libraries."""
import argparse
import os
from pathlib import Path
import subprocess
import zipfile
import shutil
import sys

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--android-jar',type=Path,required=True)
parser.add_argument('--build-tools',type=Path,required=True)
parser.add_argument('--sdl-source',type=Path,required=True)
parser.add_argument('--native-build',type=Path,required=True)
parser.add_argument('--ffmpeg-build',type=Path,required=True)
parser.add_argument('--ffmpeg-sdk',type=Path,required=True)
parser.add_argument('--ndk',type=Path,required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
repo=Path(__file__).resolve().parents[5]
java=repo/'Extensions/Android_Gradle/app/src/main/java'
root=args.output.resolve();root.mkdir(parents=True,exist_ok=True)
classes=root/'classes';classes.mkdir(exist_ok=True)
jdk=Path(os.environ['JAVA_HOME'])/'bin'
def run(command):
    print('+',' '.join(map(str,command)),flush=True);subprocess.run(list(map(str,command)),check=True)
manifest=root/'AndroidManifest.xml'
manifest.write_text('''<manifest xmlns:android="http://schemas.android.com/apk/res/android" package="ofe.sdlprobe">
<uses-sdk android:minSdkVersion="21" android:targetSdkVersion="23"/>
<uses-permission android:name="android.permission.INTERNET"/>
<application android:label="OFE SDL Mixer Probe" android:debuggable="true" android:extractNativeLibs="true">
<provider android:name=".ProbeProvider" android:authorities="ofe.sdlprobe.files" android:exported="false"/>
<activity android:name=".ProbeActivity" android:exported="true"><intent-filter><action android:name="android.intent.action.MAIN"/><category android:name="android.intent.category.LAUNCHER"/></intent-filter></activity>
</application></manifest>''')
owned=[java/'_DeLib'/name for name in ['SDLMixer.java','SDLMixerStorage.java','FusionStorage.java','FusionFiles.java','FFMpegStorage.java','FFMpegPlayer.java','FFMpegTexture.java','SDLAudioRuntime.java','MediaSource.java']]
vendor=list((args.sdl_source/'android-project/app/src/main/java/org/libsdl/app').glob('*.java'))
sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
from fusion_compile_fixture import FIXTURES
fusion=[]
for name in ['Application/CRunApp','RunLoop/CRun','Extensions/CExtStorage','OpenGL/ITexture','OpenGL/GLRenderer','Runtime/MMFRuntime']:
    path=root/'fusion-fixtures'/(name+'.java');path.parent.mkdir(parents=True,exist_ok=True)
    package,cls=name.split('/');path.write_text('package '+package+';\n'+FIXTURES[name]);fusion.append(path)
run([jdk/'javac','--release','8','-Xlint:-options','-cp',args.android_jar,'-d',classes,*owned,*vendor,*fusion,*[Path(__file__).with_name(name) for name in ['StorageProbe.java','FilesProbe.java','VideoProbe.java','ProbeProvider.java','ProbeActivity.java']]])
jar=root/'classes.jar'
run([jdk/'jar','--create','--file',jar,'-C',classes,'.'])
dex=root/'dex';dex.mkdir(exist_ok=True)
run([args.build_tools/'d8','--min-api','21','--lib',args.android_jar,'--output',dex,jar])
base=root/'probe-base.apk'
run([args.build_tools/'aapt','package','-f','-M',manifest,'-I',args.android_jar,'-F',base])
fixtures=root/'assets';fixtures.mkdir(exist_ok=True)
wave=fixtures/'tone.wav'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','sine=frequency=440:sample_rate=44100:duration=2','-ac','2',wave])
for extension in ['mp3','ogg','flac','opus','wv']:run(['ffmpeg','-v','error','-y','-i',wave,fixtures/('tone.'+extension)])
(fixtures/'mmf').mkdir(exist_ok=True);shutil.copy2(wave,fixtures/'mmf/extra.wav')
video=fixtures/'video.mp4'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','testsrc2=size=64x48:rate=10:duration=1.2','-f','lavfi','-i','sine=frequency=1000:sample_rate=48000:duration=1.2','-c:v','libx264','-pix_fmt','yuv420p','-profile:v','baseline','-c:a','aac','-shortest',video])
run(['ffmpeg','-v','error','-y','-i',video,'-frames:v','1','-f','rawvideo','-pix_fmt','rgba',fixtures/'video.rgba'])
material='1234567890abcdef'.encode('utf-16le')
run(['openssl','enc','-aes-128-cbc','-K',material[:16].hex(),'-iv',material[16:].hex(),'-in',wave,'-out',fixtures/'tone.enc'])
host=next((args.ndk/'toolchains/llvm/prebuilt').iterdir())
with zipfile.ZipFile(base,'a',zipfile.ZIP_DEFLATED) as apk:
    apk.write(dex/'classes.dex','classes.dex')
    for name in ['SDL2','SDL2_mixer_ext','ofe_audio','ofe_sdl_mixer']:apk.write(args.native_build/'lib'/('lib'+name+'.so'),'lib/x86_64/lib'+name+'.so')
    apk.write(host/'sysroot/usr/lib/x86_64-linux-android/libc++_shared.so','lib/x86_64/libc++_shared.so')
    apk.write(args.ffmpeg_build/'libofe_ffmpeg.so','lib/x86_64/libofe_ffmpeg.so')
    for component in ['avcodec','avformat','avutil','swscale','swresample','avfilter']:apk.write(args.ffmpeg_sdk/'x86_64/lib'/('lib'+component+'.so'),'lib/x86_64/lib'+component+'.so')
    for source in fixtures.rglob('*'):
        if source.is_file():apk.write(source,'assets/'+source.relative_to(fixtures).as_posix(),compress_type=zipfile.ZIP_STORED)
aligned=root/'probe-aligned.apk';run([args.build_tools/'zipalign','-f','4',base,aligned])
key=root/'probe.keystore'
if not key.exists():run([jdk/'keytool','-genkeypair','-keystore',key,'-storepass','android','-keypass','android','-alias','probe','-keyalg','RSA','-validity','30','-dname','CN=OFE Android Test'])
output=root/'SDL-Mixer-Probe.apk'
run([args.build_tools/'apksigner','sign','--ks',key,'--ks-pass','pass:android','--key-pass','pass:android','--out',output,aligned])
print('Probe APK:',output)
