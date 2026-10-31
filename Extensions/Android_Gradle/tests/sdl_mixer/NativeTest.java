package _DeLib;
import java.lang.reflect.*;
import java.nio.file.*;
import java.util.*;
/** Uses the production native declarations; Android device setup is covered separately. */
public final class NativeTest {
    private static final Map<String,Method> methods=new HashMap<>();
    private static Object call(String name,Object... args) throws Exception {
        try {return methods.get(name).invoke(null,args);}
        catch(InvocationTargetException error){Throwable cause=error.getCause();if(cause instanceof Exception)throw (Exception)cause;throw error;}
    }
    private static void check(boolean valid,String message){if(!valid)throw new AssertionError(message);}
    public static void main(String[] args)throws Exception{
        System.loadLibrary("ofe_sdl_mixer");
        for(Method method:SDLMixer.class.getDeclaredMethods())if(Modifier.isNative(method.getModifiers())){method.setAccessible(true);methods.put(method.getName(),method);}
        long a=(Long)call("nCreate"),b=(Long)call("nCreate");
        call("nPlay",a,"声音🎵",args[0],0,true,1,-1);
        check((Boolean)call("nCondition",b,0,0,true,""),"shared channel registry");
        check(call("nName",b,0).equals("声音🎵"),"UTF-16/UTF-8 roundtrip");
        call("nDestroy",a);
        check((Boolean)call("nCondition",b,0,0,true,""),"one client close retains playback");
        call("nAction",b,2,0,true,73.,0.,"");check((Double)call("nNumber",b,0,0,true,"")==73.,"gain command");
        call("nAction",b,4,0,true,0.,0.,"");check((Boolean)call("nCondition",b,3,0,true,""),"pause command");
        call("nSuspend",b,true);call("nSuspend",b,false);check((Boolean)call("nCondition",b,3,0,true,""),"suspend preserves user pause");
        call("nCache",b,"bank",args[0],false);
        String view=(String)call("nMemoryName",b,"memory 🎵","bank",0L,Files.size(Paths.get(args[0])));
        check(view.startsWith("ofe-mixer-memory://"),"opaque memory view");
        call("nPlay",b,view,"",1,false,1,-1);check(!(Boolean)call("nCondition",b,6,0,true,"bank"),"cache reader lifetime");
        try{call("nMemoryName",b,"bad","bank",-1L,1L);throw new AssertionError("binary bounds");}catch(IllegalStateException expected){}
        try{call("nPlay",b,"bad",args[0],-1,true,1,-1);throw new AssertionError("negative channel");}catch(IllegalStateException expected){}
        call("nAction",b,7,0,true,-1.,0.,"");call("nUpdate",b);
        call("nErase",b,"");check(call("nMemoryName",b,"gone","bank",0L,1L).equals(""),"cache release");
        call("nDestroy",b);call("nDestroy",b);
        try{call("nNumber",b,9,0,true,"");throw new AssertionError("closed handle");}catch(IllegalStateException expected){}
        long c=(Long)call("nCreate");call("nCache",c,"bank",args[0],false);
        String next=(String)call("nMemoryName",c,"new","bank",0L,Files.size(Paths.get(args[0])));
        check(!next.equals(view),"memory names cannot alias a previous mixer lifetime");
        try{call("nPlay",c,view,"",0,true,1,-1);throw new AssertionError("expired memory name");}catch(IllegalStateException expected){}
        call("nDestroy",c);
        long parent=(Long)call("nCreateIsolated"),child=(Long)call("nCreateIsolated");
        call("nPlay",parent,"parent",args[0],0,true,0,-1);
        check(!(Boolean)call("nCondition",child,0,0,true,""),"subapp channel isolation");
        call("nCache",parent,"bank",args[0],false);
        check(call("nMemoryName",child,"other","bank",0L,1L).equals(""),"subapp cache isolation");
        call("nAction",parent,13,0,true,37.,0.,"");
        check((Double)call("nNumber",child,9,0,true,"")==100.,"subapp master gain isolation");
        call("nPlay",child,"child",args[0],0,true,0,-1);
        call("nSuspend",parent,true);
        double frozen=(Double)call("nNumber",parent,2,0,true,"");
        Thread.sleep(120);
        check(Math.abs((Double)call("nNumber",parent,2,0,true,"")-frozen)<.02,"one app pauses while another renders");
        check((Double)call("nNumber",child,2,0,true,"")>0.,"other app continues callbacks");
        call("nAction",parent,4,0,true,0.,0.,"");
        call("nSuspend",parent,false);
        check((Boolean)call("nCondition",parent,3,0,true,""),"isolated resume preserves user pause");
        call("nDestroy",child);
        check((Boolean)call("nCondition",parent,0,0,true,""),"subapp cleanup preserves parent streams");
        call("nDestroy",parent);
        System.out.println("PASS: actual JNI signatures, Unicode, shared clients, pause, cache bounds and stale handles");
    }
}
