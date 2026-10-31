package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import RunLoop.CCreateObjectInfo;
import Runtime.MMFRuntime;
import Services.CBinaryFile;
import _DeLib.SDLMixer;
import _DeLib.SDLMixerStorage;

/** Fusion Android binding for the existing 7/17/14 Windows ACE table. */
public final class CRunSDL_Mixer extends CRunExtension {
    private SDLMixer mixer;
    private SDLMixerStorage.Binding binding;
    private final CValue result = new CValue(0);
    @Override public int getNumberOfConditions() { return 7; }
    private void failure(Throwable error) { android.util.Log.e("OFE.SDL_Mixer", error.getMessage(), error); }
    @Override public boolean createRunObject(CBinaryFile file, CCreateObjectInfo cob, int version) {
        try { binding = SDLMixerStorage.attach(rh, MMFRuntime.inst); mixer = binding.mixer(); return true; }
        catch (RuntimeException | LinkageError error) { failure(error); return false; }
    }
    @Override public void destroyRunObject(boolean fast) { if (binding != null) { binding.close(); binding = null; mixer = null; } }
    @Override public int handleRunObject() { if (mixer != null) mixer.update(); return 0; }
    @Override public void pauseRunObject() { if (binding != null) binding.suspend(true); }
    @Override public void continueRunObject() { if (binding != null) binding.suspend(false); }
    @Override public boolean condition(int id, CCndExtension cnd) {
        if (mixer == null) return false;
        try {
            int channel = 0; boolean exclusive = true; String name = "";
            switch (id) {
                case 0: case 3: case 5: channel = cnd.getParamExpression(rh, 0); exclusive = cnd.getParamExpression(rh, 1) != 0; break;
                case 2: channel = cnd.getParamExpression(rh, 0); break;
                case 6: name = cnd.getParamExpString(rh, 0); break;
                default: break;
            }
            return mixer.condition(id, channel, exclusive, name);
        } catch (RuntimeException error) { failure(error); return false; }
    }
    @Override public void action(int id, CActExtension act) {
        if (mixer == null) return;
        try {
            if (id == 0 || id == 1) {
                String name = act.getParamExpString(rh, 0), key = act.getParamExpString(rh, 1);
                mixer.play(name, key, act.getParamExpression(rh, 2), id == 0, act.getParamExpression(rh, 3), act.getParamExpression(rh, 4)); return;
            }
            if (id == 10 || id == 12) { mixer.cache(act.getParamExpString(rh, 0), act.getParamExpString(rh, 1), id == 12); return; }
            if (id == 11) { mixer.erase(act.getParamExpString(rh, 0)); return; }
            int channel = 0; boolean exclusive = true; double a = 0, b = 0; String text = "";
            switch (id) {
                case 2: case 3: channel = act.getParamExpression(rh, 0); a = act.getParamExpression(rh, 1); exclusive = act.getParamExpression(rh, 2) != 0; break;
                case 4: case 5: channel = act.getParamExpression(rh, 0); exclusive = act.getParamExpression(rh, 1) != 0; break;
                case 6: channel = act.getParamExpression(rh, 0); a = act.getParamExpDouble(rh, 1); break;
                case 7: case 13: a = act.getParamExpression(rh, 0); break;
                case 8: channel = act.getParamExpression(rh, 0); a = act.getParamExpDouble(rh, 1); b = act.getParamExpDouble(rh, 2); break;
                case 9: channel = act.getParamExpression(rh, 0); a = act.getParamExpression(rh, 1); text = act.getParamExpString(rh, 2); b = act.getParamExpression(rh, 3); exclusive = false; break;
                case 14: case 15: channel = act.getParamExpression(rh, 0); a = act.getParamExpression(rh, 1); b = act.getParamExpression(rh, 2); exclusive = act.getParamExpression(rh, 3) != 0; break;
                case 16: channel = act.getParamExpression(rh, 0); a = act.getParamExpression(rh, 1); exclusive = act.getParamExpression(rh, 2) != 0; break;
                default: return;
            }
            mixer.action(id, channel, exclusive, a, b, text);
        } catch (Exception error) { failure(error); }
    }
    @Override public CValue expression(int id) {
        try {
            int channel = 0; boolean exclusive = true; String name = "";
            switch (id) {
                case 0: case 1: case 10: case 11: case 12: case 13: channel = ho.getExpParam().getInt(); exclusive = ho.getExpParam().getInt() != 0; break;
                case 2: case 3: case 5: channel = ho.getExpParam().getInt(); break;
                case 4: name = ho.getExpParam().getString(); exclusive = ho.getExpParam().getInt() != 0; break;
                // Native desktop address expressions have no safe Android int-pointer ABI.
                case 6: ho.getExpParam().getString(); ho.getExpParam().getInt(); ho.getExpParam().getInt(); result.forceString(""); return result;
                case 7: {
                    String access = ho.getExpParam().getString(), binary = ho.getExpParam().getString();
                    int offset = ho.getExpParam().getInt(), size = ho.getExpParam().getInt();
                    result.forceString(mixer == null ? "" : mixer.memoryName(access, binary, offset, size)); return result;
                }
                case 8: ho.getExpParam().getString(); ho.getExpParam().getInt(); result.forceInt(0); return result;
                default: break;
            }
            if (id == 5) { result.forceString(mixer == null ? "" : mixer.channelName(channel)); return result; }
            double value = mixer == null ? 0 : mixer.number(id, channel, exclusive, name);
            if (id == 2 || id == 3) result.forceDouble(value); else result.forceInt((int)value);
        } catch (RuntimeException error) { failure(error); result.forceInt(0); }
        return result;
    }
}
