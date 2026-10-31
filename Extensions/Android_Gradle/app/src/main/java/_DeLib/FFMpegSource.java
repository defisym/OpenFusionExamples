package _DeLib;
import android.content.Context;
/** Compatibility facade; source ownership and AES support are shared with SDL Mixer. */
public final class FFMpegSource implements AutoCloseable {
    private final MediaSource source;
    public final String path;
    private FFMpegSource(MediaSource source) { this.source = source; this.path = source.path; }
    public static FFMpegSource open(Context context, String name, String key, boolean cache) throws Exception {
        return new FFMpegSource(MediaSource.open(context, name, key, cache));
    }
    public static void cache(Context context, String name, String key) throws Exception { MediaSource.cache(context, name, key); }
    public static void erase(Context context, String name) throws Exception { MediaSource.erase(context, name); }
    static byte[] keyMaterial(String key) { return MediaSource.keyMaterial(key); }
    @Override public void close() { source.close(); }
}
