"""Verify and stage a Mac-prepared iOS extension for the RunOnly publisher.

This consumes release artifacts; it does not build Apple binaries or invent .ext files.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import plistlib
import shutil
import tempfile
import zipfile

EXTENSIONS = Path(__file__).resolve().parent.parent


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source_hashes(root: Path) -> dict[str, str]:
    import re
    owned = [root / 'iOS/FindTheWayCore', root / 'iOS/Helpers']
    pending = [root / 'iOS/Classes/Extensions/CRunFindTheWay.h',
               root / 'iOS/Classes/Extensions/CRunFindTheWay.m']
    for folder in owned:
        pending.extend(p for p in folder.rglob('*') if p.suffix in {'.h', '.mm', '.cpp'}
                       and 'tests' not in p.parts)
    pending.append(root / 'iOS/FindTheWayCore/CMakeLists.txt')
    # Producer configuration changes must also invalidate transferred binaries.
    project = root.parent / 'iOS/FindTheWay'
    pending.extend(project / name for name in ('CMakeLists.txt', 'build.py')
                   if (project / name).is_file())
    pending.append(root / 'iOS/FindTheWayCore/build_xcframework.py')
    hashes = {}
    while pending:
        source = pending.pop().resolve()
        name = (source.relative_to(root.resolve()).as_posix()
                if source.is_relative_to(root.resolve())
                else '../' + source.relative_to(root.resolve().parent).as_posix())
        if name in hashes:
            continue
        # Git may check text out as CRLF on Windows and LF on Mac.
        hashes[name] = hashlib.sha256(source.read_bytes().replace(b'\r\n', b'\n')).hexdigest()
        for include in re.findall(r'^\s*#\s*include\s*"([^"]+)"',
                                  source.read_text(encoding='utf-8-sig'), re.M):
            candidates = [source.parent / include, root / 'iOS/FindTheWayCore/include' / include,
                          root / '_DeLib' / include, root / '_3rdLib' / include]
            match = next((p for p in candidates if p.is_file()), None)
            if match:
                pending.append(match)
            elif source.is_relative_to((root / 'iOS/FindTheWayCore').resolve()):
                raise ValueError(f'Unresolved shared dependency: {include}; update the shared-core checkout')
    return hashes


def payload_files(bundle: Path, require_extension: bool = True) -> dict[str, Path]:
    files = {}
    for path in bundle.rglob('*'):
        if path.is_symlink():
            raise ValueError(f'Symlinks are not allowed: {path}')
        if path.is_file() and path != bundle / 'manifest.json':
            name = path.relative_to(bundle).as_posix()
            if name not in {'FindTheWay.ext', 'Integration.md'} and not name.startswith('FindTheWayCore.xcframework/'):
                raise ValueError(f'Unexpected artifact: {name}')
            files[name] = path
    required = ['Integration.md', 'FindTheWayCore.xcframework/Info.plist']
    if require_extension:
        required.append('FindTheWay.ext')
    if not all(name in files for name in required):
        raise ValueError('Bundle requires FindTheWay.ext, Integration.md and FindTheWayCore.xcframework')
    if (require_extension and not files['FindTheWay.ext'].stat().st_size) or not files['Integration.md'].stat().st_size:
        raise ValueError('Empty extension or integration instructions')
    info = plistlib.loads(files['FindTheWayCore.xcframework/Info.plist'].read_bytes())
    variants = set()
    for entry in info['AvailableLibraries']:
        if entry['SupportedPlatform'] != 'ios' or entry['SupportedArchitectures'] != ['arm64']:
            raise ValueError('Only iOS device/simulator arm64 artifacts are supported')
        variant = entry.get('SupportedPlatformVariant', 'device')
        if variant not in {'device', 'simulator'} or variant in variants:
            raise ValueError('Unexpected or duplicate XCFramework platform slice')
        variants.add(variant)
        relative = PurePosixPath(entry['LibraryIdentifier']) / entry['LibraryPath']
        if relative.is_absolute() or '..' in relative.parts:
            raise ValueError('Invalid XCFramework library path')
        library = files.get('FindTheWayCore.xcframework/' + str(relative))
        if not library:
            raise ValueError('Missing static archive in XCFramework slice')
        with library.open('rb') as stream:
            if stream.read(8) != b'!<arch>\n':
                raise ValueError('Expected a static archive in each XCFramework slice')
    if variants != {'device', 'simulator'}:
        raise ValueError('Both device and simulator slices are required')
    return files


def verify(bundle: Path, root: Path) -> dict[str, Path]:
    manifest = json.loads((bundle / 'manifest.json').read_text(encoding='utf-8'))
    kind = manifest.get('kind', 'release')
    if kind not in {'native', 'release'}:
        raise ValueError('Unknown iOS artifact kind')
    files = payload_files(bundle, require_extension=kind == 'release')
    if manifest.get('schema') != 1:
        raise ValueError('Unsupported iOS artifact manifest')
    if manifest.get('sources') != source_hashes(root):
        raise ValueError('iOS artifacts are stale or from a different source checkout; rebuild on Mac')
    if manifest.get('files') != {name: sha(path) for name, path in files.items()}:
        raise ValueError('iOS artifact file set or SHA-256 mismatch')
    return files


def stage(bundle: Path, root: Path) -> None:
    files = verify(bundle, root)  # Complete validation before replacing existing outputs.
    if 'FindTheWay.ext' not in files:
        raise ValueError('Native core verified, but the full .ext package is not ready; complete iOS packaging first')
    install = root / 'FindTheWay/ToInstall/Files'
    extension = install / 'Data/Runtime/iPhone/FindTheWay.ext'
    support = install / 'Tools/FindTheWay-iOS.zip'
    support.parent.mkdir(parents=True, exist_ok=True)
    extension.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='ftw-ios-', dir=install) as temporary:
        temporary = Path(temporary)
        archive = temporary / support.name
        with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as output:
            for name, path in sorted(files.items()):
                if name != 'FindTheWay.ext':
                    output.write(path, name)
            output.write(bundle / 'manifest.json', 'manifest.json')
        copied = temporary / extension.name
        shutil.copyfile(files['FindTheWay.ext'], copied)
        os.replace(archive, support)
        os.replace(copied, extension)
    # The existing Android publisher archives all of ToInstall after this target.
    print(f'Staged iOS extension: {extension}')
    print(f'Staged Xcode integration bundle: {support}')


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--artifacts', type=Path, required=True)
    parser.add_argument('--record-manifest', action='store_true',
                        help='Record hashes after completing Mac build/export/Runtime validation')
    args = parser.parse_args()
    bundle = args.artifacts.resolve()
    if not bundle.is_dir():
        parser.error('iOS artifact directory is missing; prepare it on Mac or explicitly disable iOS packaging')
    if args.record_manifest:
        files = payload_files(bundle)
        manifest = {'schema': 1, 'sources': source_hashes(EXTENSIONS),
                    'files': {name: sha(path) for name, path in files.items()}}
        (bundle / 'manifest.json').write_text(json.dumps(manifest, indent=2, sort_keys=True), encoding='utf-8')
    else:
        stage(bundle, EXTENSIONS)


if __name__ == '__main__':
    main()
