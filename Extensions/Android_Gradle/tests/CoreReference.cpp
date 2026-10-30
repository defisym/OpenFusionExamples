#include "FindTheWay.h"
#include "AOE.h"
#include <cassert>
#include <fstream>
#include <queue>
#include <random>
using namespace FindTheWay;
static std::string flat(const CoordSet& values) {
    std::string result;
    for (auto c:values) { result+=std::to_string(c.x)+","+std::to_string(c.y)+";"; }
    return result;
}
static std::string area(FindTheWayClass& map) {
    std::string result;
    for (const auto& ring:map.GetArea()) { result+="["+flat(ring)+"]"; }
    return result;
}
static void require(bool value) { if (!value) { throw std::runtime_error("Core reference check failed"); } }
// Independent Dijkstra oracle checks weighted A* results, rather than repeating A* implementation.
static int oracle(FindTheWayClass& map) {
    using Node=std::pair<int,int>;
    std::priority_queue<Node,std::vector<Node>,std::greater<Node>> open;
    int distance[64]; std::fill(std::begin(distance),std::end(distance),INT32_MAX);
    distance[0]=0; open.emplace(0,0);
    while(!open.empty()) {
        auto [cost,id]=open.top(); open.pop();
        if(cost!=distance[id]) continue;
        if(id==63) return cost;
        for(auto [dx,dy]:std::vector<std::pair<int,int>>{{1,0},{0,1},{-1,0},{0,-1}}) {
            int x=id%8+dx,y=id/8+dy;
            if(x<0||y<0||x>=8||y>=8) continue;
            auto cell=map.GetMap(x,y,MapType::MAP);
            if(cell.IsObstacle()) continue;
            int next=y*8+x, value=cost+10+cell.GetCost();
            if(value<distance[next]) { distance[next]=value; open.emplace(value,next); }
        }
    }
    return -1;
}
int main(int argc,char** argv) {
    require(argc==2);
    std::ofstream out(argv[1]);
    FindTheWayClass map(4,4);
    map.Find({0,0},{3,3});
    CoordSet path;
    for(size_t i=0;i<map.GetDistance();++i) path.emplace_back(map.GetCoordAtPath(i,CoordType::X),map.GetCoordAtPath(i,CoordType::Y));
    out<<"path="<<flat(path)<<'\n';
    CoordSet empty;
    map.CalcLineArea({1,1},2,0,&empty,&empty,&empty,9,true);
    out<<"line="<<area(map)<<'\n';
    map.CalcArea({1,1},2,0,&empty,&empty,&empty,9,false);
    out<<"area="<<area(map)<<'\n';
    CoordSet zoc;
    map.GenerateZoc({1,1},&zoc,false,false);
    out<<"zoc="<<flat(zoc)<<'\n';
    map.SetIsometric(true);map.SetGridSize(map.GetIsometricGridSize(64,32));
    CoordSet iso;
    map.IterateMap(MapType::MAP,[&](size_t x,size_t y,const MapData&){iso.push_back(map.GetRealCoord({x,y}));});
    out<<"iso="<<flat(iso)<<'\n';
    AOE::AOEClass aoe;
    for(int type:{11,12,21,22,23,24,25,26,31,32,41}) for(int dir=0;dir<4;++dir) {
        CoordSet values;aoe.GetAOE({4,4},dir,type,&values);
        out<<"aoe"<<type<<"_"<<dir<<"="<<flat(values)<<'\n';
    }
    std::mt19937 random(20261026);
    for(int trial=0;trial<200;++trial) {
        FindTheWayClass weighted(8,8);
        for(int y=0;y<8;++y) for(int x=0;x<8;++x) {
            if((x==0&&y==0)||(x==7&&y==7)) continue;
            unsigned r=random()%10;
            weighted.SetMap(x,y,MapData(static_cast<MapCost>(random()%70),0,r==0),MapType::TERRAIN);
        }
        int expected=oracle(weighted);
        weighted.Find({0,0},{7,7});
        require(weighted.GetPathValidity()==(expected!=-1));
        if(expected!=-1) {
            int actual=0;
            for(size_t i=1;i<weighted.GetDistance();++i) {
                auto x=weighted.GetCoordAtPath(i,CoordType::X),y=weighted.GetCoordAtPath(i,CoordType::Y);
                actual+=10+weighted.GetMap(x,y,MapType::MAP).GetCost();
            }
            require(actual==expected);
        }
    }
    // Optional/null lists, saturated layers, and zero-range all-range expansion.
    FindTheWayClass edge(3,3);
    CoordSet zone{{1,0}};
    edge.Find({0,0},{2,2},false,true,nullptr,nullptr,&zone);
    edge.CalcArea({1,1},0,0,nullptr,nullptr,nullptr,9,false,true,1);
    require(!edge.GetArea().empty());
    std::cout<<"PASS: direct C++ reference; 200 weighted maps checked against Dijkstra\n";
}
