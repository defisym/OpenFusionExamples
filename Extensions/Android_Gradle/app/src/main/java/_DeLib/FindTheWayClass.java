package _DeLib;

import java.util.*;
import java.util.function.BiPredicate;
import java.util.function.Predicate;

import static java.lang.Math.*;

class Offset {
    int x;
    int y;
    int generalCost;

    Offset(int x, int y, int generalCost) {
        this.x = x;
        this.y = y;
        this.generalCost = generalCost;
    }
}

class Point {
    Coord coord;
    int parent;

    int priority;
    int cost;

    Coord getCoord() {
        return coord;
    }
}

class StashPathKey {
    Coord start;
    Coord destination;
    int ignoreFlag;
    Vector<Coord> ally;
    Vector<Coord> enemy;
    Vector<Coord> zoc;

    StashPathKey(Coord start, Coord destination, int ignoreFlag,
            Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc) {
        this.start = start;
        this.destination = destination;
        this.ignoreFlag = ignoreFlag;
        this.ally = ally;
        this.enemy = enemy;
        this.zoc = zoc;
    }
}

class StashAreaKey {
    boolean isLine;
    Coord start;
    AreaRange areaRange;
    int ignoreFlag;
    Vector<Coord> ally;
    Vector<Coord> enemy;
    Vector<Coord> zoc;

    StashAreaKey(boolean isLine, Coord start, AreaRange areaRange, int ignoreFlag,
            Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc) {
        this.isLine = isLine;
        this.start = start;
        this.areaRange = areaRange;
        this.ignoreFlag = ignoreFlag;
        this.ally = ally;
        this.enemy = enemy;
        this.zoc = zoc;
    }
}

class StashAreaValue {
    Vector<Vector<Coord>> area;
    int extraRangeStartPos;

    StashAreaValue(Vector<Vector<Coord>> area, int extraRangeStartPos) {
        this.area = area;
        this.extraRangeStartPos = extraRangeStartPos;
    }
}

class AreaRange {
    int range;
    int startRange;
    boolean attack;
    boolean allRange;
    int allRangeAttackRange = 0;

    AreaRange(int range, int startRange, boolean attack, boolean allRange, int allRangeAttackRange) {
        this.range = range;
        this.startRange = startRange;
        this.attack = attack;
        this.allRange = allRange;
        this.allRangeAttackRange = allRangeAttackRange;
    }
}

public class FindTheWayClass {
    public final static int MAPTYPENUM = 3;

    public final static Throwable INVALID_SIZE = new Throwable("Invalid size");
    public final static Throwable INVALID_DATA = new Throwable("Invalid data");
    public final static Throwable INVALID_FAILED = new Throwable("Invalid failed");

    public final static int PATH_RESERVE = 50;
    public final static int SAVEDPATH_RESERVE = 10;
    public final static int UNIT_RESERVE = 50;

    public final static int AREA_RESERVE = 50;

    public final static int MAP_PATH = 0;
    public final static int MAP_OBSTACLE = 255;

    public static int Range(int x) {
        return max(MAP_PATH, min(MAP_OBSTACLE, x));
    }

    public final static int STEP_UNREACHABLE = 65536;

    public final static int STEP_GENERALCOST_NORMAL = 10;
    public final static int STEP_GENERALCOST_DIAGONAL = 14; // 1.414

    public final static int COORD_INVALID = 65536;
    public final static int REAL_INVALID = 2147483647;

    public final static String DELIMITER = ":";

    public final static int DEFAULT_IGNOREFLAG = 0b01001;
    public final static int REPEL_IGNOREFLAG = 0b11100;
    public final static int DEFAULT_EXTRARANGESTARTPOS = 65536;

    public final static boolean ATTACK = true;
    public final static boolean MOVE = false;
    public final static boolean REPEL = false;

    private int width = 0;
    private int height = 0;

    private int mapSize = 0;

    private int gridSize = 1;
    private int gridOffsetX = 0;
    private int gridOffsetY = 0;

    private int isoGridWidth = 1;
    private int isoGridHeight = 1;

    private boolean isometric = false;
    boolean updateMap = false;

    UpdateMapCallBack updateCallBack;
    Object updateCallBackCoef;

    private int[] map;
    private int[] terrain;
    private int[] dynamic;

