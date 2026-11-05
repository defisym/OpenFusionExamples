package _DeLib;

import Application.CRunApp;
import RunLoop.CRun;
import android.app.Activity;

/** Windows' application-owned FFmpeg file cache, held by native Fusion storage. */
public final class FFMpegStorage extends FusionStorage {
    private static final int STORAGE_ID = 'F' | ('F' << 8) | ('M' << 16) | ('G' << 24);
    public final MediaSource.Resolver files;
    private FFMpegStorage(CRunApp app, Activity activity) { super(app, activity, STORAGE_ID); files=FusionFiles.resolver(app,activity); }
    public static FFMpegStorage get(CRun run,Activity activity) {
        return acquire(run,activity,STORAGE_ID,FFMpegStorage.class,()->new FFMpegStorage(run.rhApp,activity));
    }
    @Override protected void update() {}
    @Override protected void release() { files.close(); }
}
