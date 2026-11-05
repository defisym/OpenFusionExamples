package _DeLib;
import java.nio.ByteBuffer;
import java.nio.file.*;
import java.util.*;
public final class DecodeTest {
    private static void check(boolean valid, String message) { if (!valid) throw new AssertionError(message); }
    public static void main(String[] args) throws Exception {
        byte[] expected = Files.readAllBytes(Paths.get(args[1]));
        try (FFMpegPlayer p = new FFMpegPlayer(args[0], null, 2, "", "", true)) {
            check(p.info()[0] == 64 && p.info()[1] == 48, "dimensions");
            ByteBuffer pixels = ByteBuffer.allocateDirect(64*48*4);
            int count = 0, retries = 0;
            for (;;) {
                int state = p.tick(true);
                if (state == 2) break;
                if (state == 0) { check(++retries < 100, "decode progress"); continue; }
                p.copyPixels(pixels); byte[] actual = new byte[pixels.capacity()]; pixels.position(0); pixels.get(actual);
                check(Arrays.equals(actual, Arrays.copyOfRange(expected, count*actual.length, (count+1)*actual.length)), "RGBA frame " + count);
                ++count;
            }
            check(count == 12, "all frames including delayed frames: " + count);
            p.seek(500, 1, true); check(p.info()[3] >= 500, "accurate seek");
            p.seek(0, 1, true); check(p.tick(true) == 1 && p.info()[3] == 0, "rewind after preview");
            p.play(true); p.suspend(true); long frozen = p.clock(); Thread.sleep(10); check(p.clock() == frozen, "pause clock");
            p.tempo(2); p.suspend(false); Thread.sleep(15); check(p.clock() > frozen + 15, "tempo clock");
            p.close(); p.close();
            try { p.info(); throw new AssertionError("closed handle"); } catch (IllegalStateException correct) {}
        }
        try { new FFMpegPlayer(args[0]+".missing", null, 0, "", "", true); throw new AssertionError("missing file"); }
        catch (IllegalStateException correct) {}
        try { new FFMpegPlayer(args[0]+"\0x", null, 0, "", "", true); throw new AssertionError("NUL path"); }
        catch (IllegalStateException correct) {}
        check(Arrays.equals(FFMpegSource.keyMaterial("1234567890abcdef"), "1234567890abcdef".getBytes("UTF-16LE")), "Windows AES material");
        check(Arrays.equals(FFMpegSource.keyMaterial("1234567890abcdefZ"), "khinolmbcj;89>?<".getBytes("UTF-16LE")), "Windows long-key derivation");
        try (FFMpegPlayer image = new FFMpegPlayer(args[2], null, 0, "", "", true)) {
            check(image.tick(true) == 1, "still image frame");
            ByteBuffer straight = ByteBuffer.allocateDirect(16*16*4), premultiplied = ByteBuffer.allocateDirect(16*16*4);
            image.copyPixels(straight); image.copyPixels(premultiplied, true);
            int alpha = straight.get(3) & 255;
            check(alpha > 0 && alpha < 255, "source transparency retained");
            check((premultiplied.get(0) & 255) == ((straight.get(0) & 255) * alpha + 127) / 255, "premultiplied Fusion output");
            check(premultiplied.get(3) == straight.get(3), "alpha unchanged");
            try { image.copyPixels(ByteBuffer.allocateDirect(1)); throw new AssertionError("buffer bounds"); }
            catch (IllegalStateException correct) {}
            check(image.tick(true) == 0 && image.info()[5] == 0, "still image held without finish");
            image.seek(0, 1, true); check(image.info()[6] != 0, "still image seek retains texture");
        }
        System.out.println("PASS: JNI RGBA/reference parity, delayed EOF, seek, clock, lifetime and errors");
    }
}
