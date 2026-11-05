package _DeLib;

import android.app.Activity;
import Application.CRunApp;
import RunLoop.CRun;
import java.util.IdentityHashMap;
import java.util.Map;

/** Playback lives in Fusion's native CExtStorage, independently of frame instances. */
public final class SDLMixerStorage extends FusionStorage {
    private static final int STORAGE_ID = 'S' | ('M' << 8) | ('I' << 16) | ('X' << 24);
    private final SDLMixer mixer;
    private final MediaSource.Resolver files;
    private final Map<Binding, Boolean> bindings = new IdentityHashMap<>();
    public static final class Binding implements AutoCloseable {
        private SDLMixerStorage storage;
        private boolean paused;
        private Binding(SDLMixerStorage storage) { this.storage = storage; storage.bindings.put(this, Boolean.TRUE); }
        public SDLMixer mixer() { synchronized (FusionStorage.class) { return live().mixer; } }
        private SDLMixerStorage live() {
            if (storage == null || !storage.valid()) throw new IllegalStateException("SDL Mixer application storage ended");
            return storage;
        }
        public void suspend(boolean value) {
            synchronized (FusionStorage.class) { SDLMixerStorage owner = live(); paused = value; owner.updatePause(); }
        }
        @Override public void close() {
            synchronized (FusionStorage.class) {
                if (storage == null) return;
                SDLMixerStorage owner = storage; storage = null; owner.bindings.remove(this);
                if (!owner.ended()) owner.updatePause();
                // DestroyRunObject only detaches; Fusion application storage owns playback.
            }
        }
    }


    public static Binding attach(CRun run, Activity activity) {
        synchronized (FusionStorage.class) {
            SDLMixerStorage storage = acquire(run, activity, STORAGE_ID, SDLMixerStorage.class,
                () -> new SDLMixerStorage(run.rhApp, activity));
            return new Binding(storage);
        }
    }
    private SDLMixerStorage(CRunApp app, Activity activity) {
        super(app, activity, STORAGE_ID);
        files = FusionFiles.resolver(app, activity);
        try { mixer = SDLMixer.isolated(activity, files); }
        catch (RuntimeException | LinkageError error) { files.close(); throw error; }
    }
    private void updatePause() {
        boolean paused = suspended();
        for (Binding binding : bindings.keySet()) paused |= binding.paused;
        mixer.suspend(paused);
    }
    @Override protected void update() { updatePause(); mixer.update(); }
    @Override protected void release() { try { mixer.close(); } finally { files.close(); bindings.clear(); } }
}
