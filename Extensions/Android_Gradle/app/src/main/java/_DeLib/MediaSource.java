package _DeLib;

import android.content.Context;
import android.net.Uri;
import java.io.*;
import java.net.HttpURLConnection;
import java.net.URLConnection;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

/** Shared source materialization. Fusion supplies its native application file opener. */
public final class MediaSource implements AutoCloseable {
    public interface Opener { InputStream open(String name) throws IOException; }
    private static final class Entry { File file; String name; int references; boolean cached; }
    public final String path;
    private Resolver owner;
    private String identity;
    private MediaSource(String path, Resolver owner, String identity) { this.path=path; this.owner=owner; this.identity=identity; }
    private static String identity(String name, String key) throws Exception {
        byte[] digest=MessageDigest.getInstance("SHA-256").digest((name+"\0"+key).getBytes(StandardCharsets.UTF_8));
        StringBuilder out=new StringBuilder(); for(byte b:digest)out.append(String.format("%02x",b&255)); return out.toString();
    }
    /** Each Fusion application owns its resolver through CExtStorage. */
    public static final class Resolver implements AutoCloseable {
        private final Context context;
        private final Opener opener;
        private final Map<String, Entry> entries=new HashMap<>();
        private boolean closed;
        public Resolver(Context context, Opener opener) { this.context=context.getApplicationContext(); this.opener=opener; }
        private static String local(String name) {
            if(name.startsWith("file://")&&!name.startsWith("file:///android_asset/")) {
                Uri uri=Uri.parse(name);
                if(uri.getHost()!=null&&!uri.getHost().isEmpty()&&!"localhost".equalsIgnoreCase(uri.getHost())) return name;
                return uri.getPath();
            }
            return name;
        }
        private InputStream input(String name) throws IOException {
            String local=local(name);
            if(new File(local).isFile()) return new FileInputStream(local);
            if(name.startsWith("content://")) {
                InputStream input=context.getContentResolver().openInputStream(Uri.parse(name));
                if(input==null)throw new FileNotFoundException(name);return input;
            }
            String asset=null;
            if(name.startsWith("asset://"))asset=name.substring(8);
            else if(name.startsWith("file:///android_asset/"))asset=Uri.parse(name).getPath().substring("/android_asset/".length());
            if(asset==null&&opener!=null) {
                InputStream input=opener.open(name);if(input!=null)return input;
            }
            if(name.startsWith("https://")||name.startsWith("http://")) {
                URLConnection connection=new java.net.URL(name).openConnection();
                connection.setConnectTimeout(15000);connection.setReadTimeout(15000);
                try {
                    InputStream stream=connection.getInputStream();
                    return new FilterInputStream(stream) { @Override public void close() throws IOException {
                        try {super.close();}finally{if(connection instanceof HttpURLConnection)((HttpURLConnection)connection).disconnect();}
                    }};
                }catch(IOException error){if(connection instanceof HttpURLConnection)((HttpURLConnection)connection).disconnect();throw error;}
            }
            if(asset==null)asset=name.replace('\\','/');
            try {return context.getAssets().open(asset);}
            catch(IOException first) {try{return context.getAssets().open("mmf/"+asset);}catch(IOException missing){missing.addSuppressed(first);throw missing;}}
        }
        public synchronized MediaSource open(String name,String key,boolean cache) throws Exception {return open(name,key,cache,false);}
        public synchronized MediaSource openFile(String name,String key) throws Exception {return open(name,key,false,true);}
        private MediaSource open(String name,String key,boolean cache,boolean localOnly) throws Exception {
            if(closed)throw new IllegalStateException("Media source application ended");
            if(name==null||name.isEmpty()||key==null||name.indexOf('\0')>=0)throw new IllegalArgumentException("Invalid media path or key");
            String local=local(name);
            boolean protocol=name.contains("://")&&!name.startsWith("asset://")&&!name.startsWith("content://")
                &&!name.startsWith("http://")&&!name.startsWith("https://")&&!name.startsWith("file://");
            if(key.isEmpty()&&!cache&&(new File(local).isFile()||(!localOnly&&protocol)))return new MediaSource(local,null,null);
            String id=identity(name,key);Entry entry=entries.get(id);
            if(entry==null) {
                File target=File.createTempFile("ofe-media-",".media",context.getCacheDir());
                try(InputStream input=input(name);OutputStream output=new FileOutputStream(target)) {
                    byte[] buffer=new byte[65536];int count;
                    if(key.isEmpty()){while((count=input.read(buffer))!=-1)output.write(buffer,0,count);}
                    else {
                        Cipher cipher=Cipher.getInstance("AES/CBC/PKCS5Padding");byte[] material=keyMaterial(key);
                        cipher.init(Cipher.DECRYPT_MODE,new SecretKeySpec(Arrays.copyOfRange(material,0,16),"AES"),new IvParameterSpec(Arrays.copyOfRange(material,16,32)));
                        while((count=input.read(buffer))!=-1){byte[] decoded=cipher.update(buffer,0,count);if(decoded!=null)output.write(decoded);}
                        output.write(cipher.doFinal());
                    }
                }catch(Exception error){target.delete();throw error;}
                entry=new Entry();entry.file=target;entry.name=name;entries.put(id,entry);
            }
            ++entry.references;entry.cached|=cache;
            return new MediaSource(entry.file.getAbsolutePath(),this,id);
        }
        public synchronized void cache(String name,String key) throws Exception {try(MediaSource source=open(name,key,true)) {}}
        public synchronized void erase(String name) {
            for(Iterator<Map.Entry<String,Entry>> it=entries.entrySet().iterator();it.hasNext();) {
                Entry entry=it.next().getValue();
                if(name.isEmpty()||name.equals(entry.name)){entry.cached=false;if(entry.references==0){entry.file.delete();it.remove();}}
            }
        }
        private synchronized void release(String identity) {
            Entry entry=entries.get(identity);
            if(entry!=null&&--entry.references==0&&!entry.cached){entry.file.delete();entries.remove(identity);}
        }
        @Override public synchronized void close(){if(!closed){closed=true;erase("");}}
    }
    // Compatibility API for standalone consumers. Fusion adapters use their own
    // native application storage, not this process-wide convenience cache.
    private static final Map<Context,Resolver> defaults=new HashMap<>();
    private static synchronized Resolver defaults(Context context) {
        Context application=context.getApplicationContext();Resolver resolver=defaults.get(application);
        if(resolver==null){resolver=new Resolver(application,null);defaults.put(application,resolver);}return resolver;
    }
    public static MediaSource open(Context context,String name,String key,boolean cache)throws Exception{return defaults(context).open(name,key,cache);}
    public static MediaSource openFile(Context context,String name,String key)throws Exception{return defaults(context).openFile(name,key);}
    public static void cache(Context context,String name,String key)throws Exception{defaults(context).cache(name,key);}
    public static void erase(Context context,String name)throws Exception{defaults(context).erase(name);}
    // Matches Encryption.cpp's Win32 size_t and UTF-16 wchar_t derivation.
    static byte[] keyMaterial(String key) {
        char[] data = new char[16];
        if (key.isEmpty()) { byte[] out = new byte[32]; for (int i = 0; i < 32; ++i) out[i] = (byte)(i % 16); return out; }
        key.getChars(0, Math.min(16, key.length()), data, 0);
        if (key.length() > 16) {
            for (int i = 16; i < key.length(); ++i) for (int j = 0; j < 16; ++j) data[j] ^= key.charAt(i);
        } else if (key.length() < 16) {
            int seed = key.length();
            for (int i = key.length(); i > 0; --i) seed ^= key.charAt(i-1) + 0x9e3779b9 + (seed << 6) + (seed >>> 2);
            for (int i = key.length(); i < 16; ++i) {
                data[i] = (char)(((seed >>> 8) & 255) ^ (seed & 255));
                seed ^= data[i] + 0x9e3779b9 + (seed << 6) + (seed >>> 2);
            }
        }
        ByteBuffer out = ByteBuffer.allocate(32).order(ByteOrder.LITTLE_ENDIAN); for (char c : data) out.putChar(c); return out.array();
    }
    @Override public synchronized void close(){if(owner!=null){owner.release(identity);owner=null;identity=null;}}
}
