"""Small SDK doubles for ACE behavior; not an Android/exporter replacement."""
from pathlib import Path

def create(root):
    files={
    'RunLoop/CRun.java': 'package RunLoop; public class CRun {public Application.CRunApp rhApp=new Application.CRunApp();}',
    'RunLoop/CCreateObjectInfo.java': 'package RunLoop; public class CCreateObjectInfo {}',
    'RunLoop/CObjInfo.java':'package RunLoop; public class CObjInfo {public String oilName="Fini";}',
    'Application/CRunApp.java': '''package Application; public class CRunApp {
        public class HFile {public java.io.InputStream stream;public void close(){try{stream.close();}catch(java.io.IOException e){throw new RuntimeException(e);}}}
        public HFile openHFile(String path){try{HFile f=new HFile();f.stream=new java.io.FileInputStream(path);return f;}catch(java.io.IOException e){return null;}}
    }''',
    'Services/CBinaryFile.java':'''package Services; public class CBinaryFile {private int p;private byte[] data;public CBinaryFile(int... values){data=new byte[values.length];for(int i=0;i<values.length;i++)data[i]=(byte)values[i];}public byte readByte(){return data[p++];}}''',
    'Objects/CObject.java':'''package Objects; public class CObject {
        public static final int HOF_DESTROYED=1;public int hoX,hoY,hoType=2,hoFlags;public RunLoop.CObjInfo hoOiList=new RunLoop.CObjInfo();
        public Values.CRVal rov=new Values.CRVal();public Com roc=new Com();public static class Com {public boolean rcChanged;}
    }''',
    'Objects/CExtension.java':'''package Objects; public class CExtension extends CObject {
        public Extensions.CRunExtension ext;public java.util.Map<Integer,CObject> objects=new java.util.HashMap<>();
        public java.util.ArrayDeque<Expressions.CValue> params=new java.util.ArrayDeque<>();
        public java.util.function.IntConsumer listener=n->{};
        public CObject getObjectFromFixed(int fixed){return objects.get(fixed);}
        public Expressions.CValue getExpParam(){return params.remove();}
        public void generateEvent(int n,int p){listener.accept(n);}
    }''',
    'Values/CRVal.java':'''package Values; public class CRVal {
        public int rvValueFlags,rvNumberOfValues=2,rvNumberOfStrings=2;
        public Expressions.CValue[] rvValues={new Expressions.CValue(0),new Expressions.CValue(0)};public String[] rvStrings={"",""};
        public Expressions.CValue getValue(int n){return rvValues[n];}public String getString(int n){return rvStrings[n];}public void setString(int n,String s){rvStrings[n]=s;}
    }''',
    'Actions/CActExtension.java':'''package Actions; public class CActExtension {
        Object[] p;public CActExtension(Object... p){this.p=p;}
        public String getParamExpString(RunLoop.CRun rh,int n){return (String)p[n];}public String getParamFilename2(RunLoop.CRun rh,int n){return (String)p[n];}
        public int getParamExpression(RunLoop.CRun rh,int n){return ((Number)p[n]).intValue();}public float getParamExpFloat(RunLoop.CRun rh,int n){return ((Number)p[n]).floatValue();}
    }''',
    'Conditions/CCndExtension.java':'''package Conditions; public class CCndExtension {Object[] p;public CCndExtension(Object... p){this.p=p;}public String getParamExpString(RunLoop.CRun rh,int n){return (String)p[n];}}''',
    'Extensions/CRunExtension.java':'''package Extensions; public abstract class CRunExtension {
        public RunLoop.CRun rh=new RunLoop.CRun();public Objects.CExtension ho=new Objects.CExtension();
        public CRunExtension(){ho.ext=this;}public abstract int getNumberOfConditions();
        public abstract boolean createRunObject(Services.CBinaryFile file,RunLoop.CCreateObjectInfo cob,int version);
        public abstract void destroyRunObject(boolean fast);public abstract int handleRunObject();
        public abstract boolean condition(int n,Conditions.CCndExtension c);public abstract void action(int n,Actions.CActExtension a);public abstract Expressions.CValue expression(int n);
    }''',
    'android/util/Base64.java':'''package android.util; public class Base64 {public static final int NO_WRAP=2;public static String encodeToString(byte[] b,int flags){return java.util.Base64.getEncoder().encodeToString(b);}}''',
    'Runtime/MMFRuntime.java':'''package Runtime; public class MMFRuntime {public static MMFRuntime inst=new MMFRuntime();public java.io.File getFilesDir(){return new java.io.File(System.getProperty("java.io.tmpdir"));}}'''
    }
    for name,text in files.items():
        p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text)
