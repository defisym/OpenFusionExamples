package _DeLib;
import java.nio.file.*;
import java.util.*;
import java.io.ByteArrayOutputStream;
import java.security.GeneralSecurityException;
import javax.crypto.*;
import javax.crypto.spec.*;

/** JCE is only an independent test oracle; production uses the shared C++ core. */
public final class NativeTest {
    private static void check(boolean value,String message){if(!value)throw new AssertionError(message);}
    private static byte[] encrypt(byte[] source,String key)throws Exception {
        byte[] material=MediaCrypto.material(key);
        Cipher cipher=Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(Cipher.ENCRYPT_MODE,new SecretKeySpec(Arrays.copyOfRange(material,0,16),"AES"),new IvParameterSpec(Arrays.copyOfRange(material,16,32)));
        return cipher.doFinal(source);
    }
    private static byte[] decrypt(byte[] source,String key,int chunk)throws Exception {
        try(MediaCrypto cipher=new MediaCrypto(key)) {
            ByteArrayOutputStream output=new ByteArrayOutputStream();
            for(int i=0;i<source.length;i+=chunk){byte[] input=Arrays.copyOfRange(source,i,Math.min(source.length,i+chunk));output.write(cipher.update(input,input.length));}
            output.write(cipher.finish());
            try{cipher.finish();throw new AssertionError("double final accepted");}catch(GeneralSecurityException expected){}
            return output.toByteArray();
        }
    }
    public static void main(String[] args)throws Exception {
        byte[] empty=MediaCrypto.material("");for(int i=0;i<32;++i)check((empty[i]&255)==i%16,"legacy default bytes");
        check(Arrays.equals(MediaCrypto.material("1234567890abcdef"),"1234567890abcdef".getBytes("UTF-16LE")),"Win32 UTF-16 mapping");
        check(Arrays.equals(MediaCrypto.material("1234567890abcdefZ"),"khinolmbcj;89>?<".getBytes("UTF-16LE")),"long-key folding");
        check(hex(MediaCrypto.material("abc")).equals("6100620063002d0026009b00f900c100e7005c0096001d003900e0005000ad00"),"frozen Win32 short-key seed on a 64-bit JVM");
        Random random=new Random(20261104);int cases=0;
        for(String key:new String[]{"","a","abc","1234567890abcdef","1234567890abcdefZ","密码🔑","a\0b","\ud800","\udfff"}) {
            for(int size:new int[]{0,1,15,16,17,31,32,4097,65537}){
                byte[] source=new byte[size];random.nextBytes(source);byte[] encrypted=encrypt(source,key);
                check(Arrays.equals(source,decrypt(encrypted,key,17)),"streaming roundtrip");++cases;
            }
        }
        if(args.length==2)check(Arrays.equals(Files.readAllBytes(Paths.get(args[1])),decrypt(Files.readAllBytes(Paths.get(args[0])),"12345678ABCDEFGH",4093)),"supplied old Windows ciphertext");
        try{decrypt(new byte[15],"abc",1);throw new AssertionError("truncated ciphertext accepted");}catch(BadPaddingException expected){}
        MediaCrypto closed=new MediaCrypto("abc");closed.close();closed.close();
        try{closed.update(new byte[1],1);throw new AssertionError("closed context");}catch(IllegalStateException expected){}
        System.out.println("PASS: shared-core JNI, "+cases+" JCE vectors, UTF-16/NUL/surrogates, Win32 seed, old asset, padding, finalization and lifetime");
    }
    private static String hex(byte[] bytes){StringBuilder out=new StringBuilder();for(byte b:bytes)out.append(String.format("%02x",b&255));return out.toString();}
}
