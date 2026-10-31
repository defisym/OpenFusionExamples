package _DeLib;

import android.content.Context;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/** JNI marshalling only. Channel policy and playback use native SDL Mixer X. */
public final class SDLMixer implements AutoCloseable {
    private static final Map<Long, MediaSource> sources = new HashMap<>();
    private static int references;
    private final Context context;
    private final boolean isolated;
    private final Map<Long, MediaSource> media;
    private long handle;
    private MediaSource.Resolver resolver;
    private static native long nCreate();
    private static native long nCreateIsolated();
    private static native void nDestroy(long handle);
    private static native long nPlay(long handle, String name, String path, int channel, boolean exclusive, int loops, int fade);
    private static native void nAction(long handle, int op, int channel, boolean exclusive, double a, double b, String text);
    private static native boolean nCondition(long handle, int op, int channel, boolean exclusive, String name);
    private static native double nNumber(long handle, int op, int channel, boolean exclusive, String name);
    private static native String nName(long handle, int channel);
    private static native void nCache(long handle, String name, String path, boolean replace);
    private static native void nErase(long handle, String name);
    private static native String nMemoryName(long handle, String access, String binary, long offset, long size);
    private static native long[] nUpdate(long handle);
    private static native void nSuspend(long handle, boolean suspend);
    public SDLMixer(Context context) { this(context, false); }
    static SDLMixer isolated(Context context) { return new SDLMixer(context, true); }
    static SDLMixer isolated(Context context, MediaSource.Resolver resolver) {
        SDLMixer mixer = new SDLMixer(context, true); mixer.resolver = resolver; return mixer;
    }
    private MediaSource openFile(String name, String key) throws Exception {
        return resolver == null ? MediaSource.openFile(context, name, key) : resolver.openFile(name, key);
    }
    private SDLMixer(Context context, boolean isolated) {
        this.context = context.getApplicationContext();
        this.isolated = isolated;
        this.media = isolated ? new HashMap<>() : sources;
        synchronized (SDLMixer.class) {
            SDLAudioRuntime.retain(context);
            try { System.loadLibrary("ofe_sdl_mixer"); handle = isolated ? nCreateIsolated() : nCreate(); if (!isolated) ++references; }
            catch (RuntimeException | LinkageError error) { SDLAudioRuntime.release(); throw error; }
        }
    }
    private long live() { if (handle == 0) throw new IllegalStateException("SDL Mixer is closed"); return handle; }
    public void play(String name, String key, int channel, boolean exclusive, int loops, int fade) throws Exception {
        synchronized (SDLMixer.class) {
            live(); SDLAudioRuntime.requestFocus();
            MediaSource source = null;
            try {
                source = name.startsWith("ofe-mixer-memory://") ? null : openFile(name, key);
                long voice = nPlay(handle, name, source == null ? "" : source.path, channel, exclusive, loops, fade);
                if (source != null) media.put(voice, source);
            } catch (Exception error) {
                if (source != null) source.close();
                if (exclusive && channel >= 0 && channel <= 4095) nAction(handle, 3, channel, true, fade, 0, "");
                throw error;
            }
        }
    }
    public void update() {
        synchronized (SDLMixer.class) {
            long[] voices = nUpdate(live()); Set<Long> active = new HashSet<>(); for (long id : voices) active.add(id);
            for (Iterator<Map.Entry<Long, MediaSource>> it = media.entrySet().iterator(); it.hasNext();) {
                Map.Entry<Long, MediaSource> entry = it.next(); if (!active.contains(entry.getKey())) { entry.getValue().close(); it.remove(); }
            }
        }
    }
    public void action(int op, int channel, boolean exclusive, double a, double b, String text) {
        synchronized (SDLMixer.class) { nAction(live(), op, channel, exclusive, a, b, text); }
    }
    public boolean condition(int op, int channel, boolean exclusive, String name) {
        synchronized (SDLMixer.class) { return nCondition(live(), op, channel, exclusive, name); }
    }
    public double number(int op, int channel, boolean exclusive, String name) {
        synchronized (SDLMixer.class) { return nNumber(live(), op, channel, exclusive, name); }
    }
    public String channelName(int channel) { synchronized (SDLMixer.class) { return nName(live(), channel); } }
    public void cache(String name, String key, boolean replace) throws Exception {
        synchronized (SDLMixer.class) { live(); try (MediaSource source = openFile(name, key)) { nCache(handle, name, source.path, replace); } }
    }
    public void erase(String name) { synchronized (SDLMixer.class) { nErase(live(), name); } }
    public String memoryName(String access, String binary, long offset, long size) {
        synchronized (SDLMixer.class) { return nMemoryName(live(), access, binary, offset, size); }
    }
    public void suspend(boolean value) { synchronized (SDLMixer.class) { nSuspend(live(), value); } }
    @Override public void close() {
        synchronized (SDLMixer.class) {
            if (handle == 0) return;
            nDestroy(handle); handle = 0;
            if (isolated || --references == 0) { for (MediaSource source : media.values()) source.close(); media.clear(); }
            SDLAudioRuntime.release();
        }
    }
}
