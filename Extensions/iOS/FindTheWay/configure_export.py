#!/usr/bin/env python3
"""Configure a Fusion-exported FindTheWay Xcode project for arm64 (stdlib only).

Run after each Fusion export, before opening/building the project in Xcode.
Does not build the native core, remove SDK extensions, or change device signing.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
import difflib
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import stat
import tempfile
import xml.etree.ElementTree as ET


@dataclass
class Node:
    value: object
    start: int
    end: int


class Project:
    """Parse OpenStep property lists, retaining spans for localized edits."""
    def __init__(self, text):
        self.text = text
        pattern = r'\s+|/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|[{}()=;,]|[^\s{}()=;,"]+'
        self.tokens = [m for m in re.finditer(pattern, text, re.S)
                       if not (m.group().isspace() or m.group().startswith(('/*', '//')))]
        self.index = 0
        self.root = self.parse()
        if self.index != len(self.tokens):
            raise ValueError('Unexpected trailing project data')
        self.objects = self.root.value['objects'].value

    def take(self, expected=None):
        if self.index >= len(self.tokens):
            raise ValueError('Unexpected end of project')
        token = self.tokens[self.index]
        self.index += 1
        if expected is not None and token.group() != expected:
            raise ValueError(f'Expected {expected}, found {token.group()}')
        return token

    def parse(self):
        token = self.take()
        word = token.group()
        if word == '{':
            value = {}
            while self.tokens[self.index].group() != '}':
                key = self.parse().value
                if not isinstance(key, str) or key in value:
                    raise ValueError(f'Invalid or duplicate project key: {key}')
                self.take('=')
                value[key] = self.parse()
                self.take(';')
            end = self.take('}').end()
        elif word == '(':
            value = []
            while self.tokens[self.index].group() != ')':
                value.append(self.parse())
                if self.tokens[self.index].group() != ')':
                    self.take(',')
            end = self.take(')').end()
        else:
            if word in ('}', ')', '=', ';', ','):
                raise ValueError(f'Unexpected token: {word}')
            value = re.sub(r'\\(.)', lambda m: {'n': '\n', 'r': '\r', 't': '\t'}.get(m[1], m[1]), word[1:-1]) if word.startswith('"') else word
            end = token.end()
        return Node(value, token.start(), end)

    def plain(self, identity):
        return plain(self.objects[identity])


def plain(node):
    value = node.value
    if isinstance(value, dict):
        return {key: plain(child) for key, child in value.items()}
    if isinstance(value, list):
        return [plain(child) for child in value]
    return value


def render(value, depth=0):
    tab = '\t' * depth
    if isinstance(value, dict):
        return '{\n' + ''.join(tab + '\t' + json.dumps(k) + ' = ' + render(v, depth + 1) + ';\n' for k, v in value.items()) + tab + '}'
    if isinstance(value, list):
        return '(\n' + ''.join(tab + '\t' + render(v, depth + 1) + ',\n' for v in value) + tab + ')'
    return json.dumps(value, ensure_ascii=False)


def words(value):
    return list(value) if isinstance(value, list) else shlex.split(value or '')


def version(value):
    if not re.fullmatch(r'\d+(?:\.\d+){0,2}', value):
        raise ValueError(f'Invalid minimum iOS version: {value}')
    return tuple(map(int, value.split('.'))) + (0,) * (3 - len(value.split('.')))


def prepare(bundle, minimum='17.0', target=None):
    path = bundle / 'project.pbxproj'
    original = path.read_bytes().decode('utf-8-sig')
    project = Project(original)
    objects = project.objects
    native = {key: project.plain(key) for key in objects if project.plain(key).get('isa') == 'PBXNativeTarget'}
    apps = {key: obj for key, obj in native.items() if obj.get('productType') == 'com.apple.product-type.application'
            and (target is None or obj.get('name') == target)}
    if len(apps) != 1:
        raise ValueError('Expected one App target; use --target with its actual name')
    app_id, app = next(iter(apps.items()))
    for platform in ('iphoneos', 'iphonesimulator'):
        archive = bundle.parent / f'libFindTheWayCore_{platform}.a'
        if not archive.is_file() or archive.read_bytes()[:8] != b'!<arch>\n':
            raise ValueError(f'Missing or invalid exported core archive: {archive}')
    root_id = project.root.value['rootObject'].value
    config_ids = set()
    for owner in [project.plain(root_id), *native.values()]:
        config_ids.update(project.plain(owner['buildConfigurationList'])['buildConfigurations'])
    settings = {key: project.plain(key)['buildSettings'] for key in config_ids}
    floor = max([minimum] + [v['IPHONEOS_DEPLOYMENT_TARGET'] for v in settings.values() if v.get('IPHONEOS_DEPLOYMENT_TARGET')], key=version)
    edits = []
    for key, values in settings.items():
        before = dict(values)
        for setting in list(values):
            if setting.split('[', 1)[0] in ('ARCHS', 'VALID_ARCHS', 'EXCLUDED_ARCHS', 'ONLY_ACTIVE_ARCH', 'SUPPORTED_PLATFORMS', 'IPHONEOS_DEPLOYMENT_TARGET'):
                del values[setting]
        values.update(ARCHS='arm64', EXCLUDED_ARCHS='', ONLY_ACTIVE_ARCH='NO',
                      SUPPORTED_PLATFORMS='iphoneos iphonesimulator', IPHONEOS_DEPLOYMENT_TARGET=floor)
        app_configs = project.plain(app['buildConfigurationList'])['buildConfigurations']
        if key in app_configs:
            search = words(values.get('LIBRARY_SEARCH_PATHS'))
            for item in ('$(inherited)', '$(SRCROOT)'):
                if item not in search:
                    search.append(item)
            values['LIBRARY_SEARCH_PATHS'] = search
            base = words(values.get('OTHER_LDFLAGS'))
            for sdk in ('iphoneos', 'iphonesimulator'):
                name = f'OTHER_LDFLAGS[sdk={sdk}*]'
                flags = words(values.get(name))
                if base and not any(flags[i:i + len(base)] == base for i in range(len(flags) - len(base) + 1)):
                    flags = base + flags
                for flag in ('$(inherited)', f'-lFindTheWayCore_{sdk}', '-lc++', '-liconv'):
                    if flag not in flags:
                        flags.append(flag)
                values[name] = flags
            values['CODE_SIGNING_ALLOWED[sdk=iphonesimulator*]'] = 'NO'
        if values != before:
            node = objects[key].value['buildSettings']
            edits.append((node.start, node.end, render(values, 3)))
    # Required static linking and direct target dependencies (including Box2D).
    products = {obj.get('productReference'): key for key, obj in native.items()}
    dependencies = list(app.get('dependencies', []))
    known = {project.plain(dep).get('target') for dep in dependencies}
    additions = {}
    for phase in app['buildPhases']:
        phase = project.plain(phase)
        if phase['isa'] != 'PBXFrameworksBuildPhase':
            continue
        for build_id in phase['files']:
            build = project.plain(build_id)
            file = project.plain(build['fileRef'])
            static = file.get('path', '').endswith('.a')
            if static and 'Weak' in build.get('settings', {}).get('ATTRIBUTES', []):
                node = objects[build_id].value['settings']
                values = dict(build['settings'])
                values['ATTRIBUTES'] = [a for a in values['ATTRIBUTES'] if a != 'Weak']
                edits.append((node.start, node.end, render(values, 2)))
            dep_id = products.get(build['fileRef']) if static else None
            if dep_id and dep_id not in known:
                proxy_id = hashlib.sha256(('FTW proxy ' + dep_id).encode()).hexdigest()[:24].upper()
                item_id = hashlib.sha256(('FTW dependency ' + dep_id).encode()).hexdigest()[:24].upper()
                if proxy_id in objects or item_id in objects:
                    raise ValueError('Generated dependency identifier collision')
                additions[proxy_id] = dict(isa='PBXContainerItemProxy', containerPortal=root_id, proxyType='1', remoteGlobalIDString=dep_id, remoteInfo=native[dep_id]['name'])
                additions[item_id] = dict(isa='PBXTargetDependency', target=dep_id, targetProxy=proxy_id)
                dependencies.append(item_id)
                known.add(dep_id)
    if dependencies != app.get('dependencies', []):
        node = objects[app_id].value['dependencies']
        edits.append((node.start, node.end, render(dependencies, 3)))
    if additions:
        at = project.root.value['objects'].end - 1
        edits.append((at, at, ''.join('\t\t' + key + ' = ' + render(value, 2) + ';\n' for key, value in additions.items()) + '\t'))
    changed = original
    for start, end, replacement in sorted(edits, reverse=True):
        changed = changed[:start] + replacement + changed[end:]
    Project(changed)  # Reject a malformed edit before writing any files.
    results = {path: changed.encode('utf-8')} if edits else {}
    for scheme in bundle.glob('xcshareddata/xcschemes/*.xcscheme'):
        old = scheme.read_text()
        ET.fromstring(old)
        def repair_ref(match):
            element = ET.fromstring(match[0])
            identity = element.attrib.get('BlueprintIdentifier')
            if identity not in native:
                return match[0]
            obj = native[identity]
            element.set('ReferencedContainer', 'container:' + bundle.name)
            element.set('BlueprintName', obj['name'])
            product = project.plain(obj['productReference'])
            element.set('BuildableName', product['path'])
            return ET.tostring(element, encoding='unicode')
        new = re.sub(r'<BuildableReference\b[^>]*(?:/\s*>|>\s*</BuildableReference>)', repair_ref, old)
        ET.fromstring(new)
        if new != old:
            results[scheme] = new.encode('utf-8')
    return results, floor


def write_atomic(path, data):
    mode = stat.S_IMODE(path.stat().st_mode)
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as stream:
        tmp = Path(stream.name)
        stream.write(data)
    try:
        tmp.chmod(mode)
        os.replace(tmp, path)
    finally:
        tmp.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('project', type=Path, help='.xcodeproj bundle or exported folder')
    parser.add_argument('--target', help='App target name, only needed for multiple Apps')
    parser.add_argument('--minimum-ios', default='17.0', help='Core minimum OS floor; higher project values are retained')
    parser.add_argument('--dry-run', action='store_true', help='Print changes without writing')
    args = parser.parse_args()
    try:
        bundle = args.project.resolve()
        if bundle.suffix != '.xcodeproj':
            projects = list(bundle.glob('*.xcodeproj'))
            if len(projects) != 1:
                raise ValueError('Specify one .xcodeproj bundle')
            bundle = projects[0]
        version(args.minimum_ios)
        results, floor = prepare(bundle, args.minimum_ios, args.target)
        for path, data in results.items():
            if args.dry_run:
                print(''.join(difflib.unified_diff(path.read_text().splitlines(True), data.decode().splitlines(True), fromfile=str(path), tofile=str(path))), end='')
            else:
                write_atomic(path, data)
                print(f'Updated: {path}')
        print(f'{"Preview" if args.dry_run else "Configured"}: arm64, minimum iOS {floor}; {len(results)} file(s) changed')
    except (OSError, ValueError, KeyError, IndexError, ET.ParseError) as error:
        parser.exit(1, f'ERROR: {error}\n')


if __name__ == '__main__':
    main()
