package _DeLib;
import java.lang.reflect.*;
import java.util.*;

/** Real FFmpeg decode -> shared Mixer X callbacks, with another music client alive. */
public final class AudioTest {
    private static final Map<String,Method> video=new HashMap<>(), music=new HashMap<>();
    private static Object call(Map<String,Method> methods,String name,Object... args)throws Exception {
        try{return methods.get(name).invoke(null,args);}catch(InvocationTargetException error){throw new RuntimeException(name,error.getCause());}
    }
    private static void check(boolean ok,String name){if(!ok)throw new AssertionError(name);}
    private static void methods(Class<?> type,Map<String,Method> out){for(Method m:type.getDeclaredMethods())if(Modifier.isNative(m.getModifiers())){m.setAccessible(true);out.put(m.getName(),m);}}
    private static double clock(long id)throws Exception{return (Double)call(video,"nClock",id);}
    private static void drain(long id,long millis)throws Exception {
        long deadline=System.nanoTime()+millis*1000000L;int state=0;
        while(System.nanoTime()<deadline){state=(Integer)call(video,"nNext",id,(long)clock(id));if(state==2&&!(Boolean)call(video,"nPending",id))return;Thread.sleep(5);}
        throw new AssertionError("audio EOF/drain timed out; state="+state+" clock="+clock(id));
    }
    public static void main(String[] args)throws Exception {
        System.loadLibrary("ofe_ffmpeg");System.loadLibrary("ofe_sdl_mixer");
        methods(FFMpegPlayer.class,video);methods(SDLMixer.class,music);
        long musicId=(Long)call(music,"nCreateIsolated");
        call(music,"nPlay",musicId,"music",args[1],0,true,0,-1);
        long id=(Long)call(video,"nOpen",args[0],null,2,"","",false);
        try {
            call(video,"nState",id,true,false,1f,1.);
            for(int i=0;i<20;++i){call(video,"nNext",id,(long)clock(id));Thread.sleep(5);}
            check(clock(id)>30,"PCM device clock advances");
            call(video,"nState",id,true,true,1f,1.);double frozen=clock(id);
            double musicBefore=(Double)call(music,"nNumber",musicId,2,0,true,"");Thread.sleep(100);
            check(Math.abs(clock(id)-frozen)<.01,"application suspend freezes PCM with another client active");
            check((Double)call(music,"nNumber",musicId,2,0,true,"")>musicBefore,"SDL music keeps rendering");
            call(video,"nSeek",id,400L,1,true);check(Math.abs(clock(id)-400)<.1,"seek discards queued audio and resets clock");
            call(video,"nState",id,true,false,.7f,1.);drain(id,4000);
            check(clock(id)>=1000,"normal-speed drain advances audio");
            call(video,"nState",id,false,false,1f,1.);
            call(video,"nSeek",id,0L,1,true);call(video,"nState",id,true,false,1f,2.);
            drain(id,4000);check(clock(id)>=1000,"pitch-preserving tempo and DSP tail drain");
        }finally{call(video,"nClose",id);}
        check((Boolean)call(music,"nCondition",musicId,0,0,true,""),"closing FFmpeg leaves SDL music alive");
        call(music,"nDestroy",musicId);
        System.out.println("PASS: FFmpeg PCM through real Mixer X, shared music, device clock, app pause, seek, tempo, DSP drain and independent closure");
    }
}
