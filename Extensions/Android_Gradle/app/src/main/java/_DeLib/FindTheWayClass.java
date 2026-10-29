package _DeLib;

import java.util.Vector;
import _DeLib.Coord.CoordType;

/** Fusion marshalling only. All map, path, area, ZOC and AOE work uses the shared C++ core. */
public final class FindTheWayClass implements AutoCloseable {
    static { System.loadLibrary("findtheway"); }

    public enum MapType { MAP, TERRAIN, DYNAMIC }
    /** Immutable snapshot; terrain IDs are unsigned 32-bit application-defined categories. */
    public static final class MapData {
        public final int cost;
        public final long terrainType;
        public final boolean obstacle;
        public MapData(int cost, long terrainType, boolean obstacle) {
            if (cost < 0 || cost > 255 || terrainType < 0 || terrainType > 0xffffffffL) {
                throw new IllegalArgumentException("Invalid map cell");
            }
            this.cost = cost;
            this.terrainType = terrainType;
            this.obstacle = obstacle;
        }
    }
    public interface UpdateMapCallBack { void CallBack(Object owner); }
    public interface UpdateCallBack<T> { void CallBack(T value); }

    public static final int MAPTYPENUM = 3;
    public static final int MAP_PATH = 0, MAP_OBSTACLE = 255;
    public static final int STEP_UNREACHABLE = 65536, COORD_INVALID = 65536;
    public static final int REAL_INVALID = Integer.MAX_VALUE;
    public static final int DEFAULT_IGNOREFLAG = 0b01001, REPEL_IGNOREFLAG = 0b11100;
    public static final int DEFAULT_EXTRARANGESTARTPOS = 65536;
    public static final boolean ATTACK = true, MOVE = false, REPEL = false;

    private long handle;
    private UpdateMapCallBack callback;
    private Object callbackOwner;

    private static native long nCreate(int width, int height, String encoded);
    private static native void nDestroy(long handle);
    private native int[] nCall(long handle, int operation, int[] args, String name,
            int[] ally, int[] enemy, int[] zoc);
    private native String nText(long handle, int operation, int[] args, String text,
            int[] ally, int[] enemy, int[] zoc);
    private static native int[] nGeometry(int operation, int[] args, int[] coords);

    public FindTheWayClass(int width, int height) { handle = nCreate(width, height, null); }
    public FindTheWayClass(String encoded) {
        if (encoded == null) { throw new IllegalArgumentException("Missing map data"); }
        handle = nCreate(0, 0, encoded);
    }

    @Override public synchronized void close() {
        if (handle != 0) {
            nDestroy(handle);
            handle = 0;
        }
        callback = null;
        callbackOwner = null;
    }

    private long liveHandle() {
        if (handle == 0) { throw new IllegalStateException("FindTheWay map is closed"); }
        return handle;
    }

    // Invoked synchronously by JNI on the Fusion event thread. Do not rename during minification.
    private void onMapUpdated() {
        UpdateMapCallBack current = callback;
        if (current != null) { current.CallBack(callbackOwner); }
    }

    private int[] call(int op, String name, Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc, int... args) {
        return nCall(liveHandle(), op, args, name, pack(ally), pack(enemy), pack(zoc));
    }
    private int[] call(int op, int... args) { return call(op, null, null, null, null, args); }
    private String text(int op, String value, int... args) {
        return nText(liveHandle(), op, args, value, null, null, null);
    }
    private static int flag(boolean value) { return value ? 1 : 0; }
    private static int[] pack(Vector<Coord> coords) {
        if (coords == null) { return new int[0]; }
        int[] result = new int[coords.size() * 2];
        for (int i = 0; i < coords.size(); i++) {
            result[2 * i] = coords.get(i).x;
            result[2 * i + 1] = coords.get(i).y;
        }
        return result;
    }
    private static Coord coord(int[] values) { return new Coord(values[0], values[1], true); }
    private static void unpack(int[] values, Vector<Coord> output) {
        output.clear();
        for (int i = 0; i < values.length; i += 2) {
            output.add(new Coord(values[i], values[i + 1], true));
        }
    }

