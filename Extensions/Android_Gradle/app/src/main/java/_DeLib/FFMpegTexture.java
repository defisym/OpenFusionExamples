package _DeLib;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.view.Surface;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.concurrent.atomic.AtomicBoolean;
import OpenGL.ITexture;
import OpenGL.GLRenderer;
import Runtime.MMFRuntime;

/** Fusion renders a regular ITexture; only the hardware adaptor sees external OES. */
public final class FFMpegTexture extends ITexture implements AutoCloseable {
    private int texture, external, framebuffer, program, vbo, width, height;
    private SurfaceTexture producer;
    private Surface surface;
    private EGLContext context;
    private final AtomicBoolean available = new AtomicBoolean();
    private final float[] transform = new float[16];
    private ByteBuffer pixels;
    private static native void nBlit(int program, int external, int framebuffer, int vbo,
                                    int width, int height, float[] transform);
    private static int integer(int name) { int[] value = new int[1]; GLES20.glGetIntegerv(name, value, 0); return value[0]; }
    public boolean contextValid() { return context == null || context.equals(EGL14.eglGetCurrentContext()); }
    public Surface decoderSurface() {
        if (surface != null) return surface;
        context = EGL14.eglGetCurrentContext();
        if (EGL14.EGL_NO_CONTEXT.equals(context)) return null;
        int old = integer(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES);
        int[] ids = new int[1]; GLES20.glGenTextures(1, ids, 0); external = ids[0];
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, external);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, old);
        producer = new SurfaceTexture(external);
        producer.setOnFrameAvailableListener(st -> available.set(true));
        surface = new Surface(producer); return surface;
    }
    private void resize(int w, int h) {
        if (w == width && h == height && texture != 0) return;
        context = EGL14.eglGetCurrentContext();
        width = w; height = h;
        if (w <= 0 || h <= 0 || (long)w * h > 67108864L) throw new IllegalStateException("Invalid video size");
        int old = integer(GLES20.GL_TEXTURE_BINDING_2D);
        if (texture == 0) { int[] ids = new int[1]; GLES20.glGenTextures(1, ids, 0); texture = ids[0]; }
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, w, h, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, old);
        pixels = null;
    }
    public void copySoftware(FFMpegPlayer player, int w, int h) {
        GLRenderer.inst.renderFlush(); resize(w, h);
        if (pixels == null) pixels = ByteBuffer.allocateDirect(w * h * 4).order(ByteOrder.nativeOrder());
        pixels.clear(); player.copyPixels(pixels, MMFRuntime.inst.texPremultiply);
        int old = integer(GLES20.GL_TEXTURE_BINDING_2D), alignment = integer(GLES20.GL_UNPACK_ALIGNMENT);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture); GLES20.glPixelStorei(GLES20.GL_UNPACK_ALIGNMENT, 1);
        GLES20.glTexSubImage2D(GLES20.GL_TEXTURE_2D, 0, 0, 0, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, pixels);
        GLES20.glPixelStorei(GLES20.GL_UNPACK_ALIGNMENT, alignment); GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, old);
    }
    private static int shader(int type, String source) {
        int s = GLES20.glCreateShader(type); GLES20.glShaderSource(s, source); GLES20.glCompileShader(s);
        int[] ok = new int[1]; GLES20.glGetShaderiv(s, GLES20.GL_COMPILE_STATUS, ok, 0);
        if (ok[0] == 0) { String error = GLES20.glGetShaderInfoLog(s); GLES20.glDeleteShader(s); throw new IllegalStateException(error); }
        return s;
    }
    private void initHardwareCopy() {
        // Fusion bitmap uploads expose their first (top) row at v=0; invert the FBO output to match.
        int vs = shader(GLES20.GL_VERTEX_SHADER, "attribute vec2 position; varying vec2 uv; uniform mat4 transform; void main(){ gl_Position=vec4(position,0.,1.); vec2 tc=(position+1.)*.5; tc.y=1.-tc.y; uv=(transform*vec4(tc,0.,1.)).xy; }");
        int fs = shader(GLES20.GL_FRAGMENT_SHADER, "#extension GL_OES_EGL_image_external : require\nprecision mediump float; varying vec2 uv; uniform samplerExternalOES image; void main(){gl_FragColor=texture2D(image,uv);}");
        program = GLES20.glCreateProgram(); GLES20.glAttachShader(program, vs); GLES20.glAttachShader(program, fs);
        GLES20.glBindAttribLocation(program, 0, "position"); GLES20.glLinkProgram(program);
        GLES20.glDeleteShader(vs); GLES20.glDeleteShader(fs);
        int[] ok = new int[1]; GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, ok, 0);
        if (ok[0] == 0) throw new IllegalStateException(GLES20.glGetProgramInfoLog(program));
        int[] ids = new int[1]; GLES20.glGenFramebuffers(1, ids, 0); framebuffer = ids[0];
        GLES20.glGenBuffers(1, ids, 0); vbo = ids[0];
        int old = integer(GLES20.GL_ARRAY_BUFFER_BINDING); GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vbo);
        FloatBuffer vertices = ByteBuffer.allocateDirect(32).order(ByteOrder.nativeOrder()).asFloatBuffer();
        vertices.put(new float[]{-1,-1, 1,-1, -1,1, 1,1}).position(0);
        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, 32, vertices, GLES20.GL_STATIC_DRAW);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, old);
    }
    public boolean copyHardware(int w, int h) {
        if (producer == null || !available.getAndSet(false)) return false;
        GLRenderer.inst.renderFlush(); resize(w, h);
        int oldExternal = integer(GLES11Ext.GL_TEXTURE_BINDING_EXTERNAL_OES);
        producer.updateTexImage(); producer.getTransformMatrix(transform);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, oldExternal);
        if (program == 0) initHardwareCopy();
        int old = integer(GLES20.GL_FRAMEBUFFER_BINDING);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        boolean complete = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) == GLES20.GL_FRAMEBUFFER_COMPLETE;
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, old);
        if (!complete) throw new IllegalStateException("FFmpeg framebuffer incomplete");
        nBlit(program, external, framebuffer, vbo, w, h, transform); return true;
    }
    @Override public int texture() { return texture == 0 ? -1 : texture; }
    @Override public int getWidth() { return width; }
    @Override public int getHeight() { return height; }
    @Override public void close() {
        if (surface != null) surface.release();
        if (producer != null) { producer.setOnFrameAvailableListener(null); producer.release(); }
        surface = null; producer = null;
        if (contextValid()) {
            if (texture != 0) GLES20.glDeleteTextures(1, new int[]{texture}, 0);
            if (external != 0) GLES20.glDeleteTextures(1, new int[]{external}, 0);
            if (framebuffer != 0) GLES20.glDeleteFramebuffers(1, new int[]{framebuffer}, 0);
            if (vbo != 0) GLES20.glDeleteBuffers(1, new int[]{vbo}, 0);
            if (program != 0) GLES20.glDeleteProgram(program);
        }
        texture = external = framebuffer = program = vbo = width = height = 0;
        pixels = null; context = null; available.set(false);
    }
}
