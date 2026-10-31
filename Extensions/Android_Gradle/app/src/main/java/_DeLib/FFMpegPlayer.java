package _DeLib;

import android.content.Context;
import android.view.Surface;
import java.nio.ByteBuffer;

/** Serialized JNI ownership; playback, clocks and decoding belong to the shared C++ core. */
public final class FFMpegPlayer implements AutoCloseable {
    static { System.loadLibrary("ofe_ffmpeg"); }
    private long handle;
    private boolean retained;
    private boolean playing, suspended;
    private float speed = 1, volume = 1;
    private static native long nOpen(String path, Surface surface, int threads, String video, String audio, boolean mute);
    private static native void nClose(long handle);
    private static native long[] nInfo(long handle);
    private static native int nNext(long handle, long targetMs);
    private static native void nSeek(long handle, long ms, int flags, boolean accurate);
    private static native void nCopy(long handle, ByteBuffer pixels, boolean premultiply);
    private static native void nState(long handle, boolean playing, boolean suspended, float volume, double tempo);
    private static native double nClock(long handle);
    private static native boolean nPending(long handle);
    public FFMpegPlayer(String path, Surface surface, int threads, String video, String audio, boolean mute) {
        this(null, path, surface, threads, video, audio, mute);
    }
    public FFMpegPlayer(Context context, String path, Surface surface, int threads, String video, String audio, boolean mute) {
        if (!mute && context == null) throw new IllegalArgumentException("Android audio requires an application Context");
        try {
            if (!mute) { SDLAudioRuntime.retain(context); retained = true; }
            handle = nOpen(path, surface, threads, video, audio, mute);
        } catch (RuntimeException | LinkageError error) { if (retained) SDLAudioRuntime.release(); retained = false; throw error; }
    }
    private long live() { if (handle == 0) throw new IllegalStateException("FFmpeg player closed"); return handle; }
    public synchronized long[] info() { return nInfo(live()); }
    public synchronized void copyPixels(ByteBuffer buffer) { nCopy(live(), buffer, false); }
    public synchronized void copyPixels(ByteBuffer buffer, boolean premultiply) { nCopy(live(), buffer, premultiply); }
    public synchronized long clock() { return Math.max(0, (long)nClock(live())); }
    private void state() { nState(live(), playing, suspended, volume, speed); }
    public synchronized void play(boolean value) { playing = value; if (value && retained) SDLAudioRuntime.requestFocus(); state(); }
    public synchronized void suspend(boolean value) { suspended = value; state(); }
    public synchronized void volume(int value) { volume = Math.max(0, Math.min(100, value)) / 100f; state(); }
    public synchronized void tempo(float value) {
        if ((Float.isNaN(value) || Float.isInfinite(value)) || value <= 0 || value > 32) throw new IllegalArgumentException("Invalid playback tempo");
        speed = value; state();
    }
    public synchronized void seek(long ms, int flags, boolean accurate) {
        nSeek(live(), ms, flags, accurate);
    }
    public synchronized int tick(boolean preview) {
        if ((!playing || suspended) && !preview) return 0;
        // The native core owns frame scheduling and decoder/filter/audio completion.
        return nNext(live(), preview ? Long.MAX_VALUE : 0);
    }
    @Override public synchronized void close() {
        try { if (handle != 0) { nClose(handle); handle = 0; } }
        finally { if (retained) { retained = false; SDLAudioRuntime.release(); } }
    }
}
