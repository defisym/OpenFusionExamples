package _DeLib;
import android.app.Activity;
import android.opengl.*;
import java.io.*;
import java.nio.ByteBuffer;
import OpenGL.GLRenderer;
import OpenGL.ITexture;
import Runtime.MMFRuntime;

/** Production decoder/copy adaptors on a real GLES pbuffer; Fusion draw API is a fixture. */
public final class VideoProbe {
    private static void check(boolean ok,String name){if(!ok)throw new AssertionError(name);}
    private static int integer(int name){int[] out=new int[1];GLES20.glGetIntegerv(name,out,0);return out[0];}
    private static byte[] pixels(FFMpegTexture texture){
        int old=integer(GLES20.GL_FRAMEBUFFER_BINDING);int[] frame=new int[1];GLES20.glGenFramebuffers(1,frame,0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,frame[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,GLES20.GL_COLOR_ATTACHMENT0,GLES20.GL_TEXTURE_2D,texture.texture(),0);
        ByteBuffer bytes=ByteBuffer.allocateDirect(texture.getWidth()*texture.getHeight()*4);
        GLES20.glReadPixels(0,0,texture.getWidth(),texture.getHeight(),GLES20.GL_RGBA,GLES20.GL_UNSIGNED_BYTE,bytes);
        byte[] out=new byte[bytes.capacity()];bytes.get(out);GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,old);GLES20.glDeleteFramebuffers(1,frame,0);return out;
    }
    public static String run(Activity activity)throws Exception {
        GLRenderer.inst=new GLRenderer(){public void renderFlush(){} public void renderImage(ITexture t,boolean a,int x,int y,int w,int h,int s,int e,int p){} public void renderScaledRotatedImage(ITexture t,boolean a,float r,float sx,float sy,int hx,int hy,int x,int y,int w,int h,int s,int e,int p){}};
        MMFRuntime.inst.texPremultiply=false;
        EGLDisplay display=EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);int[] version=new int[2];check(EGL14.eglInitialize(display,version,0,version,1),"EGL initialize");
        EGLConfig[] configs=new EGLConfig[1];int[] count=new int[1];
        EGL14.eglChooseConfig(display,new int[]{EGL14.EGL_RENDERABLE_TYPE,4,EGL14.EGL_SURFACE_TYPE,EGL14.EGL_PBUFFER_BIT,EGL14.EGL_RED_SIZE,8,EGL14.EGL_GREEN_SIZE,8,EGL14.EGL_BLUE_SIZE,8,EGL14.EGL_ALPHA_SIZE,8,EGL14.EGL_NONE},0,configs,0,1,count,0);check(count[0]>0,"GLES config");
        EGLContext context=EGL14.eglCreateContext(display,configs[0],EGL14.EGL_NO_CONTEXT,new int[]{EGL14.EGL_CONTEXT_CLIENT_VERSION,2,EGL14.EGL_NONE},0);
        EGLSurface surface=EGL14.eglCreatePbufferSurface(display,configs[0],new int[]{EGL14.EGL_WIDTH,64,EGL14.EGL_HEIGHT,48,EGL14.EGL_NONE},0);
        check(EGL14.eglMakeCurrent(display,surface,surface,context),"GLES context");
        byte[] video=FilesProbe.asset(activity,"video.mp4"),reference=FilesProbe.asset(activity,"video.rgba");
        File file=new File(activity.getFilesDir(),"video.mp4");try(OutputStream output=new FileOutputStream(file)){output.write(video);}
        String hardware="fallback";
        try {
            try(FFMpegTexture texture=new FFMpegTexture();FFMpegPlayer player=new FFMpegPlayer(activity,file.getAbsolutePath(),null,2,"","",false);SDLMixer music=new SDLMixer(activity)) {
                music.play("asset://tone.wav","",0,true,0,-1);check(player.tick(true)==1,"FFmpeg software frame");
                texture.copySoftware(player,64,48);
                byte[] actual=pixels(texture);ByteBuffer decoded=ByteBuffer.allocateDirect(actual.length);player.copyPixels(decoded);byte[] nativePixels=new byte[actual.length];decoded.get(nativePixels);
                check(java.util.Arrays.equals(actual,nativePixels),"software texture matches native RGBA upload");
                long difference=0;for(int i=0;i<actual.length;++i)difference+=Math.abs((actual[i]&255)-(reference[i]&255));
                check(difference/(double)actual.length<3,"software decoder/reference color tolerance: "+difference/(double)actual.length);
                player.play(true);long deadline=System.nanoTime()+5000000000L;int state=0;
                while(System.nanoTime()<deadline){state=player.tick(false);if(state==2)break;Thread.sleep(10);}
                check(state==2,"FFmpeg audio/video completion through shared Mixer X");
                check(music.condition(0,0,true,""),"SDL music still playing after FFmpeg drains");
                player.seek(0,1,true);player.tempo(2);player.play(true);deadline=System.nanoTime()+5000000000L;
                while(System.nanoTime()<deadline){state=player.tick(false);if(state==2)break;Thread.sleep(10);}
                check(state==2,"Android FFmpeg tempo/drain");
            }
            try(FFMpegTexture texture=new FFMpegTexture();FFMpegPlayer player=new FFMpegPlayer(activity,file.getAbsolutePath(),texture.decoderSurface(),2,"","",false)) {
                check(player.tick(true)==1,"hardware/fallback decode");
                if(player.info()[4]!=0) {
                    GLES20.glViewport(3,4,17,19);GLES20.glEnable(GLES20.GL_BLEND);GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
                    boolean copied=false;for(int retry=0;retry<200&&!copied;++retry){copied=texture.copyHardware(64,48);if(!copied)Thread.sleep(10);}
                    check(copied,"MediaCodec SurfaceTexture callback and OES copy");
                    int[] viewport=new int[4];GLES20.glGetIntegerv(GLES20.GL_VIEWPORT,viewport,0);
                    check(viewport[0]==3&&viewport[1]==4&&viewport[2]==17&&viewport[3]==19&&GLES20.glIsEnabled(GLES20.GL_BLEND)&&integer(GLES20.GL_ACTIVE_TEXTURE)==GLES20.GL_TEXTURE1,"hardware copy restores GL state");
                    byte[] actual=pixels(texture);long difference=0;for(int i=0;i<actual.length;++i)difference+=Math.abs((actual[i]&255)-(reference[i]&255));
                    check(difference/(double)actual.length<12,"hardware orientation/color reference");hardware="MediaCodec Surface/OES";
                    player.play(true);long deadline=System.nanoTime()+5000000000L;int state=0;
                    while(System.nanoTime()<deadline){state=player.tick(false);texture.copyHardware(64,48);if(state==2)break;Thread.sleep(10);}
                    check(state==2,"MediaCodec Surface playback with shared PCM clock/drain");
                }else {
                    texture.copySoftware(player,64,48);byte[] actual=pixels(texture);ByteBuffer decoded=ByteBuffer.allocateDirect(actual.length);player.copyPixels(decoded);byte[] nativePixels=new byte[actual.length];decoded.get(nativePixels);
                    check(java.util.Arrays.equals(actual,nativePixels),"hardware request software fallback");
                }
            }
            try(FFMpegTexture texture=new FFMpegTexture();
                FFMpegPlayer player=new FFMpegPlayer(activity,file.getAbsolutePath(),texture.decoderSurface(),1,"h264","",true)) {
                check(player.tick(true)==1 && player.info()[4]==0,"explicit software decoder overrides Surface hardware request");
                texture.copySoftware(player,64,48);
            }
            // Actual content-URI reads feed FFmpeg through the same private materialization path.
            try(MediaSource source=MediaSource.open(activity,"content://ofe.sdlprobe.files/video.mp4","",false);
                FFMpegPlayer player=new FFMpegPlayer(activity,source.path,null,1,"","",true)){check(player.tick(true)==1,"FFmpeg content URI playback");}
        }finally{EGL14.eglMakeCurrent(display,EGL14.EGL_NO_SURFACE,EGL14.EGL_NO_SURFACE,EGL14.EGL_NO_CONTEXT);EGL14.eglDestroySurface(display,surface);EGL14.eglDestroyContext(display,context);EGL14.eglTerminate(display);}
        return hardware;
    }
}
