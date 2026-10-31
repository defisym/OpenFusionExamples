"""Package and optionally install FindTheWay iOS from a verified Mac core.

Consumes verified Mac native artifacts and creates the current Runtime .ext on Windows.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import importlib.util
import os
from pathlib import Path, PurePosixPath
import plistlib
import shutil
import tempfile
import zipfile

EXTENSIONS = Path(__file__).resolve().parent.parent


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source_hashes(root: Path, include_runtime: bool = True) -> dict[str, str]:
    import re
    owned = [root / 'iOS/FindTheWay/Core']
    pending = []
    if include_runtime:
        owned.append(root / 'iOS/Helpers')
        pending = [p for p in (root / 'iOS/FindTheWay/Runtime').glob('*')
                   if p.suffix in {'.h', '.m', '.mm'}]
    for folder in owned:
        pending.extend(p for p in folder.rglob('*') if p.suffix in {'.h', '.mm', '.cpp'}
                       and 'tests' not in p.parts)
    pending.append(root / 'iOS/FindTheWay/Core/CMakeLists.txt')
    # Producer configuration changes must also invalidate transferred binaries.
    project = root / 'iOS/FindTheWay'
    pending.extend(project / name for name in ('CMakeLists.txt', 'build.py')
                   if (project / name).is_file())
    pending.append(root / 'iOS/FindTheWay/Core/build_xcframework.py')
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
            candidates = [source.parent / include, root / 'iOS/FindTheWay/Core/include' / include,
                          root / '_DeLib' / include, root / '_3rdLib' / include]
            match = next((p for p in candidates if p.is_file()), None)
            if match:
                pending.append(match)
            elif source.is_relative_to((root / 'iOS/FindTheWay/Core').resolve()):
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
    expected = source_hashes(root, include_runtime=kind != 'native')
    recorded = manifest.get('sources', {})
    matches = (all(recorded.get(name) == value for name, value in expected.items())
               if kind == 'native' and isinstance(recorded, dict) else recorded == expected)
    if not matches:
        raise ValueError('iOS artifacts are stale or from a different source checkout; rebuild on Mac')
    if manifest.get('files') != {name: sha(path) for name, path in files.items()}:
        raise ValueError('iOS artifact file set or SHA-256 mismatch')
    return files


def extension_codec():
    script = Path(__file__).resolve().parents[1] / 'iOS/Tools/make_ext.py'
    spec = importlib.util.spec_from_file_location('ftw_ext_codec', script)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def package_contents(files: dict[str, Path], root: Path) -> dict[str, bytes]:
    payload = {}
    runtime = root / 'iOS/FindTheWay/Runtime'
    for name in ('CRunFindTheWay.h', 'CRunFindTheWay.m', 'FTWObjectAdapter.mm', 'FTWPlatformAdapter.mm'):
        payload['Classes/Extensions/' + name] = (runtime / name).read_bytes()
    for name in ('FTWObjectHelpers.h', 'FTWObjectHelpers.mm'):
        payload['Classes/Extensions/' + name] = (root / 'iOS/Helpers' / name).read_bytes()
    info = plistlib.loads(files['FindTheWayCore.xcframework/Info.plist'].read_bytes())
    for entry in info['AvailableLibraries']:
        folder = 'FindTheWayCore.xcframework/' + entry['LibraryIdentifier'] + '/'
        platform = 'iphonesimulator' if entry.get('SupportedPlatformVariant') == 'simulator' else 'iphoneos'
        payload['libFindTheWayCore_' + platform + '.a'] = files[folder + entry['LibraryPath']].read_bytes()
        header = files.get(folder + entry.get('HeadersPath', 'Headers') + '/FindTheWayCore.h')
        if not header:
            raise ValueError('Native bundle is missing the compiled public ABI header')
        data = header.read_bytes()
        previous = payload.get('Classes/Extensions/FindTheWayCore.h', data)
        if previous != data:
            raise ValueError('Device and simulator public ABI headers differ')
        payload['Classes/Extensions/FindTheWayCore.h'] = data
    for name, path in files.items():
        if name.startswith('FindTheWayCore.xcframework/'):
            payload['Frameworks/' + name] = path.read_bytes()
    license_file = root.parent / 'LICENSE'
    if license_file.is_file():
        payload['FindTheWay-LICENSE.txt'] = license_file.read_bytes()
    return payload


def stage(bundle: Path, root: Path, fusion_root: Path | None = None) -> None:
    files = verify(bundle, root)
    destination = fusion_root / 'Data/Runtime/iPhone' if fusion_root else None
    if destination and not destination.is_dir():
        raise ValueError('Fusion iPhone runtime directory is missing; set FusionRoot to the installed Fusion directory')
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
        extension_codec().write_extension('FindTheWay', package_contents(files, root), copied)
        os.replace(archive, support)
        os.replace(copied, extension)
    if destination:
        with tempfile.NamedTemporaryFile(dir=destination, prefix='FindTheWay-', suffix='.partial', delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(extension.read_bytes())
        try:
            os.replace(temporary, destination / 'FindTheWay.ext')
        finally:
            temporary.unlink(missing_ok=True)
        print(f'Installed iOS extension: {destination / "FindTheWay.ext"}')
    print(f'Created and verified iOS extension: {extension}')


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--artifacts', type=Path, required=True)
    parser.add_argument('--install', action='store_true', help='Install into FusionRoot/Data/Runtime/iPhone')
    actions = parser.add_mutually_exclusive_group()
    actions.add_argument('--verify-only', action='store_true',
                         help='Verify transferred native/release artifacts without publishing')
    actions.add_argument('--record-manifest', action='store_true',
                        help='Record hashes after completing Mac build/export/Runtime validation')
    args = parser.parse_args()
    bundle = args.artifacts.resolve()
    if not bundle.is_dir():
        parser.error('iOS artifact directory is missing; prepare it on Mac or explicitly disable iOS packaging')
    if args.verify_only:
        files = verify(bundle, EXTENSIONS)
        kind = json.loads((bundle / 'manifest.json').read_text(encoding='utf-8')).get('kind', 'release')
        print(f'Verified {kind} artifacts: {len(files)} payload files')
        if kind == 'native':
            print('Core verified; Windows can package the current Runtime adapter.')
    elif args.record_manifest:
        files = payload_files(bundle)
        manifest = {'schema': 1, 'sources': source_hashes(EXTENSIONS),
                    'files': {name: sha(path) for name, path in files.items()}}
        (bundle / 'manifest.json').write_text(json.dumps(manifest, indent=2, sort_keys=True), encoding='utf-8')
    else:
        fusion = Path(os.environ.get('FusionRoot') or r'C:\Steam\steamapps\common\Clickteam Fusion 2.5') if args.install else None
        stage(bundle, EXTENSIONS, fusion)


if __name__ == '__main__':
    main()
