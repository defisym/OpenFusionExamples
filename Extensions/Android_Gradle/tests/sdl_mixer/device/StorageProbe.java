package _DeLib;

import Application.CRunApp;
import RunLoop.CRun;
import android.app.Activity;

/** Real JNI/device output with narrow Fusion storage/lifecycle fixtures. */
public final class StorageProbe {
    private static void check(boolean ok, String name) { if (!ok) throw new AssertionError(name); }
    private static CRun frame(CRunApp app) { CRun run = new CRun(); run.rhApp = app; app.run = run; return run; }
    public static void run(Activity activity) throws Exception {
        CRunApp app = new CRunApp();
        SDLMixerStorage.Binding first = SDLMixerStorage.attach(frame(app), activity);
        SDLMixer audio = first.mixer();
        audio.play("asset://tone.enc", "1234567890abcdef", 0, true, 0, -1);
        audio.action(8, 0, true, .05, .15, "");
        audio.cache("asset://tone.wav", "", false);
        String view;
        try (android.content.res.AssetFileDescriptor file = activity.getAssets().openFd("tone.wav")) {
            view = audio.memoryName("retained", "asset://tone.wav", 0, file.getLength());
        }
        audio.action(13, 0, true, 63, 0, "");
        first.close(); app.run = null;
        // No extension exists. The production Handler still maintains AB loops.
        Thread.sleep(450);
        SDLMixerStorage.Binding next = SDLMixerStorage.attach(frame(app), activity);
        check(next.mixer() == audio, "frame recreation reuses native storage");
        check(audio.condition(0, 0, true, ""), "encrypted source lease survives last frame instance");
        check(audio.number(2, 0, true, "") < .25, "AB maintenance without objects");
        check(audio.number(9, 0, true, "") == 63, "channel settings survive frame change");
        audio.play(view, "", 1, false, 0, -1);
        check(audio.number(4, 0, false, "retained") == 1, "memory cache survives frame change");
        next.close();
        // Complete a restart between observer ticks: no transient SL_RESTART needed.
        app.playerNames = new String[4];
        SDLMixerStorage.Binding restarted = SDLMixerStorage.attach(frame(app), activity);
        SDLMixer fresh = restarted.mixer();
        check(fresh != audio && fresh.condition(1, 0, true, ""), "Restart Application resets playback");
        check(fresh.number(9, 0, true, "") == 100, "Restart Application resets settings");
        check(fresh.memoryName("gone", "asset://tone.wav", 0, 1).isEmpty(), "restart clears cache");
        CRunApp child = new CRunApp(); child.parentApp = app;
        SDLMixerStorage.Binding sub = SDLMixerStorage.attach(frame(child), activity);
        SDLMixer childAudio = sub.mixer();
        fresh.play("asset://tone.wav", "", 0, true, 0, -1);
        check(!childAudio.condition(0, 0, true, ""), "subapp channel isolation");
        childAudio.play("asset://tone.wav", "", 0, true, 0, -1);
        restarted.suspend(true);
        double stopped = fresh.number(2, 0, true, ""); Thread.sleep(100);
        check(Math.abs(fresh.number(2, 0, true, "") - stopped) < .02, "app suspension with another client running");
        check(childAudio.number(2, 0, true, "") > 0, "subapp callbacks continue");
        restarted.suspend(false); sub.close(); child.appRunningState = CRunApp.SL_QUIT;
        ((SDLMixerStorage)child.extensionStorage.get(0)).maintain();
        check(fresh.condition(0, 0, true, ""), "subapp exit retains parent playback");
        restarted.close(); app.appRunningState = CRunApp.SL_QUIT;
        Thread.sleep(200);
        check(app.extensionStorage.isEmpty(), "EndApp cleanup without frame objects");
        check(child.extensionStorage.isEmpty(), "subapp native storage entry removed");
    }
}