    Offset[] normalNeighbour = { new Offset(1, 0, STEP_GENERALCOST_NORMAL), new Offset(0, 1, STEP_GENERALCOST_NORMAL),
            new Offset(-1, 0, STEP_GENERALCOST_NORMAL), new Offset(0, -1, STEP_GENERALCOST_NORMAL) };

    Offset[] diagonalNeighbour = { new Offset(1, 0, STEP_GENERALCOST_NORMAL),
            new Offset(1, 1, STEP_GENERALCOST_DIAGONAL) // diagonal
            , new Offset(0, 1, STEP_GENERALCOST_NORMAL), new Offset(-1, 1, STEP_GENERALCOST_DIAGONAL) // diagonal
            , new Offset(-1, 0, STEP_GENERALCOST_NORMAL), new Offset(-1, -1, STEP_GENERALCOST_DIAGONAL) // diagonal
            , new Offset(0, -1, STEP_GENERALCOST_NORMAL), new Offset(1, -1, STEP_GENERALCOST_DIAGONAL) }; // diagonal

    boolean pathAvailable = false;
    Vector<Coord> path;

    Map<String, Vector<Coord>> savedPath;

    boolean stash = false;

    StashPathKey stashPathKey;
    Map<StashPathKey, Vector<Coord>> stashPath;

    Vector<Point> open_set;
    Vector<Point> close_set;
    Vector<Point> point_set;
    Vector<Coord> unit_set;

    String base64;

    Vector<Vector<Coord>> area;
    Vector<Point> cur_set;
    Vector<Point> continue_set;
    Vector<Coord> extra_set;

    int[] ranges;

    AreaRange settings;

    StashAreaKey stashAreaKey;
    Map<StashAreaKey, StashAreaValue> stashArea;

    int pOffset(int x, int y) {
        return y * width + x;
    }

    boolean PosValid(int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height && ((y * width + x) < mapSize);
    }

    int[] GetMapPointer(MapType type) {
        switch (type) {
            case MAP:
                return map;
            case TERRAIN:
                return terrain;
            case DYNAMIC:
                return dynamic;
            default:
                return null;
        }
    }

    int GetMapPosPointer(int x, int y, int[] map) {
        if (map == null || !PosValid(x, y)) {
            return -1;
        }

        // return map[pOffset(x, y)];
        return pOffset(x, y);
    }

    int GetMapPosPointer(int x, int y, MapType type) {
        return GetMapPosPointer(x, y, GetMapPointer(type));
    }

    void ClearMap(int[] map, int cost, boolean needUpdate) {
        if (map != null) {
            for (int i = 0; i < mapSize; i++) {
                map[i] = cost;
            }

            updateMap = needUpdate;
        }
    }

    void SetMap(int x, int y, int cost, int[] map, boolean needUpdate) {
        if (map != null) {
            int pos = GetMapPosPointer(x, y, map);

            if (pos >= 0) {
                map[pos] = cost;

                updateMap = needUpdate;
            }
        }
    }

    int GetMap(int x, int y, int[] map) {
        if (map != null) {
            int pos = GetMapPosPointer(x, y, map);

            if (pos >= 0) {
                return map[pos];
            } else {
                return MAP_OBSTACLE;
            }
        }

        return MAP_OBSTACLE;
    }

    Vector<Coord> GetPathPointer(String name) {
        if (savedPath != null) {
            return savedPath.get(name);
        }

        return path;
    }

    static int ZeroProtection(int value) {
        if (value < 0) {
            return 1;
        }
        return value;
    }

    void Reserve() {
        path.ensureCapacity(PATH_RESERVE);
        // savedPath.reserve(SAVEDPATH_RESERVE);

        open_set.ensureCapacity(2 * PATH_RESERVE);
        close_set.ensureCapacity(2 * PATH_RESERVE);
        point_set.ensureCapacity(8 * mapSize);
        unit_set.ensureCapacity(UNIT_RESERVE);

        area.ensureCapacity(AREA_RESERVE);
        cur_set.ensureCapacity(2 * AREA_RESERVE);
        continue_set.ensureCapacity(2 * AREA_RESERVE);
        extra_set.ensureCapacity(AREA_RESERVE);
    }

