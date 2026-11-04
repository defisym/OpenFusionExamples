package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import Objects.CObject;
import Objects.CExtension;
import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;
import _DeLib.FiniNative;
import org.json.*;
import java.util.*;
import java.io.*;
import android.util.Base64;
import Runtime.MMFRuntime;
import Application.CRunApp;

/** Fini's original ACE IDs. Runtime objects and events are accessed only in Java. */
public class CRunFini extends CRunExtension {
    private long state;
    private boolean cf25p, allowRVforCS, keep;
    private int localizationFixed;
    private boolean linked;
    private String section="", item="", sectionLoop="", itemLoop="";
    private static final Map<Object,Map<String,String>> saved = new WeakHashMap<>();
    public String lastError="";

    private Object call(String op, Object... args) {
        if(state==0) throw new IllegalStateException("Closed Fini object");
        try { return new JSONTokener(FiniNative.request(state,op,new JSONArray(Arrays.asList(args)).toString())).nextValue(); }
        catch(JSONException e) { throw new IllegalArgumentException(e); }
    }
    private String str(String op,Object... a) { return (String)call(op,a); }
    private String writePath(String path) {return new File(path).isAbsolute()?path:new File(MMFRuntime.inst.getFilesDir(),path).getPath();}
    private String readBytes(String path) throws IOException {
        String privatePath=writePath(path);
        CRunApp.HFile file=rh.rhApp.openHFile(new File(privatePath).isFile()?privatePath:path);
        if(file==null||file.stream==null)throw new IOException("Open INI input");
        try {ByteArrayOutputStream bytes=new ByteArrayOutputStream();byte[] buffer=new byte[16384];int n;while((n=file.stream.read(buffer))!=-1)bytes.write(buffer,0,n);return Base64.encodeToString(bytes.toByteArray(),Base64.NO_WRAP);}
        finally {file.close();}
    }
    private String objectName() {return ho.hoOiList.oilName;}
    @Override public int getNumberOfConditions(){return 4;}
    @Override public boolean createRunObject(CBinaryFile file,CCreateObjectInfo cob,int version) {
        cf25p=version>=2&&file.readByte()!=0; allowRVforCS=version>=2&&file.readByte()!=0; keep=version>=3&&file.readByte()!=0;
        state=FiniNative.create();
        if(keep) {Map<String,String> data=saved.get(rh.rhApp);if(data!=null){String value=data.remove(objectName());if(value!=null)call("load",value);}}
        return state!=0;
    }
    @Override public void destroyRunObject(boolean fast) {
        if(state==0)return;
        try {call("flush");}catch(RuntimeException e){lastError=e.getMessage();}
        try {if(keep&&(Boolean)call("initialized")) saved.computeIfAbsent(rh.rhApp,k->new HashMap<>()).putIfAbsent(objectName(),str("save"));}
        finally {FiniNative.destroy(state);state=0;}
    }
    private CObject object(int fixed) {CObject o=ho.getObjectFromFixed(fixed);return o!=null&&(o.hoFlags&CObject.HOF_DESTROYED)==0?o:null;}
    private CRunFini linkedObject() {CObject o=object(localizationFixed);if(o instanceof CExtension&&((CExtension)o).ext instanceof CRunFini)return (CRunFini)((CExtension)o).ext;return null;}
    @Override public int handleRunObject() {return 0;}
    private void updateLocalization() {if(linked){CRunFini o=linkedObject();call("localData",o!=null&&o.state!=0?o.str("save"):"");if(o==null)linked=false;}}
    @Override public boolean condition(int n,CCndExtension c) {
        try {switch(n){
            case 0:return sectionLoop.equals(c.getParamExpString(rh,0));
            case 1:return itemLoop.equals(c.getParamExpString(rh,0));
            case 2:return (Boolean)call("hasItem",c.getParamExpString(rh,0),c.getParamExpString(rh,1));
            case 3:return (Boolean)call("hasSection",c.getParamExpString(rh,0));
        }}catch(RuntimeException e){lastError=e.getMessage();}return false;
    }
    private boolean valuesAllowed(CObject o) {return o!=null&&o.rov!=null&&(o.hoType>=32||o.hoType==2||((o.hoType==3||o.hoType==7)&&cf25p&&allowRVforCS));}
    private int valueCount(CObject o){return cf25p?o.rov.rvNumberOfValues:Math.min(26,o.rov.rvNumberOfValues);}
    private int stringCount(CObject o){return cf25p?o.rov.rvNumberOfStrings:Math.min(10,o.rov.rvNumberOfStrings);}
    private String packValues(CObject o) throws JSONException {
        if(!valuesAllowed(o))return "";
        JSONObject data=new JSONObject();data.put("flags",o.rov.rvValueFlags&0xffffffffL);
        JSONArray values=new JSONArray(),strings=new JSONArray();
        for(int i=0;i<valueCount(o);i++){CValue v=o.rov.getValue(i);JSONObject value=new JSONObject();value.put("type",v.getType()==CValue.TYPE_INT?0:2);value.put("value",v.getType()==CValue.TYPE_INT?(Object)v.getInt():(Object)v.getDouble());values.put(value);}
        for(int i=0;i<stringCount(o);i++)strings.put(o.rov.getString(i)==null?"":o.rov.getString(i));
        data.put("values",values);data.put("strings",strings);return str("packValues",data);
    }
    private void unpackValues(CObject o,String text) throws JSONException {
        if(!valuesAllowed(o))return;
        JSONObject data=(JSONObject)call("unpackValues",text,valueCount(o),stringCount(o));
        JSONArray values=data.getJSONArray("values"),strings=data.getJSONArray("strings");
        o.rov.rvValueFlags=(int)data.getLong("flags");
        for(int i=0;i<values.length();i++){JSONObject v=values.getJSONObject(i);if(v.getInt("type")==0)o.rov.getValue(i).forceInt(v.getInt("value"));else o.rov.getValue(i).forceDouble(v.getDouble("value"));}
        for(int i=0;i<strings.length();i++)o.rov.setString(i,strings.getString(i));
    }
    @Override public void action(int n,CActExtension a) {
        try {lastError="";switch(n){
            case 0:call("new");break;case 1:call("release");break;
            case 2:{String file=a.getParamFilename2(rh,0),key=a.getParamExpString(rh,1);call("flush");call("fileData",readBytes(file),key,writePath(file));break;}
            case 3:call("fileSave",writePath(a.getParamFilename2(rh,0)),a.getParamExpString(rh,1));break;
            case 4:call("setNumber",a.getParamExpString(rh,0),a.getParamExpString(rh,1),a.getParamExpFloat(rh,2));break;
            case 5:call("set",a.getParamExpString(rh,0),a.getParamExpString(rh,1),a.getParamExpString(rh,2));break;
            case 6:call("delete",a.getParamExpString(rh,0),a.getParamExpString(rh,1));break;
            case 7:{sectionLoop=a.getParamExpString(rh,0);JSONArray names=(JSONArray)call("sections");String loop=sectionLoop;for(int i=0;i<names.length()&&state!=0;i++){sectionLoop=loop;section=names.getString(i);ho.generateEvent(0,0);}break;}
            case 8:{String sec=a.getParamExpString(rh,0);itemLoop=a.getParamExpString(rh,1);if(!sec.isEmpty())section=sec;sec=section;String loop=itemLoop;JSONArray names=(JSONArray)call("items",sec);for(int i=0;i<names.length()&&state!=0;i++){section=sec;itemLoop=loop;item=names.getString(i);ho.generateEvent(1,0);}break;}
            case 9:call("load",a.getParamExpString(rh,0));break;
            case 10:call("autoOn",writePath(a.getParamFilename2(rh,0)),a.getParamExpString(rh,1));break;
            case 11:call("autoOff");break;
            case 12:call("copy",a.getParamExpString(rh,0),a.getParamExpString(rh,1),a.getParamExpression(rh,2)!=0);break;
            case 13:{int fixed=a.getParamExpression(rh,0);String text=a.getParamExpString(rh,1);unpackValues(object(fixed),text);break;}
            case 14:{int fixed=a.getParamExpression(rh,0);String text=a.getParamExpString(rh,1);CObject o=object(fixed);if(o!=null){JSONArray xy=(JSONArray)call("unpackPosition",text);o.hoX=xy.getInt(0);o.hoY=xy.getInt(1);if(o.roc!=null)o.roc.rcChanged=true;}break;}
            case 15:case 19:call("load",str("decode",a.getParamExpString(rh,0),n==15));break;
            case 16:{linked=false;String file=a.getParamFilename2(rh,0),key=a.getParamExpString(rh,1);if(file.isEmpty())call("localData","");else call("localBytes",readBytes(file),key);break;}
            case 17:call("language",a.getParamExpString(rh,0));break;
            case 18:localizationFixed=a.getParamExpression(rh,0);linked=true;updateLocalization();break;
        }}catch(Exception e){lastError=e.getMessage();}
    }

    @Override public CValue expression(int n) {
        try {switch(n){
            case 0:return new CValue(((Number)call("number",ho.getExpParam().getString(),ho.getExpParam().getString())).doubleValue());
            case 1:{String sec=ho.getExpParam().getString(),key=ho.getExpParam().getString();updateLocalization();return new CValue(str("get",sec,key));}
            case 2:return new CValue(section);case 3:return new CValue(item);case 4:return new CValue(str("save"));
            case 5:return new CValue(packValues(object(ho.getExpParam().getInt())));
            case 6:{CObject o=object(ho.getExpParam().getInt());return new CValue(o==null?"":str("packPosition",o.hoX,o.hoY));}
            case 7:case 8:return new CValue(str("encode",str("save"),n==7));
            case 9:case 10:{String text=ho.getExpParam().getString();boolean compressed=ho.getExpParam().getInt()!=0;return new CValue(str(n==9?"encode":"decode",text,compressed));}
        }}catch(Exception e){lastError=e.getMessage();}return n==0?new CValue(0):new CValue("");
    }
}
