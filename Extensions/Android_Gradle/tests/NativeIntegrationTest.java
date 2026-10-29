import _DeLib.*;
import _DeLib.Coord.CoordType;
import _DeLib.FindTheWayClass.MapType;
import java.util.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;

/** Uses the real JNI library and the real shared core; no Java algorithm substitutes. */
public final class NativeIntegrationTest {
    private static int checks;
    private static void check(boolean value, String message) {
        checks++;
        if (!value) { throw new AssertionError(message); }
    }
    private static void fails(Runnable fn, String message) {
        boolean failed = false;
        try { fn.run(); } catch (IllegalArgumentException | IllegalStateException e) { failed = true; }
        check(failed, message);
    }
    private static Vector<Coord> set(Coord... values) { return new Vector<>(Arrays.asList(values)); }
    private static void find(FindTheWayClass map, boolean diagonal, boolean force) {
        if (force) { map.ForceFind(new Coord(0,0), new Coord(3,3), diagonal, true, set(), set(), set(), 9); }
        else { map.Find(new Coord(0,0), new Coord(3,3), diagonal, true, set(), set(), set(), 9); }
    }
    private static String path(FindTheWayClass map, String name) {
        StringBuilder value = new StringBuilder();
        for (int i=0; i<map.GetDistance(name); i++) {
            value.append(map.GetCoordAtPath(i, CoordType.X, name)).append(',')
                 .append(map.GetCoordAtPath(i, CoordType.Y, name)).append(';');
        }
        return value.toString();
    }
    private static String coords(Vector<Coord> values) {
        StringBuilder result = new StringBuilder();
        for (Coord c : values) { result.append(c.x).append(',').append(c.y).append(';'); }
        return result.toString();
    }
    private static String area(FindTheWayClass map) {
        StringBuilder result = new StringBuilder();
        for (Vector<Coord> ring : map.GetArea()) { result.append('[').append(coords(ring)).append(']'); }
        return result.toString();
    }
    public static void main(String[] args) throws Exception {
        Map<String,String> reference = new HashMap<>();
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.UTF_8)) {
            int eq = line.indexOf('=');
            if (eq >= 0) { reference.put(line.substring(0,eq), line.substring(eq+1)); }
        }
        fails(() -> new FindTheWayClass(0, 2), "zero size rejected");
        fails(() -> new FindTheWayClass(-1, 2), "negative size rejected");
        fails(() -> new FindTheWayClass(Integer.MAX_VALUE, 2), "overflow rejected");
        fails(() -> new FindTheWayClass("2:2:AA==:AA=="), "short maps rejected");
        fails(() -> new FindTheWayClass("garbage"), "malformed maps rejected");
        for (String invalid : new String[] {"1:1:AA==junk:AA==", "1:1:A===:AA==", "1:1:AB==:AA==",
                "1:1:AA==:AA==:", "-1:1:AA==:AA==", "1:1:éé==:AA=="}) {
            fails(() -> new FindTheWayClass(invalid), "strict serialized input validation");
        }
        try (FindTheWayClass cells = new FindTheWayClass(3,1)) {
            cells.SetMapData(1,0,new FindTheWayClass.MapData(255,0xffffffffL,false),MapType.TERRAIN,true);
            check(!cells.CoordObstacle(1,0,MapType.MAP), "255 cost independently passable");
            check(cells.GetPointCost(new Coord(1,0))==255, "actual cost survives legacy projection");
            check(cells.GetMap(1,0,MapType.MAP,true)==254, "numeric view reserves 255 for obstacles");
            check(cells.GetMapData(1,0,MapType.MAP,true).terrainType==0xffffffffL, "unsigned terrain ID");
            cells.Find(new Coord(0,0),new Coord(2,0),false,true,set(),set(),set(),9);
            check(cells.GetPathValidity(null), "path uses explicit obstacle state");
            String encoded = cells.GetMap();
            check(encoded.startsWith("FTW2:"), "rich format is versioned");
            try (FindTheWayClass restored = new FindTheWayClass(encoded)) {
                check(restored.GetMapData(1,0,MapType.TERRAIN,true).terrainType==0xffffffffL, "terrain ID round trip");
                check(restored.GetMap().equals(encoded), "rich canonical round trip");
            }
            cells.SetMapData(1,0,new FindTheWayClass.MapData(0,7,true),MapType.TERRAIN,true);
            check(cells.CoordObstacle(1,0,MapType.MAP), "zero-cost obstacle");
            cells.Find(new Coord(0,0),new Coord(2,0),false,true,set(),set(),set(),9);
            check(!cells.GetPathValidity(null), "zero-cost obstacle blocks path");
            fails(() -> new FindTheWayClass.MapData(256,0,false), "invalid cost rejected");
            fails(() -> new FindTheWayClass.MapData(0,-1,false), "invalid terrain ID rejected");
        }
        try (FindTheWayClass map = new FindTheWayClass(4,4)) {
            check(map.GetMapWidth()==4 && map.GetMapHeight()==4, "dimensions");
            map.SetStash(true);
            find(map,false,false);
            check(map.GetDistance(null)==7, "cardinal distance includes both endpoints");
            check(path(map,null).equals(reference.get("path")), "JNI/direct C++ path equality");
            String unicode = "路线\uD83D\uDE80\u0000终点";
            map.SaveLastPath(unicode);
            String saved = path(map,unicode);
            find(map,true,false);
            check(map.GetDistance(null)==4, "cache distinguishes diagonal mode");
            check(path(map,unicode).equals(saved), "saved paths own their data and preserve UTF-16");
            check(map.GetCoordAtPath(-1,CoordType.X,null)==65536, "negative step sentinel");
            check(map.GetCoordAtPath(100,CoordType.X,null)==65536, "large step sentinel");
            check(map.GetDistance("missing")==65536, "unknown path sentinel");
            check(map.GetPathValidity(""), "empty name selects last path");
            int[] callbacks={0};
            map.SetUpdateMapCallBack(owner -> callbacks[0]++, null);
            map.SetMap(0,0,255,MapType.TERRAIN,true);
            find(map,false,false);
            check(callbacks[0]==1 && !map.GetPathValidity(null), "lazy map update and event");
            find(map,false,true);
            check(map.GetDistance(null)==7, "forced endpoint search");
            check(map.GetMap(0,0,MapType.MAP,true)==255, "force restores endpoint costs");
            find(map,false,false);
            check(!map.GetPathValidity(null), "force does not contaminate ordinary cache");
            map.ClearMap(MapType.TERRAIN,0,true);
            map.SetMap(1,1,128,MapType.TERRAIN,true);
            map.SetMap(1,1,127,MapType.DYNAMIC,true);
            String encoded=map.GetMap();
            try (FindTheWayClass restored=new FindTheWayClass(encoded)) {
                check(restored.GetMap(1,1,MapType.TERRAIN,true)==128, "unsigned terrain serialization");
                check(restored.GetMap(1,1,MapType.DYNAMIC,true)==127, "unsigned dynamic serialization");
                check(restored.GetMap(1,1,MapType.MAP,true)==254, "legacy numeric view preserves obstacle sentinel");
                check(restored.GetMap().equals(encoded), "canonical Base64 round trip");
            }
            fails(() -> map.SetMap("4:4:AA==:AA=="), "invalid replacement rejected");
            check(map.GetMap().equals(encoded), "failed import preserves map");
            map.ClearMap(MapType.TERRAIN,0,true);
            map.ClearMap(MapType.DYNAMIC,0,true);
            Coord start=new Coord(1,1);
            check(map.GetAbleLineRange(start,10,0,set(),set(),set(),9,true)==2, "line range at map edge");
            check(start.isEqual(new Coord(1,1)), "JNI does not mutate caller coordinates");
            map.CalcLineArea(start,2,0,set(),set(),set(),9,true);
            check(area(map).equals(reference.get("line")), "JNI/direct C++ line area equality");
            map.CalcArea(start,2,0,set(),set(),set(),9,false,false,0,null,false);
            check(area(map).equals(reference.get("area")), "JNI/direct C++ area equality");
            map.CalcArea(start,0,0,set(),set(),set(),9,false,true,1,null,false);
            check(!map.GetArea().isEmpty(), "zero move range still permits attack area");
            Vector<Coord> zoc=set();
            map.GenerateZoc(start,zoc,false,false);
            check(zoc.size()==4, "cardinal ZOC");
            check(coords(zoc).equals(reference.get("zoc")), "JNI/direct C++ ZOC equality");
            map.GenerateZoc(set(start),zoc,false,false);
            check(zoc.size()==4, "multi-source ZOC respects diagonal flag");
            map.GenerateZoc(start,zoc,false,true);
            check(zoc.size()==8, "diagonal ZOC");
            map.GenerateZoc(set(),zoc,false,false);
            check(zoc.isEmpty(), "empty source ZOC clears target");
            Vector<Coord> one=set(); map.GenerateSet(start,one,false,false);
            check(one.size()==1 && one.get(0).isEqual(start), "single coordinate generation");
            Vector<Coord> snapshot=map.GetMapCoordinates(false);
            check(snapshot.size()==16 && snapshot.get(15).isEqual(new Coord(3,3)), "row-major map snapshot");
            map.SetIsometric(true);
            map.SetGridSize(FindTheWayClass.GetIsometricGridSize(64,32),0,0);
            check(map.GetGridWidth()==64 && map.GetGridHeight()==32, "new C++ grid packing");
            for(int y=0;y<4;y++) for(int x=0;x<4;x++) {
                Coord c=new Coord(x,y);
                check(map.GetGridCoord(map.GetRealCoord(c)).isEqual(c), "isometric center round trip");
            }
            check(coords(map.GetMapCoordinates(true)).equals(reference.get("iso")), "JNI/direct C++ isometric equality");
            map.SetIsometric(false); map.SetGridSize(16,4,6);
            check(map.GetRealCoord(new Coord(2,3)).isEqual(new Coord(44,62)), "traditional grid offsets");
        }
        try (FindTheWayClass map=new FindTheWayClass(2,2)) {
            map.SetUpdateMapCallBack(owner -> { throw new IllegalStateException("callback-test"); },null);
            fails(map::UpdateMap, "callback exception propagates through JNI");
            map.SetUpdateMapCallBack(owner -> map.close(),null);
            map.UpdateMap(); // native shared ownership pins map through reentrant destruction
            fails(map::GetMap, "reentrant close safely releases native handle");
            map.close();
        }
        try (FindTheWayClass map=new FindTheWayClass(2,2)) {
            int[] count={0};
            map.SetUpdateMapCallBack(owner -> { count[0]++; check(map.GetMapWidth()==2,"reentrant query"); },null);
            map.UpdateMap(); map.UpdateMap();
            check(count[0]==2,"nested JNI calls retain callback safety");
        }
        AOEClass aoe=new AOEClass();
        for(int type:new int[]{11,12,21,22,23,24,25,26,31,32,41}) for(int dir=0;dir<4;dir++) {
            Vector<Coord> values=set(); aoe.GetAOE(new Coord(4,4),dir,type,values);
            check(coords(values).equals(reference.get("aoe"+type+"_"+dir)),"JNI/direct C++ AOE equality");
        }
        for(int i=0;i<1000;i++) { try(FindTheWayClass map=new FindTheWayClass(2,2)) { map.UpdateMap(); } }
        System.out.println("PASS: "+checks+" JNI checks; 1000 create/close cycles");
    }
}
