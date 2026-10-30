#!/usr/bin/env python3
"""Compile the production helpers as Java 8 and test against small runtime fixtures.

No Android SDK is needed. These fixtures model only the runtime fields touched by
these helpers; this is not an Android device or full runtime integration test.
"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCES = HERE.parents[1] / 'app/src/main/java'
FIXTURES = {
    'Application/CRunApp': '''
        public RunLoop.CRun run;
        public Frame.CRunFrame frame;
    ''',
    'Frame/CRunFrame': '''
        public Events.CEventProgram evtProg;
        public int nLayers = 2;
        public CLayer[] layers = {new CLayer(), new CLayer()};
    ''',
    'Frame/CLayer': '''
        public static final int FLOPT_TOHIDE = 1, FLOPT_VISIBLE = 2;
        public int dwOptions = FLOPT_VISIBLE;
    ''',
    'Events/CEvent': '''
        public static final int EVFLAG2_NOT = 1;
        public int evtFlags2;
        public Params.CParam[] evtParams;
    ''',
    'Conditions/CCndExtension': '',
    'Events/CEventGroup': '',
    'Events/CQualToOiList': 'public short[] qoiList;',
    'Events/CEventProgram': '''
        public int rh2EventCount = 10, rh2ActionCount, rh2ActionLoopCount;
        public boolean rh2ActionLoop, rh2ActionOn;
        public CEventGroup rhEventGroup;
        public CQualToOiList[] qualToOiList = new CQualToOiList[1];
        public Objects.CObject current;
        public void evt_AddCurrentObject(Objects.CObject object) { current = object; }
    ''',
    'Objects/CObject': '''
        public static final int HOF_DESTROYED = 1;
        public short hoNumber, hoOi, hoNextSelected = -1, hoNumNext = -1;
        public int hoFlags, hoIdentifier, hoOEFlags;
        public RunLoop.CObjInfo hoOiList;
        public Sprite ros = new Sprite();
        public static class Sprite {
            public boolean hidden;
            public void obHide() { hidden = true; }
        }
    ''',
    'RunLoop/CObjInfo': '''
        public short oilOi, oilListSelected = -1, oilObject = -1;
        public int oilEventCount, oilNumOfSelected, oilNObjects;
        public String oilName;
    ''',
    'RunLoop/CRun': '''
        public int rhMaxOI = 3;
        public CObjInfo[] rhOiList = new CObjInfo[3];
        public Objects.CObject[] rhObjectList = new Objects.CObject[8];
        public Frame.CRunFrame rhFrame;
        public Events.CEventProgram rhEvtProg;
        public int result, lastLayer, lastX;
        public short lastOi;
        public int f_CreateObject(short hfii, short oi, int x, int y, int dir,
                                  short flags, int layer, int creation) {
            lastLayer = layer; lastOi = oi; lastX = x;
            return result;
        }
    ''',
    'OI/CObjectCommon': 'public static final int OEFLAG_SPRITES = 1;',
    'Params/CParam': '',
    'Params/PARAM_OBJECT': 'public short oiList;',
    'Params/PARAM_CREATE': 'public short cdpHFII, cdpOi; public CPositionInfo position;',
    'Params/CPositionInfo': 'public int x, y, dir, layer;',
    '_DeLib/FusionUtilities': '''
        public static Params.CPositionInfo GetPositionInfo(RunLoop.CRun run, Params.PARAM_CREATE param) {
            return param.position;
        }
    ''',
}
EXTENDS = {
    'Conditions/CCndExtension': 'Events.CEvent',
    'Params/PARAM_OBJECT': 'CParam',
    'Params/PARAM_CREATE': 'CParam',
}


def main():
    java_home = os.environ.get('JAVA_HOME')
    javac = str(Path(java_home) / 'bin/javac') if java_home else 'javac'
    java = str(Path(java_home) / 'bin/java') if java_home else 'java'
    with tempfile.TemporaryDirectory(prefix='object-helpers-') as temp:
        root = Path(temp)
        files = []
        for name, body in FIXTURES.items():
            package, cls = name.split('/')
            parent = EXTENDS.get(name)
            extension = f' extends {parent}' if parent else ''
            path = root / (name + '.java')
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(f'package {package};\npublic class {cls}{extension} {{\n{body}\n}}\n')
            files.append(str(path))
        files += [str(SOURCES / '_3rdLib' / (name + '.java')) for name in
                  ('ObjectBaseClass', 'ObjectSelectionClass', 'ObjectCreationClass')]
        files.append(str(HERE / 'ObjectHelpersTest.java'))
        subprocess.run([javac, '--release', '8', '-d', str(root / 'classes'), *files], check=True)
        subprocess.run([java, '-cp', str(root / 'classes'), 'ObjectHelpersTest'], check=True, timeout=20)


if __name__ == '__main__':
    main()
