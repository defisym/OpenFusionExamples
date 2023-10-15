package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import RunLoop.CCreateObjectInfo;
import Services.CBinaryFile;
import _DeLib.Downloader;
import androidx.annotation.NonNull;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Objects;

import static _DeLib.FusionUtilities.*;

public class CRunAndroidUtilities extends CRunExtension {
    // Define ACE ID here
    // Condition
    final int CND_ISEMBED = 0;
    final int CND_CONDITION_DOWNLOADERROR = 1;
    final int CND_CONDITION_DOWNLOADCOMPLETE = 2;

    final int CND_LAST = 3;

    // Action
    final int ACT_BINARYFILEEXTRACT_SETTINGS = 0;
    final int ACT_EXTRACTBINARYFILE = 1;
    final int ACT_ACTION_DOWNLOAD = 2;

    // Expression
    final int EXP_GETBINARYFILEPATH = 0;

    // Expression return value
    CValue expRet;

    // Binary file settings
    String BinaryFileExtractDir = "";
    String BinaryFileExtractName = "";

    // Download
    Map<String, Downloader> DownloadThread = new HashMap<>();
    String CurDownloadURL = null;

    // Constructor
    public CRunAndroidUtilities() {
        expRet = new CValue(0);
    }

    // Fusion Funcs
    @Override
    public int getNumberOfConditions() {
        return CND_LAST;
    }

    @Override
    public boolean createRunObject(@NonNull final CBinaryFile file, final CCreateObjectInfo cob, final int version) {
        // Read Properties
        // file.readByte();
        // file.readInt();
        // file.readString(260);
        // ......

        // Return
        return true;
    }

    @Override
    public void destroyRunObject(boolean bFast) {
    }

    @Override
    public int handleRunObject() {
        DownloadHandler();

        return 0;
    }

    // ......

    // Jump Table
    // Condition Jump Table
    @Override
    public boolean condition(int num, CCndExtension cnd) {
        switch (num) {
            case CND_ISEMBED: {
                return IsEmbed(ho, cnd.getParamExpString(rh, 0));
            }
            case CND_CONDITION_DOWNLOADERROR:
            case CND_CONDITION_DOWNLOADCOMPLETE: {
                return Objects.equals(cnd.getParamExpString(rh, 0), CurDownloadURL);
            }
        }

        return false;
    }

    // Action Jump Table
    @Override
    public void action(int num, CActExtension act) {
        switch (num) {
            case ACT_BINARYFILEEXTRACT_SETTINGS: {
                this.BinaryFileExtractDir = act.getParamExpString(rh, 0);
                this.BinaryFileExtractName = act.getParamExpString(rh, 1);

                break;
            }
            case ACT_EXTRACTBINARYFILE: {
                ExtractEmbedFile(ho, act.getParamExpString(rh, 0), BinaryFileExtractDir, BinaryFileExtractName);

                break;
            }
            case ACT_ACTION_DOWNLOAD: {
                Download(act.getParamExpString(rh, 0),
                        act.getParamExpString(rh, 1),
                        act.getParamExpString(rh, 2));

                break;
            }
        }
    }

    // Expression Jump Table
    @Override
    public CValue expression(int num) {
        switch (num) {
            case EXP_GETBINARYFILEPATH: {
                expRet.forceString("Not Binary File");

                String SrcPath = ho.getExpParam().getString();
                if (IsEmbed(ho, SrcPath)) {
                    expRet.forceString(GetExtractPath(ho, SrcPath, BinaryFileExtractDir, BinaryFileExtractName));
                }

                return expRet;
            }
        }

        return new CValue(0);
    }

    // Functions
    void Download(String Url, String DirName, String FileName) {
        if (Objects.equals(DirName, "")) {
            DirName = ho.getControlsContext().getFilesDir().getAbsolutePath();
        }

        if (!DirName.endsWith("/") || DirName.endsWith("\\")) {
            DirName = DirName + "/";
        }

        DownloadThread.put(Url, new Downloader(Url, DirName, FileName));
        Objects.requireNonNull(DownloadThread.get(Url)).start();
    }

    void DownloadHandler() {
        if (DownloadThread == null || DownloadThread.size() == 0) {
            return;
        }

        Iterator<Map.Entry<String, Downloader>> Thread = DownloadThread.entrySet().iterator();

        while (Thread.hasNext()) {
            Map.Entry<String, Downloader> ThreadEntry = Thread.next();
            CurDownloadURL = ThreadEntry.getKey();

            if (DownloadHandler(ThreadEntry.getValue())) {
                Thread.remove();
            }

            CurDownloadURL = null;
        }
    }

    boolean DownloadHandler(Downloader Thread) {
        if (Thread == null) {
            return true;
        }

        if (Thread.GetState() == Downloader.Error) {
            ho.generateEvent(CND_CONDITION_DOWNLOADERROR, 0);

            return true;
        }

        if (Thread.GetState() == Downloader.Finished) {
            ho.generateEvent(CND_CONDITION_DOWNLOADCOMPLETE, 0);

            return true;
        }

        return false;
    }
}
