#!/usr/bin/env python3
"""Compile real Android bridge APIs, then test every Fusion ACE against runtime fixtures."""
import argparse
import os
from pathlib import Path
import subprocess
import sys
import tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from fusion_compile_fixture import FIXTURES
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--android-jar',required=True,type=Path)
args=parser.parse_args()
repo=Path(__file__).resolve().parents[4];java=repo/'Extensions/Android_Gradle/app/src/main/java';tests=Path(__file__).resolve().parent
jdk=Path(os.environ['JAVA_HOME'])/'bin'
with tempfile.TemporaryDirectory(prefix='sdl-mixer-java-') as temp:
    root=Path(temp);fixtures=[]
    for name,body in FIXTURES.items():
        package,cls=name.split('/');path=root/(name+'.java');path.parent.mkdir(parents=True,exist_ok=True);path.write_text('package '+package+';\n'+body);fixtures.append(path)
    actual=[java/'Extensions/CRunSDL_Mixer.java']+[java/'_DeLib'/name for name in ['SDLMixer.java','SDLMixerStorage.java','FusionStorage.java','FusionFiles.java','SDLAudioRuntime.java','MediaSource.java']]
    subprocess.run([str(jdk/'javac'),'--release','8','-Xlint:-options','-cp',str(args.android_jar),'-d',str(root/'actual'),*map(str,fixtures+actual)],check=True)
    android_fixtures = {
        'android/content/Context': 'public class Context {public Context getApplicationContext(){return this;} public android.content.ContentResolver getContentResolver(){return null;} public android.content.res.AssetManager getAssets(){return null;} public java.io.File getCacheDir(){return new java.io.File(System.getProperty("java.io.tmpdir"));}}',
        'android/app/Activity': 'public class Activity extends android.content.Context { private final Application app=new Application(); public Application getApplication(){return app;} public boolean isFinishing(){return false;} public boolean isDestroyed(){return false;} }',
        'android/app/Application': 'public class Application {public java.util.ArrayList<ActivityLifecycleCallbacks> callbacks=new java.util.ArrayList<>(); public void registerActivityLifecycleCallbacks(ActivityLifecycleCallbacks c){callbacks.add(c);} public void unregisterActivityLifecycleCallbacks(ActivityLifecycleCallbacks c){callbacks.remove(c);} public interface ActivityLifecycleCallbacks {void onActivityCreated(Activity a,android.os.Bundle b);void onActivityStarted(Activity a);void onActivityResumed(Activity a);void onActivityPaused(Activity a);void onActivityStopped(Activity a);void onActivitySaveInstanceState(Activity a,android.os.Bundle b);void onActivityDestroyed(Activity a);}}',
        'android/os/Bundle': 'public class Bundle {}',
        'android/os/Looper': 'public class Looper {public static Looper getMainLooper(){return new Looper();}}',
        'android/os/Handler': 'public class Handler {public Handler(Looper l){} public boolean postDelayed(Runnable r,long t){return true;} public void removeCallbacks(Runnable r){}}',
        'android/util/Log': 'public class Log {public static int e(String tag,String message,Throwable error){throw new AssertionError(message,error);}}',
        '_DeLib/SessionsTestSweep': 'public class SessionsTestSweep {public static void run(Application.CRunApp app){for(Extensions.CExtStorage e:new java.util.ArrayList<>(app.extensionStorage))((SDLMixerStorage)e).maintain();}}',
    }
    platform=[]
    for name,body in android_fixtures.items():
        package,cls=name.rsplit('/',1);path=root/(name+'.java');path.parent.mkdir(parents=True,exist_ok=True);path.write_text('package '+package.replace('/','.')+';\n'+body);platform.append(path)
    subprocess.run([str(jdk/'javac'),'--release','8','-Xlint:-options','-cp',str(args.android_jar),'-d',str(root/'test'),*map(str,fixtures+platform),str(java/'Extensions/CRunSDL_Mixer.java'),str(java/'_DeLib/SDLMixerStorage.java'),str(java/'_DeLib/FusionStorage.java'),str(java/'_DeLib/FusionFiles.java'),str(java/'_DeLib/MediaSource.java'),str(tests/'fixtures/SDLMixer.java'),str(tests/'SessionsTest.java'),str(tests/'MediaSourceTest.java'),str(tests/'AdapterTest.java')],check=True)
    subprocess.run([str(jdk/'java'),'-cp',str(root/'test')+os.pathsep+str(args.android_jar),'Extensions.AdapterTest'],check=True)
    subprocess.run([str(jdk/'java'),'-cp',str(root/'test')+os.pathsep+str(args.android_jar),'_DeLib.SessionsTest'],check=True)
    subprocess.run([str(jdk/'java'),'-cp',str(root/'test')+os.pathsep+str(args.android_jar),'_DeLib.MediaSourceTest'],check=True)
print('PASS: Java 8 Android compilation; Fusion fixtures do not establish full Runtime APK behavior')
