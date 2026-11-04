package Extensions;
import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import Services.CBinaryFile;
import Objects.CObject;
import _DeLib.FiniNative;
import java.nio.file.*;
import java.util.*;
public class IntegrationTest {
    static Set<Integer> actions=new HashSet<>(),expressions=new HashSet<>(),conditions=new HashSet<>();
    static void check(boolean value,String message){if(!value)throw new AssertionError(message);}
    static void act(CRunFini o,int n,Object... p){actions.add(n);o.action(n,new CActExtension(p));check(o.lastError.isEmpty(),"Action "+n+": "+o.lastError);}
    static CValue exp(CRunFini o,int n,Object... p){expressions.add(n);for(Object v:p)o.ho.params.add(v instanceof String?new CValue((String)v):new CValue(((Number)v).intValue()));return o.expression(n);}
    static boolean cnd(CRunFini o,int n,Object... p){conditions.add(n);return o.condition(n,new CCndExtension(p));}
    static CRunFini create(){CRunFini o=new CRunFini();check(o.createRunObject(new CBinaryFile(1,1,1),null,3),"Create");return o;}
    public static void main(String[] args)throws Exception {
        long h=FiniNative.create();String text="中文 😀\u0000end";String q=org.json.JSONObject.quote(text);
        check(FiniNative.request(h,"decode","["+FiniNative.request(h,"encode","["+q+",false]")+",false]").equals(q),"JNI UTF16/NUL");
        FiniNative.destroy(h);FiniNative.destroy(h);boolean rejected=false;try{FiniNative.request(h,"save","[]");}catch(IllegalArgumentException e){rejected=true;}check(rejected,"Validated closed handle");
        CRunFini o=create();check(o.getNumberOfConditions()==4,"Conditions count");check(!cnd(o,3,"A"),"Uninitialized");
        act(o,0);act(o,5,"A","key","你好 😀");act(o,4,"A","number",15.2);check(exp(o,1,"a","KEY").getString().equals("你好 😀"),"String");check(Math.abs(exp(o,0,"A","number").getDouble()-15.2)<0.0001,"Number");
        check(cnd(o,2,"A","key"),"Has item");act(o,12,"A","B",0);act(o,6,"B","key");check(!cnd(o,2,"B","key"),"Delete");
        List<String> seen=new ArrayList<>();o.ho.listener=n->{if(n==0){check(cnd(o,0,"sections"),"Section event");seen.add(exp(o,2).getString());}else{check(cnd(o,1,"items"),"Item event");seen.add(exp(o,3).getString());}};
        act(o,7,"sections");check(seen.contains("A")&&seen.contains("B"),"Sections");seen.clear();act(o,8,"A","items");check(seen.contains("key")&&seen.contains("number"),"Items");
        String ini=exp(o,4).getString(),b64=exp(o,8).getString(),compressed=exp(o,7).getString();
        act(o,1);act(o,19,b64);check(exp(o,4).getString().equals(ini),"Base64 load");act(o,15,compressed);check(exp(o,4).getString().equals(ini),"Compressed load");act(o,9,ini);
        for(int zip=0;zip<2;zip++){String value=exp(o,9,"text 中文 😀",zip).getString();check(exp(o,10,value,zip).getString().equals("text 中文 😀"),"String codec");}
        CObject object=new CObject();object.hoX=-42;object.hoY=12;object.rov.rvValueFlags=0x80000001;object.rov.rvValues[0].forceDouble(15.25);object.rov.rvValues[1].forceInt(-2);object.rov.rvStrings[0]="你好 😀";o.ho.objects.put(123,object);
        String values=exp(o,5,123).getString(),position=exp(o,6,123).getString();object.hoX=0;object.hoY=0;object.rov.rvStrings[0]="changed";act(o,13,123,values);act(o,14,123,position);check(object.hoX==-42&&object.hoY==12&&object.rov.rvStrings[0].equals("你好 😀")&&object.rov.rvValueFlags==0x80000001,"Object restore");
        Path file=Files.createTempFile("fini-jni",".ini");Path local=Files.createTempFile("fini-local",".ini");
        act(o,3,file.toString(),"密码 😀");act(o,2,file.toString(),"密码 😀");check(exp(o,1,"A","key").getString().equals("你好 😀"),"Encrypted file");act(o,10,file.toString(),"密码 😀");act(o,11);
        Files.write(local,"[zh]\n你好 😀=翻译\n".getBytes("UTF-8"));act(o,16,local.toString(),"");act(o,17,"zh");check(exp(o,1,"A","key").getString().equals("翻译"),"Localization file");
        CRunFini other=create();act(other,5,"zh","你好 😀","链接");o.ho.objects.put(456,other.ho);act(o,18,456);check(exp(o,1,"A","key").getString().equals("链接"),"Localization link");other.destroyRunObject(false);check(exp(o,1,"A","key").getString().equals("你好 😀"),"Destroyed link");
        o.action(15,new CActExtension("invalid"));check(!o.lastError.isEmpty(),"Bad input recorded");act(o,5,"A","recovery","OK");
        Object app=o.rh.rhApp;o.destroyRunObject(false);o.destroyRunObject(false);CRunFini next=new CRunFini();next.rh.rhApp=(Application.CRunApp)app;next.createRunObject(new CBinaryFile(1,1,1),null,3);check(exp(next,1,"A","recovery").getString().equals("OK"),"Cross-frame state");next.destroyRunObject(false);
        // Reentrant event may destroy its own runtime object safely.
        CRunFini destroyed=create();act(destroyed,5,"A","B","C");destroyed.ho.listener=n->destroyed.destroyRunObject(false);act(destroyed,7,"destroy");
        check(actions.size()==20&&expressions.size()==11&&conditions.size()==4,"Complete ACE coverage");
        Files.delete(file);Files.delete(local);
        System.out.println("PASS: real JNI/Java adapter; all 20 actions,11 expressions,4 conditions; SDK doubles only");
    }
}
