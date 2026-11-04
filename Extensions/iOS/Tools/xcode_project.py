#!/usr/bin/env python3
"""Shared OpenStep project parser/localized-edit utilities, reused from PR #100.

Run after each Fusion export, before opening/building the project in Xcode.
Does not build the native core, remove SDK extensions, or change device signing.
"""
from __future__ import annotations
from dataclasses import dataclass
import hashlib
import json
import re
import shlex


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


def payload_digest(path):
    """Normalize only declared artifact text; binary plists and libraries stay exact."""
    data=path.read_bytes()
    if path.suffix in ('.h','.md') or (path.name=='Info.plist' and data.startswith(b'<?xml')):
        data=data.replace(b'\r\n',b'\n')
    return hashlib.sha256(data).hexdigest()
