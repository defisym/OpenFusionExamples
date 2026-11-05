#!/usr/bin/env python3
"""Verify shared version selection and reject mismatched/missing Android SDK ABIs."""
import importlib.util
import json
from pathlib import Path
import subprocess
import tempfile
import argparse

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--cmake', default='cmake')
args=parser.parse_args()
repo=Path(__file__).resolve().parents[4]
port=repo/'Extensions/FFMpeg/Android'
spec=importlib.util.spec_from_file_location('sdk_builder',port/'build_sdk.py')
builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)
with tempfile.TemporaryDirectory(prefix='ffmpeg-version-') as temporary:
    root=Path(temporary)
    canonical=builder.VERSION_FILE.read_text()
    version_file=root/'FFmpegVersion.cmake'
    version_file.write_text(canonical.replace('set(OFE_FFMPEG_VERSION "'+builder.VERSION+'")','set(OFE_FFMPEG_VERSION "10.1")'))
    assert builder.version_settings(version_file)==('10.1',builder.SHA256)
    manifest={'version':builder.VERSION,'source_sha256':builder.SHA256,'abis':['arm64-v8a']}
    for component in builder.COMPONENTS:
        lib=root/'arm64-v8a/lib'/('lib'+component+'.so');lib.parent.mkdir(parents=True,exist_ok=True);lib.touch()
        header=root/'arm64-v8a/include'/('lib'+component)/'version.h';header.parent.mkdir(parents=True,exist_ok=True);header.touch()
    (root/'arm64-v8a/include/libavcodec/avcodec.h').touch()
    def write(): (root/'manifest.json').write_text(json.dumps(manifest))
    write()
    assert builder.sdk_matches(root,['arm64-v8a'])
    assert not builder.sdk_matches(root,['x86_64'])
    script=root/'check.cmake'
    script.write_text('cmake_minimum_required(VERSION 3.22)\nset(ANDROID_ABI arm64-v8a)\nset(OFE_FFMPEG_AUTO_DEPLOY OFF)\nset(OFE_FFMPEG_ROOT "'+root.as_posix()+'")\ninclude("'+(port/'cpp/EnsureSDK.cmake').as_posix()+'")\n')
    def check_cmake(success):
        result=subprocess.run([args.cmake,'-P',script],capture_output=True,text=True)
        assert (result.returncode==0)==success,result.stdout+result.stderr
        if not success: assert 'must match' in result.stderr,result.stderr
    check_cmake(True)
    manifest['version']='0.0';write()
    assert not builder.sdk_matches(root,['arm64-v8a']);check_cmake(False)
    manifest['version']=builder.VERSION;manifest['source_sha256']='0'*64;write()
    assert not builder.sdk_matches(root,['arm64-v8a']);check_cmake(False)
    manifest['source_sha256']=builder.SHA256;manifest['abis']=[];write()
    assert not builder.sdk_matches(root,['arm64-v8a']);check_cmake(False)
    manifest['abis']=['arm64-v8a'];write()
    (root/'arm64-v8a/lib/libavcodec.so').unlink()
    assert not builder.sdk_matches(root,['arm64-v8a']);check_cmake(False)
print('PASS: Windows/Android shared FFmpeg version, SDK version/source/ABI validation and missing-library rejection')
