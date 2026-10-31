package ofe.sdlprobe;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import _DeLib.SDLMixer;
import Runtime.MMFRuntime;
import java.io.File;
import java.nio.charset.StandardCharsets;
import java.io.FileOutputStream;

/** Runs the production Android bridge in an ordinary Activity, without SDLActivity. */
public final class ProbeActivity extends Activity {
    private TextView status;
    @Override public void onCreate(Bundle saved) {
        super.onCreate(saved); new File(getFilesDir(),"result.txt").delete(); MMFRuntime.inst=new MMFRuntime(); status=new TextView(this);status.setText("Testing SDL Mixer JNI...");setContentView(status);
        // Fusion initializes extensions on its run/GL thread, so test without a prepared Looper.
        new Thread(this::probe,"ofe-sdl-probe").start();
    }
    private static void check(boolean valid,String name){if(!valid)throw new AssertionError(name);}
    private void probe(){
        String outcome;
        try {
            if(getIntent().hasExtra("driver")){
                System.loadLibrary("SDL2");
                org.libsdl.app.SDLActivity.nativeSetenv("SDL_AUDIODRIVER",getIntent().getStringExtra("driver"));
            }
        try(SDLMixer first=new SDLMixer(this);SDLMixer second=new SDLMixer(this)){
            for(String name:new String[]{"tone.wav","tone.mp3","tone.ogg","tone.flac","tone.opus","tone.wv"}){
                first.play("asset://"+name,"",0,true,1,-1);
                for(int retry=0;retry<50&&second.number(2,0,true,"")<=0;++retry)Thread.sleep(100);
                check(second.condition(0,0,true,""),"codec playback: "+name);
                check(second.number(2,0,true,"")>0,"audio callback advances: "+name);
            }
            first.play("asset://tone.wav","",0,true,0,80);Thread.sleep(150);first.update();
            first.close();check(second.condition(0,0,true,""),"shared device after one object closes");
            second.suspend(true);double stopped=second.number(2,0,true,"");Thread.sleep(100);
            check(Math.abs(second.number(2,0,true,"")-stopped)<.02,"application pause freezes media position");second.suspend(false);
            second.action(6,0,true,.4,0,"");check(second.number(2,0,true,"")>=.35,"fractional WAV seek");
            second.action(4,0,true,0,0,"");second.update();check(second.condition(3,0,true,""),"paused stream survives update");
            second.suspend(true);Thread.sleep(80);second.suspend(false);
            check(second.condition(3,0,true,""),"app resume preserves user pause");
            second.action(5,0,true,0,0,"");second.action(8,0,true,.05,.1,"");Thread.sleep(200);second.update();
            check(second.number(2,0,true,"")<.15,"AB loop");
            second.play("asset://tone.enc","1234567890abcdef",2,true,1,-1);Thread.sleep(100);
            check(second.condition(0,2,true,""),"AES materialization");
            second.cache("asset://tone.wav","",false);
            long size;
            try(android.content.res.AssetFileDescriptor file=getAssets().openFd("tone.wav")){size=file.getLength();}
            String memory=second.memoryName("内存🎵","asset://tone.wav",0,size);
            second.play(memory,"",3,false,1,-1);check(second.number(4,0,false,"内存🎵")==3,"Unicode memory name");check(!second.condition(6,0,true,"asset://tone.wav"),"memory reader");
            second.erase("asset://tone.wav");check(second.condition(0,3,false,""),"erase retains active reader");
            second.action(7,0,true,-1,0,"");second.update();second.erase("");
            check(second.condition(1,0,true,""),"all stopped");
            outcome="PASS: Android non-SDL Activity, worker-thread JNI bootstrap, WAV/MP3/OGG/FLAC/Opus/WavPack audio callbacks, shared clients, crossfade, seek/AB, pause, AES and handled memory";
        }
        try(SDLMixer reopened=new SDLMixer(this)){
            reopened.play("asset://tone.wav","",0,true,1,-1);
            for(int retry=0;retry<50&&reopened.number(2,0,true,"")<=0;++retry)Thread.sleep(100);
            check(reopened.number(2,0,true,"")>0,"device reopens after last client closes");
        }
        _DeLib.FilesProbe.run(this);
        String rendering=_DeLib.VideoProbe.run(this);
        _DeLib.StorageProbe.run(this);
        outcome += "; native application storage, empty frames, restart, subapps and EndApp; files/content/HTTP/embedded/AES; shared FFmpeg audio and GLES "+rendering;
        }catch(Throwable error){outcome="FAIL: "+android.util.Log.getStackTraceString(error);}
        try(FileOutputStream output=new FileOutputStream(new File(getFilesDir(),"result.txt"))){output.write(outcome.getBytes(StandardCharsets.UTF_8));}
        catch(Exception error){android.util.Log.e("OFESDLProbe","Cannot write result",error);}
        final String text=outcome;android.util.Log.i("OFESDLProbe",text);runOnUiThread(()->status.setText(text));
    }
}
