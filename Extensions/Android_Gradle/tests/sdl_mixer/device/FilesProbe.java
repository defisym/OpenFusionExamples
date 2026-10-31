package _DeLib;
import android.app.Activity;
import Application.CRunApp;
import java.io.*;
import java.util.Arrays;
import java.net.ServerSocket;
import java.net.Socket;

/** Actual Android assets, ContentResolver, local files and TLS-capable Java URL IO. */
public final class FilesProbe {
    private static void check(boolean ok,String name){if(!ok)throw new AssertionError(name);}
    static byte[] asset(Activity activity,String name)throws Exception {
        try(InputStream input=activity.getAssets().open(name);ByteArrayOutputStream output=new ByteArrayOutputStream()) {
            byte[] buffer=new byte[8192];int count;while((count=input.read(buffer))!=-1)output.write(buffer,0,count);return output.toByteArray();
        }
    }
    private static void verify(MediaSource.Resolver resolver,String name,byte[] expected)throws Exception {
        try(MediaSource source=resolver.openFile(name,"")) {
            try(InputStream input=new FileInputStream(source.path);ByteArrayOutputStream output=new ByteArrayOutputStream()) {
                byte[] buffer=new byte[8192];int count;while((count=input.read(buffer))!=-1)output.write(buffer,0,count);
                check(Arrays.equals(output.toByteArray(),expected),"file read: "+name);
            }
        }
    }
    public static void run(Activity activity)throws Exception {
        CRunApp app=new CRunApp();byte[] bytes=asset(activity,"tone.wav");
        String embedded="C:\\project\\内置声音🎵.wav";app.embedded.put(embedded,bytes);
        File local=new File(activity.getFilesDir(),"声音 🎵.wav");try(OutputStream output=new FileOutputStream(local)){output.write(bytes);}
        try(MediaSource.Resolver resolver=FusionFiles.resolver(app,activity)) {
            for(String name:new String[]{local.getAbsolutePath(),android.net.Uri.fromFile(local).toString(),"asset://tone.wav","file:///android_asset/tone.wav","tone.wav","extra.wav",embedded,
                    "content://ofe.sdlprobe.files/"+android.net.Uri.encode(local.getName())})verify(resolver,name,bytes);
            try(MediaSource decrypted=resolver.openFile("asset://tone.enc","1234567890abcdef")) {
                check(new File(decrypted.path).length()==bytes.length,"AES file read");
            }
            try(ServerSocket server=new ServerSocket(0)) {
                Thread response=new Thread(()->{try(Socket socket=server.accept()) {
                    socket.setSoTimeout(5000);BufferedReader input=new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    while(true){String line=input.readLine();if(line==null||line.isEmpty())break;}
                    OutputStream output=socket.getOutputStream();output.write(("HTTP/1.1 200 OK\r\nContent-Length: "+bytes.length+"\r\nConnection: close\r\n\r\n").getBytes("US-ASCII"));output.write(bytes);output.flush();
                }catch(Exception error){throw new RuntimeException(error);}},"ofe-file-http");
                response.start();verify(resolver,"http://127.0.0.1:"+server.getLocalPort()+"/tone.wav",bytes);response.join(5000);
                check(!response.isAlive(),"HTTP file read completed");
            }
        }
    }
}