    void Allocate(int width, int height) throws Throwable {
        this.width = width;
        this.height = height;

        mapSize = width * height;

        if (mapSize <= 0) {
            throw INVALID_SIZE;
        }

        map = new int[mapSize];
        terrain = new int[mapSize];
        dynamic = new int[mapSize];

        path = new Vector<Coord>();
        savedPath = new HashMap<String, Vector<Coord>>();

        stashPath = new HashMap<StashPathKey, Vector<Coord>>();

        open_set = new Vector<>();
        close_set = new Vector<>();
        point_set = new Vector<>();
        unit_set = new Vector<Coord>();

        area = new Vector<>();
        cur_set = new Vector<>();
        continue_set = new Vector<>();
        extra_set = new Vector<>();

        stashArea = new HashMap<StashAreaKey, StashAreaValue>();

        ClearMap();
    }

    void ClearData() {
        updateMap = false;

        path.clear();
        savedPath.clear();
        pathAvailable = false;

        area.clear();

        ClearStash();
    }

    void ClearMap() {
        ClearMap(map, MAP_PATH, false);
        ClearMap(terrain, MAP_PATH, false);
        ClearMap(dynamic, MAP_PATH, false);

        ClearData();
    }

    void Free() {
        map = null;
        terrain = null;
        dynamic = null;
    }

    public FindTheWayClass(int width, int height) throws Throwable {
        Allocate(width, height);
        Reserve();
    }

    public FindTheWayClass(String base64) throws Throwable {
        SetMap(base64);
        Reserve();
    }

    // public ~FindTheWay(){
    // Free();
    // }

    public static int CalcMapWidth(int width, int gridSize, boolean isometric) {
        return isometric ? CalcIsometricMapWidth(width, gridSize) : CalcTraditionalMapWidth(width, gridSize);
    }

    public static int CalcMapHeight(int height, int gridSize, boolean isometric) {
        return isometric ? CalcIsometricMapHeight(height, gridSize) : CalcTraditionalMapHeight(height, gridSize);
    }

    // Only support n x n map
    public static int CalcIsometricMapWidth(int width, int gridSize) {
        int isoGridWidth = ZeroProtection((gridSize & 0xFFFF0000) >> 16);

        return width / isoGridWidth;
    }

    public static int CalcIsometricMapHeight(int height, int gridSize) {
        int isoGridHeight = ZeroProtection((gridSize & 0x0000FFFF));

        return height / isoGridHeight;
    }

    public static int CalcTraditionalMapWidth(int width, int gridSize) {
        gridSize = ZeroProtection(gridSize);

        return width / gridSize;
    }

    public static int CalcTraditionalMapHeight(int height, int gridSize) {
        gridSize = ZeroProtection(gridSize);

        return height / gridSize;
    }

    public void SetIsometric(boolean isometric) {
        this.isometric = isometric;
    }

    public int GetGridWidth() {
        if (!isometric) {
            return gridSize;
        } else {
            return isoGridWidth;
        }
    }

    public int GetGridHeight() {
        if (!isometric) {
            return gridSize;
        } else {
            return isoGridHeight;
        }
    }

    public int GetGridOffsetX() {
        return gridOffsetX;
    }

    public int GetGridOffsetY() {
        return gridOffsetY;
    }

    public void SetGridSize(int gridSize, int gridOffsetX, int gridOffsetY) {
        if (!isometric) {
            this.gridSize = ZeroProtection(gridSize);
        } else {
            this.isoGridWidth = ZeroProtection((gridSize & 0xFFFF0000) >> 16);
            this.isoGridHeight = ZeroProtection((gridSize & 0x0000FFFF));
        }

        this.gridOffsetX = gridOffsetX;
        this.gridOffsetY = gridOffsetY;
    }

    public Coord GetGridCoord(Coord realCoord) {
        return isometric ? GetIsometricGridCoord(realCoord) : GetTraditionalGridCoord(realCoord);
    }

    public Coord GetRealCoord(Coord gridCoord) {
        return isometric ? GetIsometricRealCoord(gridCoord) : GetTraditionalRealCoord(gridCoord);
    }

    public Coord GetTraditionalGridCoord(Coord realCoord) {
        return realCoord.sub(new Coord(gridOffsetX, gridOffsetY).div(gridSize));
    }

