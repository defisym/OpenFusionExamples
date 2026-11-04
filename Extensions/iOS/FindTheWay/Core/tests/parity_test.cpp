// Direct oracle: the same C++ classes and call patterns as Windows Main.cpp.
#include "FindTheWayCore.h"
#include "FindTheWayFusion.h"
#include "AOE.h"
#include <cassert>
#include <iostream>
#include <codecvt>
#include <locale>
using namespace FindTheWay;
static void check(bool value,int line) {if(!value){std::cerr<<"Parity check failed at "<<line<<'\n';std::abort();}}
#define CHECK(x) check((x),__LINE__)
static void on_update(void* p,FTWMap map) {++*static_cast<int*>(p);}
static std::vector<FTWCoord> path(FTWMap h,const char* name) {
    uint64_t count=0;CHECK(ftw_map_named_path(h,name,nullptr,0,&count)==FTW_OK);
    std::vector<FTWCoord> result(count);CHECK(ftw_map_named_path(h,name,result.data(),count,&count)==FTW_OK);return result;
}
static void replace_in_callback(void* p,FTWMap old) {
    FTWMap* replacement=static_cast<FTWMap*>(p);
    CHECK(ftw_map_release(old)==FTW_OK);
    CHECK(ftw_map_create(2,2,replacement)==FTW_OK);
}
int main() {
    {
        FTWMap old=0,replacement=0;uint64_t count=0;
        CHECK(ftw_map_create(3,3,&old)==FTW_OK);
        CHECK(ftw_map_set_cost(old,1,1,3,FTW_TERRAIN)==FTW_OK);
        CHECK(ftw_map_set_callback(old,replace_in_callback,&replacement)==FTW_OK);
        CHECK(ftw_map_find_ex(old,{0,0},{2,2},0,9,nullptr,&count)==FTW_OK && count==5);
        CHECK(replacement && replacement!=old);
        int value=-1;CHECK(ftw_map_get_cost(old,0,0,0,&value)==FTW_INVALID_HANDLE);
        CHECK(ftw_map_release(replacement)==FTW_OK);
    }
    for(int iso=0;iso<2;++iso) {
        FTWMap h=0;CHECK(ftw_map_create(9,8,&h)==FTW_OK);
        FindTheWayFusion win(9,8);win.SetIsometric(iso);win.SetGridSize(iso?win.GetIsometricGridSize(64,32):32,10,20);
        CHECK(ftw_map_set_grid(h,iso,iso?64:32,32,10,20)==FTW_OK);
        for(int y=0;y<8;++y)for(int x=0;x<9;++x) {
            int cost=(x==4 && y!=3)?255:((x*17+y*11)%5);
            win.SetMap(x,y,cost,MapType::TERRAIN);CHECK(ftw_map_set_cost(h,x,y,cost,FTW_TERRAIN)==FTW_OK);
            FTWCoord point;CHECK(ftw_map_convert_coord(h,{x,y},0,&point)==FTW_OK);
            auto expected=win.GetRealCoord({x,y});CHECK(point.x==expected.x && point.y==expected.y);
        }
        CoordSet ally={{2,2},{3,2}},enemy={{5,3},{6,3}},zoc={{3,3},{5,2}};
        FTWCoord a[]={{2,2},{3,2}},e[]={{5,3},{6,3}},z[]={{3,3},{5,2}};
        FTWLists lists={a,e,z,2,2,2};CHECK(ftw_map_set_lists(h,&lists)==FTW_OK);
        int updates=0;CHECK(ftw_map_set_callback(h,on_update,&updates)==FTW_OK);
        const std::wstring savedName=L"路线";
        for(unsigned ignore=0;ignore<32;++ignore)for(unsigned flags=0;flags<8;++flags) {
            Coord start{1,3},goal{7,3};
            if(flags&4)win.ForceFind(start,goal,flags&1,flags&2,&ally,&enemy,&zoc,ignore);
            else win.Find(start,goal,flags&1,flags&2,&ally,&enemy,&zoc,ignore);
            win.SaveLastPath(L"路线");
            uint64_t count=999;
            CHECK(ftw_map_find_ex(h,{1,3},{7,3},flags,ignore,"路线",&count)==FTW_OK);
            CHECK(count==(win.GetPathValidity()?win.GetDistance():0));
            auto result=path(h,nullptr);
            for(size_t i=0;i<result.size();++i) CHECK(result[i].x==win.GetCoordAtPath(i,CoordType::X) && result[i].y==win.GetCoordAtPath(i,CoordType::Y));
            CHECK(path(h,"\xE8\xB7\xAF\xE7\xBA\xBF").size()==(win.GetPathValidity(&savedName)?win.GetDistance(&savedName):0));
        }
        CHECK(updates==1);
        for(int stash=0;stash<2;++stash) {
            win.SetStash(stash);CHECK(ftw_map_stash(h,stash)==FTW_OK);
            for(int repeat=0;repeat<2;++repeat)for(int line=0;line<2;++line)for(int attack=0;attack<2;++attack)for(int all=0;all<2;++all) {
                FTWAreaOptions options={{3,3},line,attack,3,1,9,all,2};
                size_t extra=65536;
                if(line)win.CalcLineArea({3,3},3,1,&ally,&enemy,&zoc,9,attack);
                else win.CalcArea({3,3},3,1,&ally,&enemy,&zoc,9,attack,all,2,&extra);
                uint64_t rings=999,boundary=999,count=0;
                CHECK(ftw_map_area(h,&options,&rings,&boundary)==FTW_OK);
                CHECK(rings==win.GetArea().size() && boundary==extra);
                CHECK(ftw_map_copy_area(h,nullptr,0,&count)==FTW_OK);
                std::vector<FTWAreaPoint> result(count);
                CHECK(ftw_map_copy_area(h,result.data(),count,&count)==FTW_OK);
                size_t index=0;
                for(size_t i=0;i<win.GetArea().size();++i)for(auto point:win.GetArea()[i]) {
                    CHECK(index<result.size() && result[index].ring==i && result[index].coord.x==point.x && result[index].coord.y==point.y);++index;
                } CHECK(index==result.size());
            }
        }
        for(int dir=0;dir<5;++dir)for(int attack=0;attack<2;++attack) {
            int result=0;CHECK(ftw_map_line_range(h,{1,3},dir,8,9,attack,&result)==FTW_OK);
            CHECK(result==(int)win.GetAbleLineRange({1,3},8,dir,&ally,&enemy,&zoc,9,attack));
        }
        AOE::AOEClass aoe;
        for(unsigned type: {11u,12u,13u,21u,22u,23u,24u,25u,26u,31u,32u,41u})for(unsigned dir=0;dir<4;++dir)for(unsigned ignore=0;ignore<32;++ignore) {
            CoordSet expected;aoe.GetAOE({3,3},dir,type,&expected);
            auto flags=win.ParseIgnoreFlag(ignore);
            std::vector<Coord> filtered;
            for(auto p:expected) {
                if(win.GetMap(p.x,p.y,MapType::MAP)==255)continue;
                if(!std::get<3>(flags) && std::find(ally.begin(),ally.end(),p)!=ally.end())continue;
                if(!std::get<4>(flags) && std::find(enemy.begin(),enemy.end(),p)!=enemy.end())continue;
                filtered.push_back(p);
            }
            uint64_t count=0;CHECK(ftw_map_aoe(h,{3,3},dir,type,ignore,nullptr,0,&count)==FTW_OK);
            std::vector<FTWCoord> result(count);CHECK(ftw_map_aoe(h,{3,3},dir,type,ignore,result.data(),count,&count)==FTW_OK);
            CHECK(result.size()==filtered.size());
            for(size_t i=0;i<result.size();++i)CHECK(result[i].x==filtered[i].x && result[i].y==filtered[i].y);
        }
        for(FTWCoord c:{FTWCoord{-1,0},FTWCoord{9,8},FTWCoord{4,3}})for(int layer=0;layer<3;++layer) {
            int cost=-1;CHECK(ftw_map_sample_cost(h,c.x,c.y,layer,&cost)==FTW_OK);
            CHECK(cost==(int)win.GetMap(static_cast<size_t>(c.x),static_cast<size_t>(c.y),static_cast<MapType>(layer)));
        }
        uint64_t count=0;CHECK(ftw_map_display(h,0,1,nullptr,nullptr,0,&count)==FTW_OK);
        std::string text(count,' ');CHECK(ftw_map_display(h,0,1,nullptr,text.data(),count,&count)==FTW_OK);
        CHECK(text==std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(win.OutPutMapStr()));
        CHECK(ftw_map_release(h)==FTW_OK);
    }
    for(FTWCoord start: {FTWCoord{0,0},FTWCoord{4,4},FTWCoord{-1,-1}}) {
        CoordSet expected;FindTheWayClass::GenerateZoc({start.x,start.y},&expected,false);
        uint64_t count=0;CHECK(ftw_generate_zoc(start,nullptr,0,&count)==FTW_OK);
        std::vector<FTWCoord> result(count);CHECK(ftw_generate_zoc(start,result.data(),count,&count)==FTW_OK);
        CHECK(result.size()==expected.size());for(size_t i=0;i<result.size();++i)CHECK(result[i].x==expected[i].x && result[i].y==expected[i].y);
    }
    std::cout<<"Windows C++ / iOS C ABI parity checks passed\n";
}
