package _DeLib;

import android.content.Context;
import android.media.AudioManager;

/** Audio-only SDL initialization within Fusion's existing Activity. */
public final class SDLAudioRuntime {
    private static int references;
    private static boolean initialized;
    private static Context context;
    private static AudioManager audio;
    private static native void nFocus(boolean focused);
    private static final AudioManager.OnAudioFocusChangeListener listener = change -> {
        nFocus(change == AudioManager.AUDIOFOCUS_GAIN);
    };
    private SDLAudioRuntime() {}
    private static void binding(String type, String method, Class<?>[] signature, Object... args) {
        try { Class.forName("org.libsdl.app." + type).getMethod(method, signature).invoke(null, args); }
        catch (ReflectiveOperationException error) { throw new IllegalStateException("SDL2 Java bindings are missing or incompatible", error); }
    }
    public static synchronized void retain(Context owner) {
        if (references++ != 0) return;
        try {
            context = owner.getApplicationContext();
            if (!initialized) {
                System.loadLibrary("SDL2"); System.loadLibrary("SDL2_mixer_ext"); System.loadLibrary("ofe_audio");
                binding("SDL", "setupJNI", new Class<?>[0]); binding("SDL", "initialize", new Class<?>[0]); initialized = true;
            }
            binding("SDL", "setContext", new Class<?>[]{Context.class}, context);
            audio = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
            requestFocus();
        } catch (RuntimeException | LinkageError error) { references = 0; throw error; }
    }
    public static synchronized void requestFocus() {
        if (audio != null) nFocus(audio.requestAudioFocus(listener, AudioManager.STREAM_MUSIC,
            AudioManager.AUDIOFOCUS_GAIN) == AudioManager.AUDIOFOCUS_REQUEST_GRANTED);
    }
    public static synchronized void release() {
        if (references == 0 || --references != 0) return;
        if (audio != null) audio.abandonAudioFocus(listener);
        audio = null;
        if (context != null) binding("SDLAudioManager", "release", new Class<?>[]{Context.class}, context);
        binding("SDL", "setContext", new Class<?>[]{Context.class}, new Object[]{null}); context = null;
    }
}
