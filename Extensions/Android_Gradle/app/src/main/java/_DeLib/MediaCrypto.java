package _DeLib;

import java.security.GeneralSecurityException;

/** JNI lifetime only. Password mapping, cipher buffering and padding live in the shared C++ core. */
public final class MediaCrypto implements AutoCloseable {
    static { System.loadLibrary("ofe_crypto"); }
    private long handle;
    private static native byte[] nMaterial(String key);
    private static native long nOpen(String key) throws GeneralSecurityException;
    private static native byte[] nUpdate(long handle, byte[] input, int count) throws GeneralSecurityException;
    private static native byte[] nFinish(long handle) throws GeneralSecurityException;
    private static native void nClose(long handle);
    public static byte[] material(String key) { return nMaterial(key); }
    public MediaCrypto(String key) throws GeneralSecurityException { handle = nOpen(key); }
    private long live() { if (handle == 0) throw new IllegalStateException("Media cipher closed"); return handle; }
    public synchronized byte[] update(byte[] input, int count) throws GeneralSecurityException { return nUpdate(live(), input, count); }
    public synchronized byte[] finish() throws GeneralSecurityException { return nFinish(live()); }
    @Override public synchronized void close() { if (handle != 0) { nClose(handle); handle = 0; } }
}