    public Coord GetTraditionalRealCoord(Coord gridCoord) {
        // gridCoord * gridSize + (size_t)(gridSize / 2);
        return gridCoord.mul(gridSize).add(new Coord(gridOffsetX, gridOffsetY).add(gridSize >> 1));
    }

    public static int GetIsometricGridSize(int isoGridWidth, int isoGridHeight) {
        isoGridWidth = isoGridWidth & 0xFFFF;
        isoGridHeight = isoGridHeight & 0xFFFF;

        return (isoGridWidth << 16) | isoGridHeight;
    }

    public Coord GetIsometricGridCoord(Coord realCoord) {
        // https://github.com/pvcraven/isometric_test/blob/master/Doc/index.rst
        int A = ((realCoord.x - gridOffsetX) << 1) / isoGridWidth;
        // Y from top (fusion)
        int B = ((isoGridHeight * height - realCoord.y - gridOffsetY) << 1) / isoGridHeight;
        // Y from bottom (Cartesian)
        // int B = ((realFindTheWayCoord.y - gridOffsetY) << 1) / isoGridHeight;

        int FindTheWayCoordX = ((width - 1 + A - B) >> 1);
        int FindTheWayCoordY = (((height << 1) + width - 1 - A - B) >> 1);

        return new Coord(FindTheWayCoordX, FindTheWayCoordY);
    }

    public Coord GetIsometricRealCoord(Coord gridCoord) {
        int realX = ((isoGridWidth * (height + gridCoord.x - gridCoord.y)) >> 1) + gridOffsetX;
        // Y from top (fusion)
        int realY = ((isoGridHeight * (height + 1 + gridCoord.y - width + gridCoord.x)) >> 1) + gridOffsetY;
        // Y from bottom (Cartesian)
        // int realY = ((isoGridHeight * (height - 1 - gridFindTheWayCoord.y + width -
        // gridFindTheWayCoord.x)) >> 1) + gridOffsetY;

        return new Coord(realX, realY);
    }

    public void ClearMap(MapType type, int cost, boolean needUpdate) {
        ClearMap(GetMapPointer(type), cost, needUpdate);
    }

    public void SetMap(int x, int y, int cost, MapType type, boolean needUpdate) {
        SetMap(x, y, cost, GetMapPointer(type), needUpdate);
    }

    public void SetMap(String base64) throws Throwable {
        int start = 0;
        int end = 0;

        String[] str = base64.split(DELIMITER);

        Free();
        Allocate(Integer.parseInt(str[0]), Integer.parseInt(str[1]));

        byte[] terrain = Base64.getDecoder().decode(str[2].getBytes());
        for (int i = 0; i < terrain.length; i++) {
            this.terrain[i] = terrain[i];
        }

        byte[] dynamic = Base64.getDecoder().decode(str[3].getBytes());
        for (int i = 0; i < dynamic.length; i++) {
            this.dynamic[i] = dynamic[i];
        }

        UpdateMap();
    }

    public int GetMap(int x, int y, MapType type, boolean forceUpdate) {
        if (forceUpdate && updateMap && type == MapType.MAP) {
            UpdateMap();
        }

        return GetMap(x, y, GetMapPointer(type));
    }

    public String GetMap() {
        String base64 = "";

        base64 += String.valueOf(width);
        base64 += DELIMITER;
        base64 += String.valueOf(height);

        base64 += Base64.getEncoder().encodeToString(Arrays.toString(terrain).getBytes());
        base64 += Base64.getEncoder().encodeToString(Arrays.toString(dynamic).getBytes());

        return base64;
    }

    public void UpdateMap() {
        for (int i = 0; i < mapSize; i++) {
            map[i] = Range(terrain[i] + dynamic[i]);
        }

        // Path/Area invalidation when map changes, clear
        ClearData();

        // Callback
        if (updateCallBack != null) {
            updateCallBack.CallBack(updateCallBackCoef);
        }
    }

    public void SetUpdateMapCallBack(UpdateMapCallBack f, Object p) {
        updateCallBack = f;
        updateCallBackCoef = p;
    }

    public boolean CoordObstacle(int x, int y, MapType type) {
        return GetMap(x, y, type, true) == MAP_OBSTACLE;
    }

    public boolean GetPathValidity(String name) {
        return ((name == null) && pathAvailable) || ((name != null) && savedPath.containsKey(name));
    }

