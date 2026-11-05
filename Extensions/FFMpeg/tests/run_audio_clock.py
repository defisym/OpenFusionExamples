#!/usr/bin/env python3
"""Test the clocks commit's estimator without a physical audio device."""
import argparse
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser(description=__doc__)
for name in ['ffmpeg-include', 'ffmpeg-lib', 'sdl-include', 'sdl-lib']:
    p.add_argument('--' + name, required=True, type=Path)
p.add_argument('--build', type=Path, default=Path('/tmp/ffmpeg-audio-clock-tests'))
p.add_argument('--baseline-include', type=Path,
               help='Optional directory containing the previous clocks-commit FFMpeg.h')
a = p.parse_args()
root = Path(__file__).resolve().parents[3]
build = a.build.resolve()
build.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')


def run(command, **kwargs):
    return subprocess.run(list(map(str, command)), check=True, **kwargs)


media = build / 'clock.mp4'
run(['ffmpeg', '-v', 'error', '-y', '-f', 'lavfi', '-i',
     'testsrc2=size=64x48:rate=10:duration=4', '-f', 'lavfi', '-i',
     'sine=frequency=1000:sample_rate=48000:duration=4', '-c:v', 'mpeg4',
     '-bf', '2', '-c:a', 'aac', '-shortest', media])


def compile_test(name, baseline=None):
    binary = build / name
    includes = ([f'-I{baseline}'] if baseline else []) + [
        f'-I{root / "Extensions/_3rdLib"}', f'-I{root / "Extensions/_DeLib"}',
        f'-I{a.ffmpeg_include}', f'-I{a.sdl_include}']
    # Expose the internal estimator to tests without adding a production API.
    run(['c++', '-std=c++20', '-fno-access-control', '-g', '-fsanitize=address,undefined',
         '-fno-omit-frame-pointer', *includes, Path(__file__).with_name('AudioClockTest.cpp'),
         root / 'Extensions/_3rdLib/FFMpegAdapter.cpp', f'-L{a.ffmpeg_lib}', f'-L{a.sdl_lib}',
         '-lavformat', '-lavcodec', '-lavfilter', '-lavutil', '-lswscale', '-lswresample',
         '-lSDL2', f'-Wl,-rpath,{a.ffmpeg_lib}', f'-Wl,-rpath,{a.sdl_lib}',
         '-Wl,--wrap=av_gettime_relative', '-o', binary])
    return binary


binary = compile_test('AudioClockTest')
run([binary, media], timeout=30, env=env)
if a.baseline_include:
    baseline = compile_test('AudioClockBefore', a.baseline_include)
    for case, message in {
        'partial': 'unfilled EOF tail must not advance media clock',
        'tempo-pause': 'tempo must not retime already submitted PCM',
    }.items():
        result = subprocess.run(list(map(str, [baseline, media, case])), timeout=30,
                                env=env, capture_output=True, text=True)
        if result.returncode == 0 or message not in result.stderr:
            raise RuntimeError(f'Unexpected baseline result for {case}: {result.stderr}')
        print(f'CONFIRMED: previous clocks commit fails {case}: {message}', flush=True)
