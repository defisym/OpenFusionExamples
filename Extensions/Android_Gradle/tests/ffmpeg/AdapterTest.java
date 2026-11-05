package Extensions;
import java.nio.*;
import Actions.CActExtension;
import Conditions.CCndExtension;
import Services.CBinaryFile;
public final class AdapterTest {
    private static void check(boolean ok, String message) { if (!ok) throw new AssertionError(message); }
    public static void main(String[] args) {
        CRunFFMpeg extension = new CRunFFMpeg();
        extension.ho = new Objects.CExtension(); extension.rh = new RunLoop.CRun();
        ByteBuffer properties = ByteBuffer.allocate(28).order(ByteOrder.LITTLE_ENDIAN);
        properties.putShort((short)320).putShort((short)240).put((byte)1).put((byte)1).put((byte)1).put((byte)1);
        properties.putInt(8).putInt(64).put((byte)1).put((byte)0).putShort((short)0);
        properties.putInt(7).put((byte)1).put((byte)1).put((byte)1).put((byte)0);
        CBinaryFile file = new CBinaryFile(properties.array());
        check(extension.createRunObject(file, null, 1), "create");
        check(file.position() == 27, "Win32 EDATA alignment");
        check(extension.getNumberOfConditions() == 8, "condition count");
        check(extension.ho.hoImgWidth == 320 && extension.ho.hoImgHeight == 240, "property dimensions");
        check(extension.condition(2, null) && extension.condition(7, null), "loop/hardware properties");
        check(extension.expression(16).getInt() == 8, "thread property");
        extension.action(4, new CActExtension(150)); check(extension.expression(3).getInt() == 100, "volume clamp");
        extension.action(4, new CActExtension(-50)); check(extension.expression(3).getInt() == 0, "negative volume clamp");
        extension.action(5, new CActExtension(0)); check(!extension.condition(2, null), "loop action");
        extension.action(8, new CActExtension(99)); check(extension.expression(16).getInt() == 16, "thread clamp");
        extension.action(13, new CActExtension("none")); check(!extension.condition(7, null), "device selection");
        extension.action(19, new CActExtension(1)); check(extension.condition(7, null), "copy requests hardware");
        extension.action(14, new CActExtension(1920, 1080)); check(extension.ho.hoImgWidth == 1920, "stretch");
        extension.action(15, new CActExtension(1.5)); check(extension.expression(13).getDouble() == 1.5, "tempo double");
        extension.action(17, new CActExtension("h264", "aac"));
        check(extension.expression(14).getString().equals("h264") && extension.expression(15).getString().equals("aac"), "codec overrides");
        extension.expression(4); extension.expression(5); check(extension.ho.parameters == 3, "pointer parameters consumed");
        extension.pauseRunObject(); extension.continueRunObject(); extension.destroyRunObject(false); extension.destroyRunObject(false);
        check(!extension.condition(0, null) && !extension.condition(6, null), "closed conditions");
        System.out.println("PASS: Fusion ACE IDs, Win32 property layout, configuration and pointer-parameter consumption");
    }
}
