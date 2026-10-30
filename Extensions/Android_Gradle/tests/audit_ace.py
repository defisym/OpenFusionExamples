"""Compare every public ACE ID against the authoritative C++ definitions."""
from pathlib import Path
import re
repo=Path(__file__).resolve().parents[3]
cpp=(repo/'Extensions/FindTheWay/Main.h').read_text(encoding='utf-8-sig')
java=(repo/'Extensions/Android_Gradle/app/src/main/java/Extensions/CRunFindTheWay.java').read_text()
for prefix,count in [('CND_CONDITION_',29),('ACT_ACTION_',21),('EXP_EXPRESSION_',20)]:
 expected={name:int(n) for name,n in re.findall(r'#define\s+('+prefix+r'\w+)\s+(\d+)',cpp)}
 actual={name:int(n) for name,n in re.findall(r'final int ('+prefix+r'\w+)\s*=\s*(\d+)',java)}
 assert expected==actual,(prefix,expected,actual)
 assert len(expected)==count
 for name in expected:
  assert len(re.findall(r'case\s+'+name+r'\s*:',java))==1,name
assert 'CND_LAST = 29' in java
print('PASS: all 29 conditions / 21 actions / 20 expressions match C++ IDs and have switch handlers')
