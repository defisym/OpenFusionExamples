package _DeLib;

/** JNI boundary uses real UTF-16 Java strings, never modified UTF-8. */
public final class FiniNative {
    static { System.loadLibrary("fini"); }
    private FiniNative() {}
    public static native long create();
    public static native void destroy(long state);
    public static native String request(long state, String operation, String arguments);
}
