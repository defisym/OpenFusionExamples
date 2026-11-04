"""Package Fini.ext on Windows from the author's verified Mac native library.
Uses the independently verified legacy .ext codec from PR #100. No SDK source.
"""
import importlib.util
import io
from pathlib import Path
import plistlib
import zipfile

ROOT=Path(__file__).resolve().parents[1]


def codec():
    spec=importlib.util.spec_from_file_location('fusion_ext',ROOT/'iOS/Tools/make_ext.py')
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module);return module


def contents(validated):
    manifest,data=validated
    if manifest['owner']!='Fini':raise ValueError('Expected Fini native artifacts')
    payload={
        'Classes/Extensions/CRunFini.m':b'''// The implementation is precompiled in the author's native archive.
#import "CRunFini.h"
#import <TargetConditionals.h>
#if TARGET_OS_SIMULATOR
__asm__(".linker_option \\\"-lFini_iphonesimulator\\\"");
#else
__asm__(".linker_option \\\"-lFini_iphoneos\\\"");
#endif
__asm__(".linker_option \\\"-lc++\\\"");
__asm__(".linker_option \\\"-lz\\\"");
''',
        'configure_fini_export.py':(ROOT/'iOS/Fini/configure_export.py').read_bytes(),
        'xcode_project.py':(ROOT/'iOS/Tools/xcode_project.py').read_bytes(),
    }
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        info=plistlib.loads(archive.read('Fini.xcframework/Info.plist'));seen=set();header=None
        for entry in info['AvailableLibraries']:
            if entry.get('SupportedPlatform')!='ios' or 'arm64' not in entry.get('SupportedArchitectures',[]):continue
            variant=entry.get('SupportedPlatformVariant','device')
            if variant not in ('device','simulator') or variant in seen:raise ValueError('Duplicate/unexpected Fini platform slice')
            seen.add(variant);prefix='Fini.xcframework/'+entry['LibraryIdentifier']+'/'
            binary=archive.read(prefix+entry['LibraryPath'])
            if binary[:8]!=b'!<arch>\n' and binary[:4] not in (b'\xca\xfe\xba\xbe',b'\xbe\xba\xfe\xca',b'\xca\xfe\xba\xbf',b'\xbf\xba\xfe\xca'):raise ValueError('Expected a native static archive')
            sdk='iphonesimulator' if variant=='simulator' else 'iphoneos'
            payload['libFini_'+sdk+'.a']=binary
            current=archive.read(prefix+entry.get('HeadersPath','Headers')+'/CRunFini.h').replace(b'\r\n',b'\n')
            if header is not None and current!=header:raise ValueError('Device/simulator public headers differ')
            header=current
        if seen!={'device','simulator'}:raise ValueError('Both Fini platforms are required')
        payload['Classes/Extensions/CRunFini.h']=header
    return payload


def write(validated,output):
    module=codec();payload=contents(validated);module.write_extension('Fini',payload,output)
    decoded=module.read_extension(output.read_bytes())
    if any(decoded.get(name)!=data for name,data in payload.items()):raise ValueError('Fini .ext verification failed')
    return output