    public int GetDistance(String name) {
        Vector<Coord> path = GetPathPointer(name);
        boolean curPathAvaliable = GetPathValidity(name);

        return curPathAvaliable ? path.size() : STEP_UNREACHABLE;
    }

    public int GetCoordAtPath(int pos, CoordType type, String name) {
        Vector<Coord> path = GetPathPointer(name);
        boolean curPathAvaliable = GetPathValidity(name);

        if (!curPathAvaliable || path.size() <= pos) {
            return COORD_INVALID;
        }

        return type == CoordType.X ? path.get(pos).x : path.get(pos).y;
    }

    public int GetMapWidth() {
        return width;
    }

    public int GetMapHeight() {
        return height;
    }

    // inline void OutPutMap(MapType type = MapType::MAP) {
    // auto find = [] (Path& path, Coord& coord)->bool {
    // return std::find(path.begin(), path.end(), coord) != path.end();
    // };
    //
    // cout << "MapType : " << type << endl;
    // for (size_t y = 0; y < height; y++) {
    // for (size_t x = 0; x < width; x++) {
    // Coord cur = { x,y };
    //
    // if (find(path, cur)) {
    // cout << '-';
    // }
    // else {
    // cout << (((*GetMapPosPointer(x, y, type)) != MAP_OBSTACLE) ? '#' : '*');
    // }
    // cout << ' ';
    // }
    // cout << endl;
    // }
    // }

    public String OutPutMapStr(MapType type, boolean showPath, String name) {
        if (updateMap) {
            UpdateMap();
        }

        StringBuilder output = new StringBuilder();

        output.append("MapType : ").append(type).append("\n");
        output.append("Obstacle = '*', Path = '#'" + "\n");

        Vector<Coord> pPath = null;
        boolean curPathAvaliable = false;

        if (showPath) {
            pPath = GetPathPointer(name);
            curPathAvaliable = GetPathValidity(name);

            if (curPathAvaliable) {
                String displayName = name == null ? "Last Path" : name;

                output.append("Step of \"").append(displayName).append("\" = '-'").append("\n");
            } else if (name != null) {
                output.append("Target Path \"").append(name).append("\" Invalid").append("\n");
            }
        }

        output.append("----------" + "\n");

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (showPath && curPathAvaliable && pPath.contains(new Coord(x, y))) {
                    output.append('-');
                } else {
                    output.append(((GetMap(x, y, type, true)) != MAP_OBSTACLE) ? '#' : '*');
                }
                output.append(' ');
            }

