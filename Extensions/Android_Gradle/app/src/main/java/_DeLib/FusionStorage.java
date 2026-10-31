package _DeLib;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import Application.CRunApp;
import Extensions.CExtStorage;
import RunLoop.CRun;
import java.util.IdentityHashMap;
import java.util.Map;

/** Lifecycle adaptation for Fusion's existing application extension storage. */
public abstract class FusionStorage extends CExtStorage {
    protected final CRunApp app;
    protected final Activity activity;
    private final Map<CRunApp, Object> generations = new IdentityHashMap<>();
    private Handler handler;
    private boolean closed, background;
    protected FusionStorage(CRunApp app, Activity activity, int id) {
        this.app = app; this.activity = activity; this.id = id;
        // initGlobal replaces playerNames at Restart Application, even when the
        // subapp shares all values. Frame changes/Restart Frame preserve it.
        for (CRunApp owner = app; owner != null; owner = owner.parentApp) generations.put(owner, owner.playerNames);
    }
    protected interface Factory<T> { T create(); }
    protected static <T extends FusionStorage> T acquire(CRun run, Activity activity, int id, Class<T> type, Factory<T> factory) {
        synchronized (FusionStorage.class) {
            if (run == null || run.rhApp == null || activity == null) throw new IllegalArgumentException("Fusion run and Activity are required");
            CExtStorage saved = run.getStorage(id);
            if (saved != null && !type.isInstance(saved)) throw new IllegalStateException("Extension storage ID collision");
            T data = type.cast(saved);
            if (data != null && !data.valid()) { data.close(); data = null; }
            if (data == null) {
                data = factory.create();
                try { run.addStorage(data, id); data.watch(); }
                catch (RuntimeException | LinkageError error) { data.close(); throw error; }
            }
            return data;
        }
    }
    protected final boolean valid() {
        if (closed || activity.isFinishing() || activity.isDestroyed()) return false;
        for (Map.Entry<CRunApp, Object> entry : generations.entrySet()) {
            CRunApp owner = entry.getKey();
            if (owner.file == null || owner.playerNames != entry.getValue() || owner.appRunningState == CRunApp.SL_QUIT
                || owner.appRunningState == CRunApp.SL_RESTART) return false;
        }
        return true;
    }
    protected final boolean ended() { return closed; }
    protected final boolean suspended() {
        boolean paused = background;
        for (CRunApp owner : generations.keySet()) paused |= owner.run != null && owner.run.rh2PauseCompteur != 0;
        return paused;
    }
    protected abstract void update();
    protected abstract void release();
    protected final void watch() {
        handler = new Handler(Looper.getMainLooper());
        activity.getApplication().registerActivityLifecycleCallbacks(callbacks);
        handler.postDelayed(tick, 50);
    }
    final void maintain() {
        synchronized (FusionStorage.class) { if (!valid()) close(); else update(); }
    }
    protected final void close() {
        if (closed) return;
        closed = true;
        if (handler != null) {
            handler.removeCallbacks(tick);
            activity.getApplication().unregisterActivityLifecycleCallbacks(callbacks);
            handler = null;
        }
        if (app.run != null) { if (app.run.getStorage(id) == this) app.run.delStorage(id); }
        else if (app.extensionStorage != null) app.extensionStorage.remove(this);
        release();
    }
    private final Runnable tick = () -> {
        synchronized (FusionStorage.class) {
            try { maintain(); }
            catch (RuntimeException error) { android.util.Log.e("OFE.Media", "Application storage update failed", error); }
            finally { if (handler != null) handler.postDelayed(this.tick, 50); }
        }
    };
    private final Application.ActivityLifecycleCallbacks callbacks = new Application.ActivityLifecycleCallbacks() {
        public void onActivityPaused(Activity owner) { change(owner, true, false); }
        public void onActivityResumed(Activity owner) { change(owner, false, false); }
        public void onActivityDestroyed(Activity owner) { change(owner, true, true); }
        public void onActivityCreated(Activity owner, Bundle state) {}
        public void onActivityStarted(Activity owner) {}
        public void onActivityStopped(Activity owner) {}
        public void onActivitySaveInstanceState(Activity owner, Bundle state) {}
    };
    private void change(Activity owner, boolean paused, boolean destroyed) {
        synchronized (FusionStorage.class) {
            if (owner != activity || closed) return;
            if (destroyed) close(); else { background = paused; update(); }
        }
    }
}
