package _DeLib;
import android.content.Context;
public final class SDLMixer implements AutoCloseable {
    public static int op,channel,loops,fade; public static double a,b; public static boolean exclusive,suspended,closed;
    public static String name,key,text;
    public boolean wasClosed,isSuspended,playing; public int updates;
    static SDLMixer isolated(Context owner,MediaSource.Resolver resolver){return new SDLMixer(owner);}
    static SDLMixer isolated(Context owner){return new SDLMixer(owner);}
    public SDLMixer(Context owner){closed=false;}
    public void play(String n,String k,int c,boolean x,int l,int f){playing=true;op=x?0:1;name=n;key=k;channel=c;exclusive=x;loops=l;fade=f;}
    public void action(int o,int c,boolean x,double v,double w,String t){op=o;channel=c;exclusive=x;a=v;b=w;text=t;}
    public boolean condition(int o,int c,boolean x,String n){op=o;channel=c;exclusive=x;name=n;return true;}
    public double number(int o,int c,boolean x,String n){op=o;channel=c;exclusive=x;name=n;return o+.5;}
    public String channelName(int c){op=5;channel=c;return "channel";}
    public void cache(String n,String k,boolean replace){op=replace?12:10;name=n;key=k;}
    public void erase(String n){op=11;name=n;}
    public String memoryName(String access,String binary,long offset,long size){op=7;name=access;key=binary;a=offset;b=size;return "memory";}
    public void update(){++updates;}
    public void suspend(boolean s){suspended=s;isSuspended=s;}
    public void close(){closed=true;wasClosed=true;playing=false;}
}
