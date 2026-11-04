"""Synthetic ELF fixtures test validation/ownership, never serve as release binaries."""
import importlib.util,struct,tempfile,unittest,zipfile,io
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
spec=importlib.util.spec_from_file_location('fini_publish',repo/'Extensions/_Script/Fini.py');publisher=importlib.util.module_from_spec(spec);spec.loader.exec_module(publisher)
def elf(machine=183,align=16384):
    data=bytearray(120);data[:6]=b'\x7fELF\x02\x01';struct.pack_into('<H',data,18,machine);struct.pack_into('<Q',data,32,64);struct.pack_into('<HH',data,54,56,1);struct.pack_into('<I',data,64,1);struct.pack_into('<Q',data,112,align);return data
class PublisherTest(unittest.TestCase):
    def test_package_ownership_and_additive_rules(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp)
            for name in ('libfini.so','libc++_shared.so'):(root/name).write_bytes(elf())
            output=root/'Fini.zip';publisher.package(output,{'arm64-v8a':root},False)
            with zipfile.ZipFile(output) as z:
                names=z.namelist();self.assertIn('app/src/main/java/Extensions/CRunFini.java',names)
                self.assertIn('app/src/main/jniLibs/arm64-v8a/libfini.so',names)
                self.assertFalse(any(n.endswith('build.gradle') or '/Runtime/' in n or '/Objects/' in n for n in names))
                with zipfile.ZipFile(io.BytesIO(z.read('app/libs/Fini-JNI-rules.jar'))) as rules:self.assertIn('META-INF/proguard/Fini.pro',rules.namelist())
    def test_invalid_architecture_alignment_and_atomic_failure(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);binary=root/'libfini.so';output=root/'Fini.zip';output.write_bytes(b'old output')
            for data in (b'invalid',elf(machine=62),elf(align=4096)):
                binary.write_bytes(data)
                with self.assertRaises(RuntimeError):publisher.package(output,{'arm64-v8a':root},False)
                self.assertEqual(output.read_bytes(),b'old output')
if __name__=='__main__':unittest.main()
