package _DeLib;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;

public class AndroidCommon {
    public static boolean SaveFile(InputStream is,
                                   String dirName, String fileName) {
        try {
            // create file
            File Dir = new File(dirName);

            if (!Dir.exists() && !Dir.mkdir()) {
                throw new Exception("Failed to create dir");
            }

            fileName = dirName + fileName;
            File file = new File(fileName);
            if (file.exists() && !file.delete()) {
                throw new Exception("Failed to delete file");
            }

            OutputStream os = Files.newOutputStream(Paths.get(fileName));

            // write data
            byte[] bs = new byte[1024];
            int len;

            while ((len = is.read(bs)) != -1) {
                os.write(bs, 0, len);
            }

            // close
            os.close();
            is.close();

            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
}
