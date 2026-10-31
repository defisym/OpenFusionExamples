package _DeLib;

import Application.CRunApp;
import android.app.Activity;
import android.app.Application;

/** Production session store against narrow runtime/lifecycle fixtures. */
public final class SessionsTest {
    private static void check(boolean valid, String name) { if (!valid) throw new AssertionError(name); }
    private static RunLoop.CRun run(CRunApp app) {
        if (app.run == null) { app.run = new RunLoop.CRun(); app.run.rhApp = app; }
        return app.run;
    }
    private static void maintain(CRunApp app) {
        if (app.extensionStorage != null) for (Extensions.CExtStorage e : new java.util.ArrayList<>(app.extensionStorage)) ((SDLMixerStorage)e).maintain();
    }
    public static void main(String[] args) {
        Activity activity = new Activity();
        CRunApp app = new CRunApp();
        SDLMixerStorage.Binding first = SDLMixerStorage.attach(run(app), activity);
        SDLMixer original = first.mixer();
        SDLMixerStorage.Binding second = SDLMixerStorage.attach(run(app), activity);
        check(second.mixer() == original, "same application shares playback");
        check(app.extensionStorage.size()==1 && run(app).getStorage(app.extensionStorage.get(0).id)==app.extensionStorage.get(0), "Fusion owns the only global storage entry");
        original.play("tone", "", 0, true, 0, -1);
        first.close(); second.close(); first.close();
        check(!original.wasClosed && original.playing, "last instance destruction retains playback");
        int updates = original.updates;
        // A frame with no instance still maintains AB loops, voices and leases.
        maintain(app);
        check(original.updates > updates, "maintenance without instances");
        app.run = null;
        run(app);
        SDLMixerStorage.Binding next = SDLMixerStorage.attach(run(app), activity);
        check(next.mixer() == original, "next frame and Restart Frame retain session");
        next.suspend(true); check(original.isSuspended, "binding pause");
        next.close(); check(!original.isSuspended, "destroy paused binding clears its vote");
        Application.ActivityLifecycleCallbacks callback = activity.getApplication().callbacks.get(0);
        callback.onActivityPaused(activity); check(original.isSuspended, "background without object");
        callback.onActivityResumed(activity); check(!original.isSuspended, "foreground without object");
        app.run.rh2PauseCompteur = 1; maintain(app);
        check(original.isSuspended, "Fusion pause without object");
        app.run.rh2PauseCompteur = 0; maintain(app);
        check(!original.isSuspended, "Fusion resume");
        CRunApp child = new CRunApp(); child.parentApp = app;
        SDLMixerStorage.Binding sub = SDLMixerStorage.attach(run(child), activity);
        SDLMixer subMixer = sub.mixer(); check(subMixer != original, "subapp state isolated");
        sub.close(); child.appRunningState = CRunApp.SL_QUIT; maintain(child);
        check(subMixer.wasClosed && !original.wasClosed, "subapp exit releases only subapp");
        // Restart can be fully consumed before the observer runs. initGlobal
        // replaces playerNames even in subapps sharing global values.
        app.playerNames = new String[4];
        SDLMixerStorage.Binding restarted = SDLMixerStorage.attach(run(app), activity);
        SDLMixer fresh = restarted.mixer();
        check(original.wasClosed && fresh != original && !fresh.playing, "restart generation reset before reacquire");
        try { next.mixer(); throw new AssertionError("stale binding"); } catch (IllegalStateException expected) {}
        CRunApp child2 = new CRunApp(); child2.parentApp = app;
        SDLMixerStorage.Binding sub2 = SDLMixerStorage.attach(run(child2), activity);
        SDLMixer childMixer = sub2.mixer(); sub2.close(); restarted.close();
        app.playerNames = new String[4]; maintain(app); maintain(child2);
        check(fresh.wasClosed && childMixer.wasClosed, "parent restart releases descendant sessions");
        SDLMixerStorage.Binding last = SDLMixerStorage.attach(run(app), activity);
        SDLMixer ending = last.mixer(); last.close();
        app.run = null; app.appRunningState = CRunApp.SL_QUIT; maintain(app);
        check(ending.wasClosed && activity.getApplication().callbacks.isEmpty(), "EndApp without object cleans session and observer");
        app.appRunningState = 1;
        SDLMixerStorage.Binding destroyed = SDLMixerStorage.attach(run(app), activity);
        SDLMixer ending2 = destroyed.mixer();
        activity.getApplication().callbacks.get(0).onActivityDestroyed(activity);
        check(ending2.wasClosed && activity.getApplication().callbacks.isEmpty(), "Activity destruction cleanup");
        destroyed.close();
        check(app.extensionStorage.isEmpty(), "native storage entry removed on cleanup");
        CRunApp direct=new CRunApp();
        SDLMixerStorage.Binding directBinding=SDLMixerStorage.attach(run(direct),activity);
        SDLMixer directMixer=directBinding.mixer();directBinding.close();direct.file=null;maintain(direct);
        check(directMixer.wasClosed && direct.extensionStorage.isEmpty(),"direct EndApp/subapp destruction without SL_QUIT");
        System.out.println("PASS: application sessions, empty frames, frame/app restart, subapps, background, pause, stale bindings and EndApp cleanup");
    }
}
