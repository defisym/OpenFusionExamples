#!/usr/bin/env python3
"""Reject stale Android crypto SDKs using the same OpenSSL selection as Windows."""
import argparse,importlib.util,json,subprocess,tempfile
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--cmake',default='cmake');args=parser.parse_args()
repo=Path(__file__).resolve().parents[4];port=repo/'Extensions/SDL_Mixer/Android'
spec=importlib.util.spec_from_file_location('crypto_sdk',port/'build_crypto_sdk.py');sdk=importlib.util.module_from_spec(spec);spec.loader.exec_module(sdk)
with tempfile.TemporaryDirectory(prefix='crypto-sdk-test-') as temporary:
 root=Path(temporary);abi='arm64-v8a'
 for name in ['include/openssl/evp.h','lib/libcrypto.a']:
  p=root/abi/name;p.parent.mkdir(parents=True,exist_ok=True);p.touch()
 metadata=dict(version=sdk.VERSION,vcpkg_commit=sdk.COMMIT,api=21,abis=[abi])
 script=root/'check.cmake';script.write_text('cmake_minimum_required(VERSION 3.22)\nset(ofe_extensions "'+(repo/'Extensions').as_posix()+'")\nset(ANDROID_ABI arm64-v8a)\nset(ANDROID_PLATFORM android-21)\nset(OFE_ENCRYPT_AUTO_DEPLOY OFF)\nset(OFE_ENCRYPT_ROOT "'+root.as_posix()+'")\ninclude("'+(port/'cpp/crypto/EnsureSDK.cmake').as_posix()+'")\n')
 def check(ok):
  (root/'manifest.json').write_text(json.dumps(metadata));assert sdk.matches(root,[abi])==ok
  result=subprocess.run([args.cmake,'-P',script],capture_output=True,text=True)
  assert (result.returncode==0)==ok,result.stdout+result.stderr
 check(True)
 for field,value in [('version','0.0'),('vcpkg_commit','0'*40),('api',24),('abis',[])]:
  original=metadata[field];metadata[field]=value;check(False);metadata[field]=original
 (root/abi/'lib/libcrypto.a').unlink();check(False)
print('PASS: shared Windows OpenSSL version/revision, Android API/ABI and missing-library checks')
