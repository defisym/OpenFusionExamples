package _DeLib;
import Application.CRunApp;
import android.app.Activity;
import java.io.File;
import java.nio.file.Files;
import java.util.Arrays;
import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public final class MediaSourceTest {
    private static void check(boolean ok,String name){if(!ok)throw new AssertionError(name);}
    public static void main(String[] args)throws Exception {
        Activity activity=new Activity();CRunApp first=new CRunApp(),second=new CRunApp();
        String name="C:\\project\\内置音乐🎵.wav";
        byte[] bytes={1,2,3,4};first.embedded.put(name,bytes);second.embedded.put(name,new byte[]{9});
        MediaSource.Resolver one=FusionFiles.resolver(first,activity),two=FusionFiles.resolver(second,activity);
        MediaSource read=one.open(name,"",true);File file=new File(read.path);
        check(Arrays.equals(Files.readAllBytes(file.toPath()),bytes),"Fusion openHFile handles Windows embedded-file name");
        try(MediaSource other=two.open(name,"",true)){check(Files.readAllBytes(new File(other.path).toPath())[0]==9,"same name in separate subapp has separate cache");}
        one.erase(name);check(file.isFile(),"erase retains active reader");read.close();check(!file.exists(),"last reader releases erased file");
        byte[] material=MediaSource.keyMaterial("1234567890abcdef");Cipher cipher=Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(Cipher.ENCRYPT_MODE,new SecretKeySpec(Arrays.copyOfRange(material,0,16),"AES"),new IvParameterSpec(Arrays.copyOfRange(material,16,32)));
        first.embedded.put("encrypted",cipher.doFinal(bytes));
        MediaSource encrypted=one.openFile("encrypted","1234567890abcdef");File decrypted=new File(encrypted.path);
        check(Arrays.equals(Files.readAllBytes(decrypted.toPath()),bytes),"Fusion source AES derivation/decryption");
        try{one.openFile("encrypted","bad");throw new AssertionError("bad key accepted");}catch(javax.crypto.BadPaddingException expected){}
        one.close();check(decrypted.exists(),"EndApp invalidates cache but preserves active reader until close");
        encrypted.close();check(!decrypted.exists(),"final source cleanup after EndApp");
        try{one.open(name,"",false);throw new AssertionError("closed application cache");}catch(IllegalStateException expected){}
        two.close();
        System.out.println("PASS: Fusion file opener, Unicode/Windows paths, per-app cache, AES, erase/read ownership and application cleanup");
    }
}
