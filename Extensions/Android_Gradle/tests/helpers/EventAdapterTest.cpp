#include "FindTheWayFusion.h"
#include <iostream>
#include <stdexcept>
using namespace FindTheWay;
static int checks;
static void check(bool value) { ++checks; if (!value) { throw std::runtime_error("Event adapter regression"); } }
int main() {
    FindTheWayFusion map(2,1);
    map.SetMapData(1,0,MapData(255,42,false),MapType::TERRAIN);
    check(map.GetMapData(1,0,MapType::MAP) == MapData(255,42,false));
    check(map.GetMap(1,0,MapType::MAP) == 254);
    check(map.GetMapData(1,0,MapType::MAP).GetCost() == 255);
    bool fullCell = false;
    map.FindTheWayClass::IterateMap(MapType::MAP,[&](size_t x,size_t,const MapData& cell) {
        if (x == 1) { fullCell = cell.GetCost() == 255 && cell.GetTerrainType() == 42 && !cell.IsObstacle(); }
    });
    check(fullCell);
    map.SetMap(1,0,255,MapType::TERRAIN);
    check(map.GetMapData(1,0,MapType::TERRAIN).GetTerrainType() == 42);
    check(map.CoordObstacle(1,0,MapType::MAP));
    map.SetMap(1,0,10,MapType::TERRAIN);
    check(map.GetMapData(1,0,MapType::MAP) == MapData(10,42,false));
    map.ClearMap(MapType::TERRAIN,255);
    check(map.GetMapData(0,0,MapType::MAP).IsObstacle());
    check(map.GetMapData(1,0,MapType::TERRAIN).GetTerrainType() == 0);
    map.ClearMap(MapType::TERRAIN,0);
    check(!map.CoordObstacle(0,0,MapType::MAP));
    check(map.GetMap().starts_with(L"FTW2:"));
    std::cout << "PASS: " << checks << " event adapter checks\n";
}
