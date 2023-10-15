// Cannot download from main thread, so a sub thread is needed

package _DeLib;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;

import static _DeLib.AndroidCommon.SaveFile;

public class Downloader extends Thread {
    public static final int Error = -65535;
    public static final int Null = -1;
    public static final int Initialized = 0;
    public static final int Processing = 1;
    public static final int Finished = 65535;

    int State;

    String URL;
    String DirName;
    String FileName;

    public Downloader() {
        this.State = Null;
    }

    public Downloader(String URL, String DirName, String FileName) {
        this.URL = URL;
        this.DirName = DirName;
        this.FileName = FileName;

        this.State = Initialized;
    }

    public int GetState() {
        return this.State;
    }

    public void run() {
        try {
            this.State = Processing;

            String urlDownload = this.URL;
            java.net.URL url = new URL(urlDownload);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();

            if (conn.getResponseCode() != HttpURLConnection.HTTP_OK) {
                throw new Exception("Internet Error");
            }

            if (!SaveFile(conn.getInputStream(), this.DirName, this.FileName)) {
                throw new Exception("File Error");
            }

            this.State = Finished;
        } catch (Exception e) {
            e.printStackTrace();
            this.State = Error;
        }
    }
}
