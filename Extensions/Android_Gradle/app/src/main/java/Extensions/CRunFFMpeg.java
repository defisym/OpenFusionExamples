package Extensions;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import OpenGL.GLRenderer;
import RunLoop.CCreateObjectInfo;
import Runtime.MMFRuntime;
import Services.CBinaryFile;
import _DeLib.FFMpegPlayer;
import _DeLib.MediaSource;
import _DeLib.FFMpegStorage;
import _DeLib.FFMpegTexture;

/** Android Fusion runtime adapter. ACE numbers match FFMpeg/Main.cpp. */
public final class CRunFFMpeg extends CRunExtension {
    private FFMpegPlayer player;
    private MediaSource source;
    private FFMpegTexture texture = new FFMpegTexture();
    private String name = "", key = "", videoCodec = "", audioCodec = "";
    private int width, height, threads, volume = 100;
    private boolean stretch, autoPlay, loop, accurate, cache, wantedHardware, copyToTexture, mute;
    private boolean playing, finished, hidden, suspended;
    private float tempo = 1;
    private final CValue result = new CValue(0);
    @Override public int getNumberOfConditions() { return 8; }
    @Override public boolean createRunObject(CBinaryFile file, CCreateObjectInfo cob, int version) {
        width = file.readUnsignedShort(); height = file.readUnsignedShort();
        file.readByte(); // Windows HWA surface preference; Android always uses the runtime renderer.
        stretch = file.readByte() != 0; autoPlay = file.readByte() != 0; loop = file.readByte() != 0;
        threads = Math.max(0, Math.min(16, file.readInt())); file.readInt();
        accurate = file.readByte() != 0; cache = file.readByte() != 0; file.skipBytes(2);
        wantedHardware = file.readInt() != 0; mute = file.readByte() != 0;
        copyToTexture = file.readByte() != 0; file.readByte();
        wantedHardware |= copyToTexture;
        ho.hoImgWidth = width; ho.hoImgHeight = height; return true;
    }
    private void release() {
        if (player != null) { player.close(); player = null; }
        texture.close();
        if (source != null) { source.close(); source = null; }
    }
    private void failure(Exception ex) { android.util.Log.e("OFE.FFMpeg", ex.getMessage(), ex); }
    private void open(String path, String password, long position) {
        release(); name = path; key = password; finished = hidden = false;
        try {
            source = FFMpegStorage.get(rh, MMFRuntime.inst).files.open(path, password, cache);
            player = new FFMpegPlayer(MMFRuntime.inst, source.path, wantedHardware ? texture.decoderSurface() : null,
                                      threads, videoCodec, audioCodec, mute);
            player.volume(volume); player.tempo(tempo);
            if (position > 0) player.seek(position, 1, accurate);
            else player.tick(true);
            updateTexture();
            playing = autoPlay; player.play(playing); player.suspend(suspended);
        } catch (Exception ex) {
            release(); playing = false; failure(ex); ho.generateEvent(5, 0);
        }
    }
    private void updateTexture() {
        if (player == null) return;
        long[] info = player.info();
        int w = (int)info[0], h = (int)info[1];
        if (info[6] != 0) { if (info[4] != 0) texture.copyHardware(w, h); else texture.copySoftware(player, w, h); }
        ho.hoImgWidth = stretch ? width : w; ho.hoImgHeight = stretch ? height : h;
        ho.hoImgXSpot = ho.hoImgYSpot = 0;
    }
    @Override public int handleRunObject() {
        if (player == null) return 0;
        try {
            if (!texture.contextValid()) { reinitDisplay(); return REFLAG_DISPLAY; }
            int state = player.tick(false);
            if (state == 1) { hidden = false; updateTexture(); }
            else if (player.info()[4] != 0) updateTexture(); // SurfaceTexture callbacks can arrive after the decode tick.
            if (state == 2 && !finished) {
                finished = true;
                if (loop) { player.seek(0, 1, accurate); finished = false; updateTexture(); }
                else { playing = false; player.play(false); }
                ho.generateEvent(4, 0); // All state committed before the reentrant Fusion event.
            }
            return REFLAG_DISPLAY;
        } catch (Exception ex) { failure(ex); release(); playing = false; return REFLAG_DISPLAY; }
    }
    @Override public void displayRunObject() {
        if (player == null || hidden || texture.texture() < 0) return;
        float sx = stretch ? width / (float)texture.getWidth() : ho.roc.rcScaleX;
        float sy = stretch ? height / (float)texture.getHeight() : ho.roc.rcScaleY;
        GLRenderer.inst.renderScaledRotatedImage(texture, true, ho.roc.rcAngle, sx, sy, 0, 0,
            ho.hoX - rh.rhWindowX, ho.hoY - rh.rhWindowY, texture.getWidth(), texture.getHeight(),
            ho.ros.rsEffectShader, ho.ros.rsEffect, ho.ros.rsEffectParam);
    }
    @Override public void reinitDisplay() {
        if (player == null) { texture.close(); return; }
        long position = player.info()[3]; boolean resume = playing;
        open(name, key, position);
        if (player != null) { playing = resume; player.play(resume); }
    }
    @Override public void pauseRunObject() { suspended = true; if (player != null) player.suspend(true); }
    @Override public void continueRunObject() { suspended = false; if (player != null) player.suspend(false); }
    @Override public void destroyRunObject(boolean fast) { release(); }
    @Override public boolean condition(int id, CCndExtension cnd) {
        switch (id) {
            case 0: return player != null;
            case 1: return player != null && playing && !finished;
            case 2: return loop;
            case 3: return finished;
            case 4: case 5: return name.equals(cnd.getParamExpString(rh, 0));
            case 6: return player != null && player.info()[4] != 0;
            case 7: return wantedHardware;
            default: return false;
        }
    }
    private void seek(long ms, int flags) {
        if (player != null) { player.seek(Math.max(0, ms), flags, accurate); finished = hidden = false;
            if ((flags & (1 << 16)) == 0) updateTexture(); }
    }
    @Override public void action(int id, CActExtension act) {
        try {
            switch (id) {
                case 0: open(act.getParamExpString(rh, 0), act.getParamExpString(rh, 1), 0); break;
                case 1: release(); playing = finished = false; break;
                case 2: playing = true; hidden = false; if (player != null) player.play(true); break;
                case 3: playing = false; if (player != null) player.play(false); break;
                case 4: volume = Math.max(0, Math.min(100, act.getParamExpression(rh, 0))); if (player != null) player.volume(volume); break;
                case 5: loop = act.getParamExpression(rh, 0) != 0; break;
                case 6: seek(act.getParamExpression(rh, 0), 1); break;
                case 7: seek(act.getParamExpression(rh, 0), act.getParamExpression(rh, 1)); break;
                case 8: threads = Math.max(0, Math.min(16, act.getParamExpression(rh, 0))); break;
                case 9: accurate = act.getParamExpression(rh, 0) != 0; break;
                case 10: {
                    String path = act.getParamExpString(rh, 0), password = act.getParamExpString(rh, 1);
                    if (cache) FFMpegStorage.get(rh, MMFRuntime.inst).files.cache(path, password); break;
                }
                case 11: { String path = act.getParamExpString(rh, 0); if (cache) FFMpegStorage.get(rh, MMFRuntime.inst).files.erase(path); break; }
                case 12: {
                    String path = act.getParamExpString(rh, 0), password = act.getParamExpString(rh, 1);
                    open(path, password, Math.max(0, act.getParamExpression(rh, 2))); break;
                }
                case 13: {
                    String device = act.getParamExpString(rh, 0);
                    wantedHardware = "mediacodec".equalsIgnoreCase(device); copyToTexture &= wantedHardware; break;
                }
                case 14: width = Math.max(0, act.getParamExpression(rh, 0)); height = Math.max(0, act.getParamExpression(rh, 1)); stretch = true;
                    ho.hoImgWidth = width; ho.hoImgHeight = height; break;
                case 15: {
                    float value = (float)act.getParamExpDouble(rh, 0);
                    if ((Float.isNaN(value) || Float.isInfinite(value)) || value <= 0 || value > 32) throw new IllegalArgumentException("Invalid playback tempo");
                    if (player != null) player.tempo(value); tempo = value; break;
                }
                case 16: mute = act.getParamExpression(rh, 0) != 0; break;
                case 17: videoCodec = act.getParamExpString(rh, 0); audioCodec = act.getParamExpString(rh, 1); break;
                case 18: if (!playing) hidden = true; break;
                case 19: copyToTexture = act.getParamExpression(rh, 0) != 0; if (copyToTexture) wantedHardware = true; break;
                default: break;
            }
        } catch (Exception ex) { failure(ex); }
    }
    @Override public CValue expression(int id) {
        switch (id) {
            case 0: result.forceString(name); break;
            case 1: result.forceInt(player == null ? -1 : (int)player.info()[3]); break;
            case 2: result.forceInt(player == null ? -1 : (int)player.info()[2]); break;
            case 3: result.forceInt(volume); break;
            // Desktop cSurface* values have no Android Runtime counterpart. Consume parameters.
            case 4: ho.getExpParam().getInt(); result.forceInt(0); break;
            case 5: ho.getExpParam().getInt(); ho.getExpParam().getInt(); result.forceInt(0); break;
            case 6: result.forceInt(player != null ? 1 : 0); break;
            case 7: result.forceInt(player != null && playing && !finished ? 1 : 0); break;
            case 8: result.forceInt(loop ? 1 : 0); break;
            case 9: result.forceInt(finished ? 1 : 0); break;
            case 10: result.forceInt(player != null && player.info()[4] != 0 ? 1 : 0); break;
            case 11: result.forceString("mediacodec"); break;
            case 12: result.forceString(wantedHardware ? "mediacodec" : "none"); break;
            case 13: result.forceDouble(tempo); break;
            case 14: result.forceString(videoCodec); break;
            case 15: result.forceString(audioCodec); break;
            case 16: result.forceInt(threads); break;
            default: result.forceInt(0); break;
        }
        return result;
    }
}
