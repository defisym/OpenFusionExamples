#!/usr/bin/env python3
"""Create a Fusion iOS .ext from owned sources, without GUI dependencies.

Independently implemented from observed SDK package framing and the standard
PKZIP traditional encryption format. No DarkEdif implementation is used.
"""
from __future__ import annotations
import argparse
import hashlib
import io
import os
from pathlib import Path, PurePosixPath
import re
import struct
import tempfile
import zipfile
import zlib

# Format compatibility value used by the legacy SDK, not an access credential.
ZIP_PASSWORD = b'Kjfkj05kfJK'


def transform(data: bytes) -> bytes:
    # The legacy framing transform is its own inverse.
    return bytes((255 + index - value) & 255 for index, value in enumerate(data))


def _crc(value: int, byte: int) -> int:
    value ^= byte
    for _ in range(8):
        value = (value >> 1) ^ (0xedb88320 if value & 1 else 0)
    return value


def _encrypt(data: bytes) -> bytes:
    keys = [0x12345678, 0x23456789, 0x34567890]
    def update(byte):
        keys[0] = _crc(keys[0], byte)
        keys[1] = ((keys[1] + (keys[0] & 255)) * 134775813 + 1) & 0xffffffff
        keys[2] = _crc(keys[2], keys[1] >> 24)
    for byte in ZIP_PASSWORD:
        update(byte)
    output = bytearray()
    for byte in data:
        temp = keys[2] | 2
        output.append(byte ^ (((temp * (temp ^ 1)) >> 8) & 255))
        update(byte)
    return bytes(output)


def safe_name(name: str) -> str:
    path = PurePosixPath(name)
    if not name or path.is_absolute() or '..' in path.parts or '\\' in name or ':' in name or '\0' in name:
        raise ValueError(f'Invalid package path: {name!r}')
    return str(path)


def project_metadata(name: str, names) -> bytes:
    if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name):
        raise ValueError('Extension name must be a C/Objective-C identifier')
    prefix = 'Classes/Extensions/'
    required = {f'{prefix}CRun{name}.h', f'{prefix}CRun{name}.m'}
    if not required.issubset(names):
        raise ValueError('Missing CRun extension .h/.m pair')
    blocks = [[] for _ in range(6)]
    for path in sorted(names):
        if not path.startswith(prefix):
            continue
        filename = path[len(prefix):]
        if '/' in filename:
            raise ValueError('Runtime compilation units must be directly under Classes/Extensions')
        if not re.fullmatch(r'[A-Za-z0-9_.-]+', filename):
            raise ValueError('Unsupported Runtime filename')
        suffix = Path(filename).suffix
        kind = {'.h': 'sourcecode.c.h', '.m': 'sourcecode.c.objc', '.mm': 'sourcecode.cpp.objcpp'}.get(suffix)
        if not kind:
            continue
        ref = hashlib.sha256(('FTW file ' + path).encode()).hexdigest()[:24].upper()
        file_line = f'\t\t{ref} /* {filename} */ = {{isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = {kind}; path = {filename}; sourceTree = "<group>"; }};\n'
        group_line = f'\t\t\t\t{ref} /* {filename} */,\n'
        if suffix == '.h':
            blocks[4].append(file_line)
            blocks[5].append(group_line)
        else:
            build = hashlib.sha256(('FTW build ' + path).encode()).hexdigest()[:24].upper()
            blocks[0].append(f'\t\t{build} /* {filename} in Sources */ = {{isa = PBXBuildFile; fileRef = {ref} /* {filename} */; }};\n')
            blocks[1].append(file_line)
            blocks[2].append(group_line)
            blocks[3].append(f'\t\t\t\t{build} /* {filename} in Sources */,\n')
    return b''.join(''.join(block).encode('utf-8') + b'\0' for block in blocks)


def _zip(files: dict[str, bytes]) -> bytes:
    output = io.BytesIO()
    central = []
    for name, data in sorted(files.items()):
        filename = safe_name(name).encode('utf-8')
        crc = zlib.crc32(data)
        compressor = zlib.compressobj(9, zlib.DEFLATED, -15)
        compressed = compressor.compress(data) + compressor.flush()
        encrypted = _encrypt(os.urandom(11) + bytes([crc >> 24]) + compressed)
        if max(len(data), len(encrypted), output.tell()) >= 0xffffffff or len(filename) > 65535:
            raise ValueError('Legacy .ext ZIP limits exceeded')
        offset = output.tell()
        flags = 0x801  # UTF-8 filename and traditional ZIP encryption.
        output.write(struct.pack('<IHHHHHIIIHH', 0x04034b50, 20, flags, 8, 0, 33,
                                 crc, len(encrypted), len(data), len(filename), 0))
        output.write(filename)
        output.write(encrypted)
        central.append(struct.pack('<IHHHHHHIIIHHHHHII', 0x02014b50, 20, 20, flags, 8, 0, 33,
                                   crc, len(encrypted), len(data), len(filename), 0, 0, 0, 0, 0, offset) + filename)
    if len(central) > 65535:
        raise ValueError('Too many legacy ZIP entries')
    start = output.tell()
    output.write(b''.join(central))
    size = output.tell() - start
    output.write(struct.pack('<IHHHHIIH', 0x06054b50, 0, 0, len(central), len(central), size, start, 0))
    return output.getvalue()


def read_extension(data: bytes) -> dict[str, bytes]:
    with zipfile.ZipFile(io.BytesIO(transform(data))) as archive:
        archive.setpassword(ZIP_PASSWORD)
        files = {}
        for info in archive.infolist():
            if not info.is_dir():
                name = safe_name(info.filename)
                if name in files:
                    raise ValueError('Duplicate .ext entry')
                files[name] = archive.read(info)  # Includes standard ZIP CRC validation.
        return files


def write_extension(name: str, files: dict[str, bytes], output: Path) -> None:
    contents = {safe_name(path): data for path, data in files.items()}
    if 'project.txt' in contents:
        raise ValueError('project.txt is generated by the packager')
    contents['project.txt'] = project_metadata(name, contents)
    encoded = transform(_zip(contents))
    if read_extension(encoded) != contents:
        raise ValueError('Encoded extension verification failed')
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=output.parent, prefix=output.name, suffix='.partial', delete=False) as stream:
        temporary = Path(stream.name)
        stream.write(encoded)
    try:
        os.replace(temporary, output)
    finally:
        temporary.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', required=True)
    parser.add_argument('--project-zip', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    files = {}
    with zipfile.ZipFile(args.project_zip) as archive:
        for basename in (f'CRun{args.name}.h', f'CRun{args.name}.m'):
            matches = [n for n in archive.namelist() if n in (
                'RuntimeIPhone/Classes/Extensions/' + basename, 'Classes/Extensions/' + basename)]
            if len(matches) != 1:
                parser.error(f'Expected one Classes/Extensions/{basename}')
            files['Classes/Extensions/' + basename] = archive.read(matches[0])
    write_extension(args.name, files, args.output)
    print(f'Created and verified: {args.output}')


if __name__ == '__main__':
    main()
