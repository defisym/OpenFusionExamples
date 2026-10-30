#!/usr/bin/env python3
"""Reproduce missing declarations before retaining newly added standard includes (Linux/GCC)."""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--cxx', default='g++')
args = parser.parse_args()
core = Path(__file__).resolve().parents[3] / '_DeLib'
third = core.parent / '_3rdLib'
probes = {
    'GeneralDefinition.h': ('#include "GeneralDefinition.h"\nint main(){auto path=CUR_FOLDER;return path.size()==0;}\n', ['cfloat','filesystem']),
    'StringTraits.h': ('#include "StringTraits.h"\nint main(){return CharType<std::string>::Length("x");}\n', ['cstring']),
    'Base64.h': ('#include "Base64.h"\nint main(){Base64<std::string> b;return b.base64_decode("YQ==").size()!=1;}\n', ['stdexcept','climits']),
}
with tempfile.TemporaryDirectory(prefix='delib-includes-') as temp:
    temp = Path(temp)
    for name, (probe, includes) in probes.items():
        source = (third if name == 'Base64.h' else core) / name
        original = source.read_text(encoding='utf-8-sig')
        (temp / 'probe.cpp').write_text(probe)
        command = [args.cxx, '-std=c++20', '-fsyntax-only', '-I', str(temp), '-I', str(core), '-I', str(third), str(temp / 'probe.cpp')]
        (temp / name).write_text(original)
        subprocess.run(command, check=True, capture_output=True, text=True)
        for include in includes:
            marker = '#include <' + include + '>\n'
            assert marker in original, (name, include)
            (temp / name).write_text(original.replace(marker,''))
            result = subprocess.run(command, capture_output=True, text=True)
            assert result.returncode != 0, (name, include, 'removal unexpectedly compiles; reevaluate the include')
            error = next((line for line in result.stderr.splitlines() if 'error:' in line), result.stderr[:200])
            print('PASS: ' + name + ' needs <' + include + '>: ' + error)
        (temp / name).unlink()
