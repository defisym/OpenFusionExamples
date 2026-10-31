"""Narrow Fusion API fixtures shared by Android adapter compile/dispatch tests.
These are not a substitute for a Fusion Runtime APK test.
"""
FIXTURES={
'Extensions/CRunExtension': '''public class CRunExtension {
 public static final int REFLAG_DISPLAY=1; public Objects.CExtension ho; public RunLoop.CRun rh;
 public int getNumberOfConditions(){return 0;} public boolean createRunObject(Services.CBinaryFile f,RunLoop.CCreateObjectInfo c,int v){return false;}
 public int handleRunObject(){return 0;} public void displayRunObject(){} public void reinitDisplay(){}
 public void pauseRunObject(){} public void continueRunObject(){} public void destroyRunObject(boolean f){}
 public boolean condition(int n,Conditions.CCndExtension c){return false;} public void action(int n,Actions.CActExtension a){}
 public Expressions.CValue expression(int n){return null;}}''',
'Objects/CExtension': '''public class CExtension { public int hoX,hoY,hoImgWidth,hoImgHeight,hoImgXSpot,hoImgYSpot;
 public int parameters; public Expressions.CValue[] values=new Expressions.CValue[0]; public void setParams(Expressions.CValue... args){values=args;parameters=0;} public Objects.CRCom roc; public Sprites.CRSpr ros; public Expressions.CValue getExpParam(){int n=parameters++;return n<values.length?values[n]:new Expressions.CValue(0);} public void generateEvent(int n,int p){} }''',
'Objects/CRCom': 'public class CRCom {public float rcAngle,rcScaleX=1,rcScaleY=1;}',
'Sprites/CRSpr': 'public class CRSpr {public int rsEffect,rsEffectParam,rsEffectShader;}',
'RunLoop/CRun': '''public class CRun {public int rhWindowX,rhWindowY,rh2PauseCompteur; public Application.CRunApp rhApp=new Application.CRunApp();
 public Extensions.CExtStorage getStorage(int id){if(rhApp.extensionStorage!=null)for(Extensions.CExtStorage e:rhApp.extensionStorage)if(e.id==id)return e;return null;}
 public void addStorage(Extensions.CExtStorage e,int id){if(getStorage(id)==null){if(rhApp.extensionStorage==null)rhApp.extensionStorage=new java.util.ArrayList<>();e.id=id;rhApp.extensionStorage.add(e);}}
 public void delStorage(int id){if(rhApp.extensionStorage!=null)for(int n=0;n<rhApp.extensionStorage.size();++n)if(rhApp.extensionStorage.get(n).id==id){rhApp.extensionStorage.remove(n);--n;}}}''',
'Extensions/CExtStorage': 'public class CExtStorage {public int id;}',
'Application/CRunApp': '''public class CRunApp { public static final int SL_RESTART=0,SL_QUIT=6; public int appRunningState=1; public CRunApp parentApp; public RunLoop.CRun run; public Object file=new Object(); public String[] playerNames=new String[4]; public java.util.ArrayList<Extensions.CExtStorage> extensionStorage;
 public java.util.Map<String,byte[]> embedded=new java.util.HashMap<>();
 public HFile openHFile(String name){byte[] bytes=embedded.get(name);if(bytes==null)return null;HFile f=new HFile();f.stream=new java.io.ByteArrayInputStream(bytes);return f;}
 public class HFile {public java.io.InputStream stream;public void close(){try{if(stream!=null)stream.close();}catch(java.io.IOException e){}stream=null;}}
 }''',
'RunLoop/CCreateObjectInfo': 'public class CCreateObjectInfo {}',
'Services/CBinaryFile': '''public class CBinaryFile {private java.nio.ByteBuffer data;
 public CBinaryFile(byte[] bytes){data=java.nio.ByteBuffer.wrap(bytes).order(java.nio.ByteOrder.LITTLE_ENDIAN);}
 public int position(){return data.position();} public int readUnsignedShort(){return data.getShort() & 65535;} public byte readByte(){return data.get();}
 public int readInt(){return data.getInt();} public void skipBytes(int n){data.position(data.position()+n);} }''',
'Conditions/CCndExtension': 'public class CCndExtension {private Object[] args; public CCndExtension(Object... args){this.args=args;} public String getParamExpString(RunLoop.CRun r,int n){return (String)args[n];} public int getParamExpression(RunLoop.CRun r,int n){return ((Number)args[n]).intValue();}}',
'Actions/CActExtension': '''public class CActExtension {private Object[] args; public CActExtension(Object... args){this.args=args;}
 public String getParamExpString(RunLoop.CRun r,int n){return (String)args[n];}
 public int getParamExpression(RunLoop.CRun r,int n){return ((Number)args[n]).intValue();} public double getParamExpDouble(RunLoop.CRun r,int n){return ((Number)args[n]).doubleValue();} }''',
'Expressions/CValue': '''public class CValue {private Object value; public CValue(int n){value=n;}
 public int getInt(){return ((Number)value).intValue();} public double getDouble(){return ((Number)value).doubleValue();} public String getString(){return (String)value;}
 public void forceInt(int n){value=n;} public void forceDouble(double n){value=n;} public void forceString(String n){value=n;} }''',
'Runtime/MMFRuntime': 'public class MMFRuntime extends android.app.Activity {public static MMFRuntime inst; public boolean texPremultiply;}',
'OpenGL/ITexture': '''public abstract class ITexture {public boolean antialiased;
 public abstract int texture(); public abstract int getWidth(); public abstract int getHeight();}''',
'OpenGL/GLRenderer': '''public abstract class GLRenderer {public static GLRenderer inst;
 public abstract void renderFlush(); public abstract void renderImage(ITexture t,boolean a,int x,int y,int w,int h,int shader,int effect,int param);
 public abstract void renderScaledRotatedImage(ITexture t,boolean a,float angle,float sx,float sy,int hx,int hy,int x,int y,int w,int h,int shader,int effect,int param);}''',
}
