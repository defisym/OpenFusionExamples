#include "FindTheWay.h"
#include <stdexcept>
#include <iostream>
#include <queue>
#include <random>
#include <array>
using namespace FindTheWay;
static int checks;
static void check(bool result) { ++checks; if (!result) { throw std::runtime_error("MapData regression"); } }
int main() {
    MapData cell(255, 0xffffffffU, false);
    check(!cell.IsObstacle() && cell.GetCost() == 255 && cell.GetTerrainType() == 0xffffffffU);
    cell.SetObstacle(true); cell.SetCost(0);
    check(cell.IsObstacle() && cell.GetCost() == 0);
    FindTheWayClass map(3, 1);
    map.SetStash(true);
    map.SetMapData(1, 0, MapData(255, 42, false));
    map.Find({0,0}, {2,0});
    check(map.GetPathValidity());
    check(map.GetPointCost({1,0}) == 255 && !map.CoordObstacle(1,0,MapType::MAP));
    check(map.GetMap(1,0,MapType::MAP) == MapData(255,42,false));
    auto rich = map.GetMap();
    check(rich.starts_with(L"FTW2:"));
    FindTheWayClass copy(rich);
    check(copy.GetMapData(1,0,MapType::TERRAIN) == MapData(255,42,false));
    check(copy.GetMap() == rich);
    map.SetMapData(1,0,MapData(0,42,true));
    map.Find({0,0},{2,0});
    check(!map.GetPathValidity()); // Metadata mutation invalidates cached paths.
    map.CalcLineArea({0,0},3,0);
    check(map.GetAbleLineRange({0,0},3,0) == 0);
    auto before = map.GetMapData(1,0,MapType::MAP);
    map.ForceFind({1,0},{2,0});
    check(map.GetPathValidity());
    check(map.GetMapData(1,0,MapType::MAP) == before);
    map.SetMap(1,0,MapData(20,42,false),MapType::TERRAIN);
    check(map.GetMapData(1,0,MapType::TERRAIN).GetTerrainType() == 42);
    map.SetMapData(1,0,MapData(200,42,false));
    map.SetMapData(1,0,MapData(100,7,false),MapType::DYNAMIC);
    check(map.GetMapData(1,0,MapType::MAP) == MapData(255,42,false));
    map.SetMapData(1,0,MapData(0,7,true),MapType::DYNAMIC);
    check(map.CoordObstacle(1,0,MapType::MAP));
    auto serialized = map.GetMap();
    FindTheWayClass round(serialized);
    check(round.GetMapData(1,0,MapType::DYNAMIC) == MapData(0,7,true));
    bool invalid = false;
    try { map.SetMap(L"FTW2:3:1:AA==:AA=="); } catch (...) { invalid = true; }
    check(invalid && map.GetMap() == serialized);
    const std::wstring old = L"2:1:AP8=:AAA=";
    FindTheWayClass imported(old);
    check(imported.GetMapData(1,0,MapType::TERRAIN).IsObstacle());
    check(imported.GetMap().starts_with(L"FTW2:"));
    FindTheWayClass upgraded(imported.GetMap());
    check(upgraded.GetMapData(1,0,MapType::TERRAIN) == imported.GetMapData(1,0,MapType::TERRAIN));
    check(imported.GetMapData(999,0,MapType::MAP).IsObstacle());
    // Independent Dijkstra comparison includes 255-cost passable cells and explicit low-cost obstacles.
    std::mt19937 rng(20261027);
    for (int trial = 0; trial < 100; ++trial) {
        FindTheWayClass weighted(4,4);
        for (int y = 0; y < 4; ++y) for (int x = 0; x < 4; ++x) {
            bool blocked = (rng() % 5 == 0) && !(x == 0 && y == 0) && !(x == 3 && y == 3);
            weighted.SetMapData(x,y,MapData(static_cast<MapCost>(rng()),rng(),blocked));
            weighted.SetMapData(x,y,MapData(static_cast<MapCost>(rng()),rng(),false),MapType::DYNAMIC);
        }
        weighted.UpdateMap();
        using Node = std::pair<int,int>;
        std::priority_queue<Node,std::vector<Node>,std::greater<Node>> pending;
        std::array<int,16> dist; dist.fill(INT32_MAX); dist[0] = 0;
        pending.emplace(0,0);
        while (!pending.empty()) {
            auto [cost,id] = pending.top(); pending.pop();
            if (cost != dist[id]) { continue; }
            for (auto [dx,dy] : std::array<std::pair<int,int>,4>{{{1,0},{-1,0},{0,1},{0,-1}}}) {
                int x = id % 4 + dx, y = id / 4 + dy;
                if (x < 0 || y < 0 || x >= 4 || y >= 4) { continue; }
                auto next = weighted.GetMapData(x,y,MapType::MAP,false);
                if (next.IsObstacle()) { continue; }
                int index = y * 4 + x, candidate = cost + 10 + next.GetCost();
                if (candidate < dist[index]) { dist[index] = candidate; pending.emplace(candidate,index); }
            }
        }
        weighted.Find({0,0},{3,3});
        check(weighted.GetPathValidity() == (dist[15] != INT32_MAX));
        if (weighted.GetPathValidity()) {
            int actual = 0;
            for (size_t i = 1; i < weighted.GetDistance(); ++i) {
                auto x = weighted.GetCoordAtPath(i,CoordType::X), y = weighted.GetCoordAtPath(i,CoordType::Y);
                actual += 10 + weighted.GetMapData(x,y,MapType::MAP,false).GetCost();
            }
            check(actual == dist[15]);
        }
        auto saved = weighted.GetMap();
        FindTheWayClass loaded(saved);
        for (int y = 0; y < 4; ++y) for (int x = 0; x < 4; ++x) {
            check(loaded.GetMapData(x,y,MapType::TERRAIN) == weighted.GetMapData(x,y,MapType::TERRAIN));
            check(loaded.GetMapData(x,y,MapType::DYNAMIC) == weighted.GetMapData(x,y,MapType::DYNAMIC));
        }
    }
    FindTheWayClass destructible(3,1);
    destructible.SetMapData(1,0,MapData(5,0,false),MapType::TERRAIN);
    destructible.SetMapData(1,0,MapData(0,0,true),MapType::DYNAMIC);
    destructible.Find({0,0},{2,0});
    check(!destructible.GetPathValidity());
    destructible.SetMapData(1,0,MapData{},MapType::DYNAMIC);
    destructible.Find({0,0},{2,0});
    check(destructible.GetPathValidity());
    check(destructible.GetMapData(1,0,MapType::MAP).GetCost() == 5);
    check(destructible.GetMapData(1,0,MapType::TERRAIN) == MapData(5,0,false));
    std::cout << "PASS: " << checks << " MapData checks\n";
}