            output.append("\n");
        }

        return output.toString();
    }

    public void SaveLastPath(String name) {
        if (!pathAvailable) {
            return;
        }

        savedPath.put(name, path);
    }

    static public int GetEuclideanDistancePow(Coord start, Coord destination) {
        return (int) (pow(start.x - destination.x, 2) + pow(start.y - destination.y, 2));
    }

    static public int GetEuclideanDistance(Coord start, Coord destination) {
        return (int) sqrt(GetEuclideanDistancePow(start, destination));
    }

    static public int GetManhattanDistance(Coord start, Coord destination) {
        return abs((int) (start.x - destination.x)) + abs((int) (start.y - destination.y));
    }

    public boolean PointObstacle(Coord p) {
        return CoordObstacle(p.x, p.y, MapType.MAP);
    }

    public int GetPointCost(Coord p) {
        return GetMap(p.x, p.y, MapType.MAP, true);
    }

    public void ForceFind(Coord start, Coord destination, boolean diagonal, boolean checkDiagonalCorner,
            Vector<Coord> ally, Vector<Coord> enemy, Vector<Coord> zoc, int ignoreFlag, Heuristic h) {
        // Still try to find a way even start & destination is obstacle
        // ......By setting them to path
        if (updateMap) {
            UpdateMap();
        }

        int startCost = GetMap(start.x, start.y, this.map);
        int destinationCost = GetMap(destination.x, destination.y, this.map);

        SetMap(start.x, start.y, MAP_PATH, this.map, false);
        SetMap(destination.x, destination.y, MAP_PATH, this.map, false);

        Find(start, destination, diagonal, checkDiagonalCorner, ally, enemy, zoc, ignoreFlag, h);

        SetMap(start.x, start.y, startCost, this.map, false);
        SetMap(destination.x, destination.y, destinationCost, this.map, false);
    }

    public void Find(Coord start, Coord destination, boolean diagonal, boolean checkDiagonalCorner, Vector<Coord> ally,
            Vector<Coord> enemy, Vector<Coord> zoc, int ignoreFlag, Heuristic h) {

        // TODO
        h.Heuristic(start, destination);

    }

    public void GenerateSet(Coord start, Vector<Coord> set, boolean add, boolean diagonal) {
        if (!add) {
            set.clear();
        }
        set.add(start);
    }

    public void GenerateZoc(Coord start, Vector<Coord> zoc, boolean add, boolean diagonal) {
        if (!add) {
            zoc.clear();
        }

        Offset[] pNeighbour = diagonal ? diagonalNeighbour : normalNeighbour;

        for (Offset it : pNeighbour) {
            Coord cur = new Coord(start.x + it.x, start.y + it.y);

            if (!zoc.contains(cur)) {
                zoc.add(cur);
            }
        }
    }

    public void GenerateZoc(Vector<Coord> start, Vector<Coord> zoc, boolean add, boolean diagonal) {
        if (!add) {
            zoc.clear();
        }

        for (Coord it : start) {
            GenerateZoc(it, zoc, true, diagonal);
        }
    }

    // inline wstring OutPutAreaStr(Coord start, size_t range
    // , CoordSet* ally = nullptr, CoordSet* enemy = nullptr
    // , CoordSet* zoc = nullptr
    // , bool allRange = false, size_t extraRangeStartPos = 0) {
    // std::wstringstream ss;
    // if (!extraRangeStartPos) {
    // extraRangeStartPos = range;
    // }
    //
    // BYTE* temp = new BYTE [mapSize];
    // memcpy(temp, map, mapSize);
    //
    // auto out = [&] (BYTE cost)->wchar_t {
    // switch (cost) {
    // case MAP_OBSTACLE:
    // return L'*';
    // case 254:
    // return L'+';
    // case 125:
    // return L'□';
    // case 124:
    // return L'■';
    // case 50:
    // return L'Z';
    // case 49:
    // return L'A';
    // case 48:
    // return L'E';
    // default:
    // return L'#';
    // }
    // };
    //
    // auto updateSet = [&] (CoordSet* p, BYTE cost) {
    // if (p != nullptr) {
    // for (auto& it : *p) {
    // *GetMapPosPointer(it.x, it.y, temp) = cost;
    // }
    // }
    // };
    //
    // for (size_t it = 0; it < area.size(); it++) {
    // if (it < extraRangeStartPos) {
    // for (auto& it_C : area [it]) {
    // *GetMapPosPointer(it_C.x, it_C.y, temp) = 125;
    // }
    // }
    // else {
    // for (auto& it_C : area [it]) {
    // if (it_C == Coord { 5, 4 }) {
    // cout << "1" << endl;
    // }
    // *GetMapPosPointer(it_C.x, it_C.y, temp) = 124;
    // }
    // }
    // }
    //
    // //updateSet(zoc, 50);
    // //updateSet(ally, 49);
    // //updateSet(enemy, 48);
    //
    // *GetMapPosPointer(start.x, start.y, temp) = 254;
    //
    // for (size_t y = 0; y < height; y++) {
    // for (size_t x = 0; x < width; x++) {
    // ss << out(*GetMapPosPointer(x, y, temp)) << ' ';
    // }
    // ss << endl;
    // }
    //
    // delete[] temp;
    //
    // return ss.str();
    // }

    public int GetAbleLineRange(Coord start, int range, int dir, Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc, int ignoreFlag // Move range ignore ally && Attack range ignore enemy by default
            , boolean attack) { // Attack = use ignore flag, only consider map collision
        // Repel = consider both map collision & unit
        if (updateMap && !attack) {
            UpdateMap();
        }

        boolean moveIgnoreZoc = (ignoreFlag & 0b10000) != 0; // Move through zoc
        boolean moveIgnoreAlly = (ignoreFlag & 0b01000) != 0; // Move through ally
        boolean moveIgnoreEnemy = (ignoreFlag & 0b00100) != 0; // Move through enemy
        boolean attackIgnoreAlly = (ignoreFlag & 0b00010) != 0; // Attack ally (e.g., heal)
        boolean attackIgnoreEnemy = (ignoreFlag & 0b00001) != 0; // Attack enemy

        Predicate<Coord> ignoreCoord = (Coord p) -> {
            BiPredicate<Vector<Coord>, Coord> findSet = (Vector<Coord> set, Coord fp) -> {
                return set.contains(fp);
            };

            if (GetMap(p.x, p.y, this.map) == MAP_OBSTACLE) {
                return false;
            }

            boolean curAlly = ally == null ? false : findSet.test(ally, p);
            boolean curEnemy = enemy == null ? false : findSet.test(enemy, p);

            if (attack) {
                if (!attackIgnoreAlly && curAlly) {
                    return false;
                }
                if (!attackIgnoreEnemy && curEnemy) {
                    return false;
                }
            } else {
                if (curAlly) {
                    return false;
                }
                if (curEnemy) {
                    return false;
                }
            }

            return true;
        };

        Offset offset = normalNeighbour[dir];
        Coord coord = start;
        int moveable = 0;

        while (moveable <= range) {
            if (!ignoreCoord.test(coord)) {
                break;
            }

            coord.x += offset.x;
            coord.y += offset.y;

            moveable++;
        }

        moveable--; // remove last while add

        return moveable;
    }

    public void CalcLineArea(Coord start, int range, int startRange, int dir, Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc, int ignoreFlag // Move range ignore ally && Attack range ignore enemy by default
            , boolean attack) { // Attack = use ignore flag, only consider map collision
        if (updateMap) {
            UpdateMap();
        }

        if (stash) {
            settings = new AreaRange(range, startRange, attack, false, 0);
            stashAreaKey = new StashAreaKey(true, start, settings, ignoreFlag, ally, enemy, zoc);

            if (stashArea.containsKey(stashAreaKey)) {
                area = stashArea.get(stashAreaKey).area;

                return;
            }
        }

        area.clear();

        if (GetMap(start.x, start.y, this.map) == MAP_OBSTACLE) {
            return;
        }

        ranges = new int[4];

        for (int itDir = 0; itDir < 4; itDir++) {
            ranges[itDir] = GetAbleLineRange(start, range, itDir, ally, enemy, zoc, ignoreFlag, attack);
        }

        for (int nest = 0; nest <= range; nest++) {
            if (!(nest > startRange)) {
                continue;
            }

            area.add(new Vector<Coord>());
            area.lastElement().ensureCapacity(4);

            boolean updated = false;
            for (int it = 0; it < 4; it++) {
                if (ranges[it] >= nest) {
                    area.lastElement().add(
                            new Coord(start.x + normalNeighbour[it].x * nest, start.y + normalNeighbour[it].y * nest));

                    updated = true;
                }
            }

            if (!updated) {
                break;
            }
        }

        if (stash) {
            stashArea.put(stashAreaKey, new StashAreaValue(area, 0));
        }
    }

    public void CalcArea(Coord start, int range, int startRange, Vector<Coord> ally, Vector<Coord> enemy,
            Vector<Coord> zoc, int ignoreFlag // Move range ignore ally && Attack range ignore enemy by default
            , boolean attack // ignored if allRange = true
            // allRange: Calc move & attack at the same time
            , boolean allRange, int allRangeAttackRange, int extraRangeStartPosOut, boolean diagonal) {
        // TODO

    }

    public Vector<Vector<Coord>> GetArea() {
        return area;
    }

    public static int GetIgnoreFlag(boolean moveIgnoreZoc, boolean moveIgnoreAlly, boolean moveIgnoreEnemy,
            boolean attackIgnoreAlly, boolean attackIgnoreEnemy) {
        return (moveIgnoreZoc ? 1 : 0) << 4
                | (moveIgnoreAlly ? 1 : 0) << 3
                | (moveIgnoreEnemy ? 1 : 0) << 2
                | (attackIgnoreAlly ? 1 : 0) << 1
                | (attackIgnoreEnemy ? 1 : 0);
    }

    public boolean GetStash() {
        return stash;
    }

    public void SetStash(boolean state) {
        stash = state;
    }

    public void ClearStash() {
        stashPath.clear();
        stashArea.clear();
    }

    public int GetPathStashSize() {
        return stashPath.size();
    }

    public int GetAreaStashSize() {
        return stashArea.size();
    }
}