    public static int Range(int value) { return Math.max(MAP_PATH, Math.min(MAP_OBSTACLE, value)); }
    public static int GetIgnoreFlag(boolean zoc, boolean ally, boolean enemy, boolean attackAlly,
            boolean attackEnemy) {
        return flag(zoc) << 4 | flag(ally) << 3 | flag(enemy) << 2 | flag(attackAlly) << 1 | flag(attackEnemy);
    }
    public static int GetIsometricGridSize(int width, int height) {
        return nGeometry(0, new int[] {width, height}, null)[0];
    }
    public static Coord CalcMapSize(int width, int height, int size, boolean isometric) {
        return coord(nGeometry(1, new int[] {width, height, size, flag(isometric)}, null));
    }

    public synchronized void SetIsometric(boolean value) { call(0, flag(value)); }
    public synchronized void SetGridSize(int size, int x, int y) { call(1, size, x, y); }
    public synchronized Coord GetGridCoord(Coord value) { return coord(call(2, 0, value.x, value.y)); }
    public synchronized Coord GetRealCoord(Coord value) { return coord(call(2, 1, value.x, value.y)); }
    public synchronized Coord GetTraditionalGridCoord(Coord value) { return coord(call(2, 2, value.x, value.y)); }
    public synchronized Coord GetTraditionalRealCoord(Coord value) { return coord(call(2, 3, value.x, value.y)); }
    public synchronized Coord GetIsometricGridCoord(Coord value) { return coord(call(2, 4, value.x, value.y)); }
    public synchronized Coord GetIsometricRealCoord(Coord value) { return coord(call(2, 5, value.x, value.y)); }
    public synchronized int GetMapWidth() { return call(12)[0]; }
    public synchronized int GetMapHeight() { return call(12)[1]; }
    public synchronized int GetGridWidth() { return call(12)[2]; }
    public synchronized int GetGridHeight() { return call(12)[3]; }
    public synchronized int GetGridOffsetX() { return call(12)[4]; }
    public synchronized int GetGridOffsetY() { return call(12)[5]; }
    public synchronized void SetMap(int x, int y, int cost, MapType type, boolean update) {
        if (type == MapType.MAP) { return; }
        call(3, x, y, Range(cost), type.ordinal(), flag(update));
    }
    public synchronized void ClearMap(MapType type, int cost, boolean update) {
        if (type == MapType.MAP) { return; }
        call(4, type.ordinal(), Range(cost), flag(update));
    }
    public synchronized int GetMap(int x, int y, MapType type, boolean update) {
        return call(5, x, y, type.ordinal(), flag(update))[0];
    }
    public synchronized MapData GetMapData(int x, int y, MapType type, boolean update) {
        int[] value = call(19, x, y, type.ordinal(), flag(update));
        return new MapData(value[0], Integer.toUnsignedLong(value[1]), value[2] != 0);
    }
    public synchronized void SetMapData(int x, int y, MapData value, MapType type, boolean update) {
        if (type == MapType.MAP) { return; }
        call(20, x, y, value.cost, (int)value.terrainType, flag(value.obstacle), type.ordinal(), flag(update));
    }
    public synchronized void SetMap(String encoded) { text(3, encoded); }
    public synchronized String GetMap() { return text(0, null); }
    public synchronized void UpdateMap() { call(6); }
    public synchronized void SetUpdateMapCallBack(UpdateMapCallBack value, Object owner) {
        callback = value;
        callbackOwner = owner;
    }
    public synchronized boolean CoordObstacle(int x, int y, MapType type) {
        return GetMapData(x, y, type, true).obstacle;
    }
    public synchronized boolean PointObstacle(Coord value) { return CoordObstacle(value.x, value.y, MapType.MAP); }
    public synchronized int GetPointCost(Coord value) { return GetMapData(value.x, value.y, MapType.MAP, true).cost; }
    public synchronized void Find(Coord start, Coord destination, boolean diagonal, boolean corner,
            Vector<Coord> ally, Vector<Coord> enemy, Vector<Coord> zoc, int ignore) {
        find(start, destination, diagonal, corner, ally, enemy, zoc, ignore, false);
    }
    public synchronized void ForceFind(Coord start, Coord destination, boolean diagonal, boolean corner,
            Vector<Coord> ally, Vector<Coord> enemy, Vector<Coord> zoc, int ignore) {
        find(start, destination, diagonal, corner, ally, enemy, zoc, ignore, true);
    }
    private void find(Coord start, Coord dest, boolean diagonal, boolean corner, Vector<Coord> ally,
            Vector<Coord> enemy, Vector<Coord> zoc, int ignore, boolean force) {
        call(7, null, ally, enemy, zoc, start.x, start.y, dest.x, dest.y,
                flag(diagonal), flag(corner), ignore, flag(force));
    }
    public synchronized boolean GetPathValidity(String name) { return call(8, name, null, null, null)[0] != 0; }
    public synchronized int GetDistance(String name) { return call(9, name, null, null, null)[0]; }
    public synchronized int GetCoordAtPath(int step, CoordType type, String name) {
        if (step < 0) { return COORD_INVALID; }
        return call(10, name, null, null, null, step, type.ordinal())[0];
    }
    public synchronized void SaveLastPath(String name) { call(11, name, null, null, null); }
    public synchronized String OutPutMapStr(MapType type, boolean showPath, String name) {
        return text(1, name, type.ordinal(), flag(showPath));
    }
    public synchronized int GetAbleLineRange(Coord start, int range, int dir, Vector<Coord> ally,
            Vector<Coord> enemy, Vector<Coord> zoc, int ignore, boolean attack) {
        return call(13, null, ally, enemy, zoc, start.x, start.y, Math.max(0, range), dir, ignore, flag(attack))[0];
    }
    public synchronized void CalcLineArea(Coord start, int range, int startRange, Vector<Coord> ally,
            Vector<Coord> enemy, Vector<Coord> zoc, int ignore, boolean attack) {
        call(14, null, ally, enemy, zoc, start.x, start.y, Math.max(0, range), Math.max(0, startRange), ignore, flag(attack));
    }
    public synchronized void CalcArea(Coord start, int range, int startRange, Vector<Coord> ally,
            Vector<Coord> enemy, Vector<Coord> zoc, int ignore, boolean attack, boolean allRange,
            int attackRange, UpdateCallBack<Integer> extraRange, boolean diagonal) {
        int[] result = call(15, null, ally, enemy, zoc, start.x, start.y, Math.max(0, range),
                Math.max(0, startRange), ignore, flag(attack), flag(allRange), Math.max(0, attackRange), flag(diagonal));
        if (extraRange != null) { extraRange.CallBack(result[0]); }
    }
    public synchronized Vector<Vector<Coord>> GetArea() {
        int[] data = call(16);
        Vector<Vector<Coord>> result = new Vector<>();
        for (int i = 0; i < data.length;) {
            int count = data[i++];
            Vector<Coord> ring = new Vector<>();
            for (int j = 0; j < count; j++) { ring.add(new Coord(data[i++], data[i++], true)); }
            result.add(ring);
        }
        return result;
    }
    public synchronized void GenerateSet(Coord start, Vector<Coord> output, boolean add, boolean diagonal) {
        Vector<Coord> input = new Vector<>(); input.add(start);
        unpack(nGeometry(2, new int[] {0, flag(add), flag(diagonal)}, join(input, output)), output);
    }
    public synchronized void GenerateZoc(Coord start, Vector<Coord> output, boolean add, boolean diagonal) {
        Vector<Coord> input = new Vector<>(); input.add(start);
        GenerateZoc(input, output, add, diagonal);
    }
    public synchronized void GenerateZoc(Vector<Coord> input, Vector<Coord> output, boolean add, boolean diagonal) {
        unpack(nGeometry(2, new int[] {1, flag(add), flag(diagonal)}, join(input, output)), output);
    }
    private static int[] join(Vector<Coord> input, Vector<Coord> output) {
        int[] a = pack(input), b = pack(output), result = new int[1 + a.length + b.length];
        result[0] = a.length;
        System.arraycopy(a, 0, result, 1, a.length);
        System.arraycopy(b, 0, result, 1 + a.length, b.length);
        return result;
    }
    public synchronized Vector<Coord> GetMapCoordinates(boolean real) {
        Vector<Coord> output = new Vector<>();
        unpack(call(17, flag(real)), output);
        return output;
    }

    public synchronized boolean GetStash() { return call(18, 0, 0)[0] != 0; }
    public synchronized void SetStash(boolean value) { call(18, 1, flag(value)); }
    public synchronized void ClearStash() { call(18, 2, 0); }
    public synchronized int GetPathStashSize() { return call(18, 3, 0)[0]; }
    public synchronized int GetAreaStashSize() { return call(18, 4, 0)[0]; }

    static int[] aoe(Coord start, int dir, int type) {
        return nGeometry(3, new int[] {start.x, start.y, dir, type}, null);
    }
}
