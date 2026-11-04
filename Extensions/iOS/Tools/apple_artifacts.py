"""Owned Mac artifacts -> Git push/pull -> Windows packaging; no Apple tools on Windows."""
import hashlib
import io
import json
import os
from pathlib import Path
import plistlib
import tempfile
import zipfile

REPO=Path(__file__).resolve().parents[3]

def source_bytes(path):
    data=path.read_bytes()
    if path.suffix in ('.h','.hpp','.c','.cpp','.m','.mm','.py','.txt','.json','.xcconfig'):
        data=data.replace(b'\r\n',b'\n')
    elif path.name=='Info.plist' and data.startswith(b'<?xml'):
        data=data.replace(b'\r\n',b'\n')
    return data


def source_digest(repo,files):
    digest=hashlib.sha256()
    for name in sorted(files):
        path=Path(name)
        if path.is_absolute() or '..' in path.parts:raise ValueError('Unsafe source manifest path')
        data=source_bytes(repo/path)
        digest.update(name.encode()+b'\0'+data+b'\0')
    return digest.hexdigest()


def source_hashes(repo,files):
    result={}
    for name in files:
        relative=Path(name)
        if relative.is_absolute() or '..' in relative.parts:raise ValueError('Unsafe source manifest path')
        path=repo/relative
        if not path.is_file():raise ValueError('Missing source dependency: '+name)
        result[name]=hashlib.sha256(source_bytes(path)).hexdigest()
    return result


def atomic_write(path,data):
    path=Path(path);path.parent.mkdir(parents=True,exist_ok=True)
    fd,temp=tempfile.mkstemp(dir=path.parent,prefix=path.name+'.')
    try:
        with os.fdopen(fd,'wb') as output:output.write(data)
        os.replace(temp,path)
    finally:
        if Path(temp).exists():Path(temp).unlink()


def check_framework(archive,owner):
    with zipfile.ZipFile(io.BytesIO(archive)) as zipped:
        root=owner+'.xcframework/'
        for name in zipped.namelist():
            if Path(name).is_absolute() or '..' in Path(name).parts or '\\' in name:raise ValueError('Unsafe Apple artifact path')
            if not name.startswith((root,'Licenses/')):raise ValueError('Unexpected Apple artifact path: '+name)
        info=plistlib.loads(zipped.read(root+'Info.plist'));slices=set()
        for library in info.get('AvailableLibraries',[]):
            platform=library.get('SupportedPlatform');variant=library.get('SupportedPlatformVariant','device')
            for arch in library.get('SupportedArchitectures',[]):slices.add((platform,variant,arch))
            path=root+library['LibraryIdentifier']+'/'+library['LibraryPath']
            if path.endswith('.framework'):path+='/'+owner
            if not zipped.read(path):raise ValueError('Empty Apple library slice')
        expected={('ios','device','arm64'),('ios','simulator','arm64'),('ios','simulator','x86_64')}
        if not expected.issubset(slices):raise ValueError('Missing Apple device/simulator slices')


def stage(framework,owner,sources,destination,requires=(),extra_files=None,repo=REPO):
    framework=Path(framework);payload=io.BytesIO()
    with zipfile.ZipFile(payload,'w',zipfile.ZIP_DEFLATED) as zipped:
        for path in sorted(framework.rglob('*')):
            if path.is_symlink():raise ValueError('Apple handoff must not contain escaping symlinks')
            if path.is_file():zipped.write(path,framework.name+'/'+path.relative_to(framework).as_posix())
        for name,path in (extra_files or {}).items():zipped.write(path,'Licenses/'+name)
    data=payload.getvalue();check_framework(data,owner)
    filename=owner+'.xcframework.zip'
    manifest={'format':1,'owner':owner,'file':filename,'sha256':hashlib.sha256(data).hexdigest(),'source_files':sorted(sources),'source_sha256':source_digest(repo,sources),'source_hashes':source_hashes(repo,sources),'requires':list(requires)}
    # Commit both files together; Windows rejects an interrupted or stale pair.
    atomic_write(Path(destination)/filename,data)
    atomic_write(Path(destination)/'manifest.json',(json.dumps(manifest,indent=2)+'\n').encode())
    print('Mac handoff staged; author may commit/push:',destination)
    return manifest


def validate(destination,owner,repo=REPO):
    destination=Path(destination);manifest_path=destination/'manifest.json';binary=destination/(owner+'.xcframework.zip')
    if not manifest_path.exists() and not binary.exists():return None
    if not manifest_path.is_file() or not binary.is_file():raise ValueError('Incomplete Mac handoff: '+str(destination))
    manifest=json.loads(manifest_path.read_text())
    if manifest.get('format')!=1 or manifest.get('owner')!=owner or manifest.get('file')!=binary.name:raise ValueError('Invalid Mac handoff manifest')
    data=binary.read_bytes()
    if hashlib.sha256(data).hexdigest()!=manifest['sha256']:raise ValueError('Corrupt Mac handoff: '+str(binary))
    hashes=source_hashes(repo,manifest['source_files'])
    if 'source_hashes' in manifest and hashes!=manifest['source_hashes']:
        changed=sorted(name for name in set(hashes)|set(manifest['source_hashes']) if hashes.get(name)!=manifest['source_hashes'].get(name))
        raise ValueError('Mac library is stale; changed source dependencies: '+', '.join(changed))
    if source_digest(repo,manifest['source_files'])!=manifest['source_sha256']:raise ValueError('Mac library is stale; rebuild after the source change: '+str(binary))
    check_framework(data,owner)
    return manifest,data


def package(validated,destination,extra_files=None):
    manifest,data=validated;payload=io.BytesIO()
    with zipfile.ZipFile(payload,'w',zipfile.ZIP_DEFLATED) as zipped:
        zipped.writestr(manifest['file'],data)
        zipped.writestr('manifest.json',json.dumps(manifest,indent=2)+'\n')
        for name,path in (extra_files or {}).items():zipped.write(path,name)
    atomic_write(destination,payload.getvalue())
