package _DeLib;

import Application.CRunApp;
import android.content.Context;
import java.io.FilterInputStream;
import java.io.IOException;

/** Uses Fusion's own embedded-file/OBB/path resolution without an exporter patch. */
public final class FusionFiles {
    private FusionFiles() {}
    public static MediaSource.Resolver resolver(CRunApp app, Context context) {
        return new MediaSource.Resolver(context, name -> {
            CRunApp.HFile file = app.openHFile(name);
            if (file == null) return null;
            if (file.stream == null) { file.close(); return null; }
            return new FilterInputStream(file.stream) {
                @Override public void close() throws IOException { file.close(); }
            };
        });
    }
}
