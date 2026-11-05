#!/usr/bin/env python3
"""Exercise the audio-pipeline commit independently of later playback-control APIs."""
import argparse
import json
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--ffmpeg-include', required=True, type=Path)
p.add_argument('--ffmpeg-lib', required=True, type=Path)
p.add_argument('--sdl-include', required=True, type=Path)
p.add_argument('--sdl-lib', required=True, type=Path)
p.add_argument('--build', type=Path, default=Path('/tmp/ffmpeg-audio-seek-tests'))
p.add_argument('--baseline-include', type=Path,
               help='Optional directory containing the original c63af8fd FFMpeg.h for negative checks')
a = p.parse_args()
root = Path(__file__).resolve().parents[3]
build = a.build.resolve()
build.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')


def run(command, **kwargs):
    return subprocess.run(list(map(str, command)), check=True, **kwargs)


media, transport = build / 'av.mp4', build / 'av.ts'
for output, video, audio in [(media, 'mpeg4', 'aac'), (transport, 'mpeg2video', 'mp2')]:
    run(['ffmpeg', '-v', 'error', '-y', '-f', 'lavfi', '-i',
         'testsrc2=size=64x48:rate=10:duration=4', '-f', 'lavfi', '-i',
         'sine=frequency=1000:sample_rate=48000:duration=4', '-c:v', video,
         '-g', '10', '-bf', '2', '-c:a', audio, '-shortest', output])
packets = json.loads(run(['ffprobe', '-v', 'error', '-select_streams', 'v:0',
                         '-show_packets', '-show_entries', 'packet=pts_time,pos,flags',
                         '-of', 'json', transport], capture_output=True, text=True).stdout)['packets']
offset = next(int(packet['pos']) for packet in packets
              if 'K' in packet['flags'] and float(packet['pts_time']) >= 2.5)


def compile_test(name, baseline=None):
    binary = build / name
    includes = ([f'-I{baseline}'] if baseline else []) + [
        f'-I{root / "Extensions/_3rdLib"}', f'-I{root / "Extensions/_DeLib"}',
        f'-I{a.ffmpeg_include}', f'-I{a.sdl_include}']
    wrappers = ['-Wl,--wrap=' + symbol for symbol in
                ['av_read_frame', 'avcodec_send_packet', 'avcodec_receive_frame', 'avformat_seek_file']]
    run(['c++', '-std=c++20', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
         *includes, Path(__file__).with_name('AudioSeekTest.cpp'),
         root / 'Extensions/_3rdLib/FFMpegAdapter.cpp', f'-L{a.ffmpeg_lib}', f'-L{a.sdl_lib}',
         '-lavformat', '-lavcodec', '-lavfilter', '-lavutil', '-lswscale', '-lswresample',
         '-lSDL2', f'-Wl,-rpath,{a.ffmpeg_lib}', f'-Wl,-rpath,{a.sdl_lib}',
         *wrappers, '-o', binary])
    return binary


binary = compile_test('AudioSeekTest')
run([binary, media, transport, offset], timeout=30, env=env)
if a.baseline_include:
    baseline = compile_test('AudioSeekBefore', a.baseline_include)
    expected = {
        'units': 'time seek must convert milliseconds to stream ticks',
        'bytes': 'byte seek retains audible PCM',
        'grab': 'grabbed audio must not enter the playback queue',
        'tempo': 'filter must use the same fractional tempo as the audio clock',
    }
    for case, message in expected.items():
        result = subprocess.run(list(map(str, [baseline, media, transport, offset, case])),
                                timeout=30, env=env, capture_output=True, text=True)
        if result.returncode == 0 or message not in result.stderr:
            raise RuntimeError(f'Unexpected baseline result for {case}: {result.stderr}')
        print(f'CONFIRMED: original commit fails {case}: {message}', flush=True)
