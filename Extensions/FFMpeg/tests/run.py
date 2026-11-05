#!/usr/bin/env python3
"""Run shared C++ FFmpeg regressions without JNI, Android or a Mixer X device."""
import argparse,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--ffmpeg-include',required=True,type=Path);p.add_argument('--ffmpeg-lib',required=True,type=Path)
p.add_argument('--sdl-include',required=True,type=Path);p.add_argument('--sdl-lib',required=True,type=Path)
p.add_argument('--build',type=Path,default=Path('/tmp/ffmpeg-cpp-tests'));a=p.parse_args()
r=Path(__file__).resolve().parents[3];b=a.build.resolve();b.mkdir(parents=True,exist_ok=True)
def run(command,**kw):subprocess.run(list(map(str,command)),check=True,**kw)
media=b/'视频-🎬.mp4';delayed=b/'延迟-音频.mp4'
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','testsrc2=size=64x48:rate=10:duration=1.2','-f','lavfi','-i','sine=frequency=1000:sample_rate=48000:duration=1.2','-c:v','mpeg4','-bf','2','-c:a','aac','-shortest',media])
run(['ffmpeg','-v','error','-y','-f','lavfi','-i','testsrc2=size=64x48:rate=10:duration=1.2','-itsoffset','0.4','-f','lavfi','-i','sine=frequency=1000:sample_rate=48000:duration=0.8','-c:v','mpeg4','-c:a','aac',delayed])
for name in ['PCMFragmentTest','SharedCoreTest','DecoderProtocolTest']:
    binary=b/name
    wrappers=['-Wl,--wrap='+symbol for symbol in ['av_read_frame','avcodec_send_packet','avcodec_receive_frame']] if name=='DecoderProtocolTest' else []
    run(['c++','-std=c++20','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(r/'Extensions/_3rdLib'),'-I'+str(r/'Extensions/_DeLib'),'-I'+str(a.ffmpeg_include),'-I'+str(a.sdl_include),Path(__file__).with_name(name+'.cpp'),r/'Extensions/_3rdLib/FFMpegAdapter.cpp','-L'+str(a.ffmpeg_lib),'-L'+str(a.sdl_lib),'-lavformat','-lavcodec','-lavfilter','-lavutil','-lswscale','-lswresample','-lSDL2','-Wl,-rpath,'+str(a.sdl_lib),*wrappers,'-o',binary])
    run([binary,media,*([delayed] if name=='SharedCoreTest' else [])],timeout=20,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0'))
