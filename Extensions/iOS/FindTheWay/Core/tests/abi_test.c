#include "FindTheWayCore.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Failed line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
    FTWMap handle=0, unchanged=91;
    int32_t cost=-1;
    CHECK(ftw_abi_version()==2);
    CHECK(ftw_map_create(0,4,&unchanged)==FTW_INVALID_ARGUMENT && unchanged==91);
    CHECK(ftw_map_create(INT32_MAX,2,&handle)==FTW_INVALID_ARGUMENT);
    CHECK(ftw_map_create(4,4,&handle)==FTW_OK && handle!=0);
    CHECK(ftw_map_set_cost(handle,1,2,37,FTW_TERRAIN)==FTW_OK);
    CHECK(ftw_map_get_cost(handle,1,2,FTW_TERRAIN,&cost)==FTW_OK && cost==37);
    CHECK(ftw_map_get_cost(handle,1,2,9,&cost)==FTW_INVALID_ARGUMENT && cost==37);
    CHECK(ftw_map_set_cost(handle,-1,0,4,FTW_MAP)==FTW_INVALID_ARGUMENT);
    CHECK(ftw_map_set_cost(handle,4,0,4,FTW_MAP)==FTW_INVALID_ARGUMENT);
    CHECK(ftw_map_get_cost(handle,0,0,FTW_MAP,0)==FTW_INVALID_ARGUMENT);
    {
        uint64_t count=99;
        FTWCoord points[16], sentinel={-7,-8};
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},0,&count)==FTW_OK && count==7);
        CHECK(ftw_map_copy_path(handle,0,0,&count)==FTW_OK && count==7);
        points[0]=sentinel;
        CHECK(ftw_map_copy_path(handle,points,1,&count)==FTW_BUFFER_TOO_SMALL && count==7);
        CHECK(points[0].x==-7 && points[0].y==-8);
        CHECK(ftw_map_copy_path(handle,points,16,&count)==FTW_OK && count==7);
        CHECK(points[0].x==0 && points[0].y==0 && points[6].x==3 && points[6].y==3);
        for (uint64_t i=1;i<count;++i) {
            int dx=points[i].x-points[i-1].x, dy=points[i].y-points[i-1].y;
            CHECK(dx*dx+dy*dy==1);
        }
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},FTW_FIND_DIAGONAL|FTW_FIND_CHECK_CORNERS,&count)==FTW_OK && count==4);
        CHECK(ftw_map_find_path(handle,(FTWCoord){-1,0},(FTWCoord){3,3},0,&count)==FTW_INVALID_ARGUMENT && count==4);
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},128,&count)==FTW_INVALID_ARGUMENT);
        CHECK(ftw_map_set_cost(handle,0,0,255,FTW_TERRAIN)==FTW_OK);
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},0,&count)==FTW_OK && count==0);
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},FTW_FIND_FORCE,&count)==FTW_OK && count>0);
        CHECK(ftw_map_get_cost(handle,0,0,FTW_MAP,&cost)==FTW_OK && cost==255);
        CHECK(ftw_map_set_cost(handle,0,0,0,FTW_TERRAIN)==FTW_OK);
        for (int y=0;y<4;++y) CHECK(ftw_map_set_cost(handle,2,y,255,FTW_TERRAIN)==FTW_OK);
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},0,&count)==FTW_OK && count==0);
        CHECK(ftw_map_copy_path(handle,points,16,&count)==FTW_OK && count==0);
        CHECK(ftw_map_set_cost(handle,2,2,0,FTW_TERRAIN)==FTW_OK);
        CHECK(ftw_map_find_path(handle,(FTWCoord){0,0},(FTWCoord){3,3},0,&count)==FTW_OK && count>0);
        CHECK(ftw_map_copy_path(handle,points,16,&count)==FTW_OK);
        for (uint64_t i=0;i<count;++i) CHECK(points[i].x!=2 || points[i].y==2);
    }
    {
        FTWCoord coord={-9,-9};
        uint64_t length=0;
        CHECK(ftw_map_set_grid(handle,0,32,32,10,20)==FTW_OK);
        CHECK(ftw_map_convert_coord(handle,(FTWCoord){2,1},0,&coord)==FTW_OK);
        CHECK(coord.x==90 && coord.y==68);
        CHECK(ftw_map_convert_coord(handle,coord,1,&coord)==FTW_OK && coord.x==2 && coord.y==1);
        CHECK(ftw_map_set_grid(handle,1,64,32,10,20)==FTW_OK);
        CHECK(ftw_map_convert_coord(handle,(FTWCoord){2,1},0,&coord)==FTW_OK);
        CHECK(ftw_map_convert_coord(handle,coord,1,&coord)==FTW_OK && coord.x==2 && coord.y==1);
        CHECK(ftw_map_set_grid(handle,1,65536,32,0,0)==FTW_INVALID_ARGUMENT);
        CHECK(ftw_map_set_grid(handle,0,32,32,-1,0)==FTW_OK);
        CHECK(ftw_map_export(handle,0,0,&length)==FTW_OK && length>0);
        char* encoded=(char*)malloc((size_t)length);
        CHECK(encoded!=0);
        encoded[0]='!';
        CHECK(ftw_map_export(handle,encoded,1,&length)==FTW_BUFFER_TOO_SMALL && encoded[0]=='!');
        CHECK(ftw_map_export(handle,encoded,length,&length)==FTW_OK);
        FTWMap restored=0;
        CHECK(ftw_map_create_serialized(encoded,length,&restored)==FTW_OK && restored!=handle);
        CHECK(ftw_map_get_cost(restored,1,2,FTW_TERRAIN,&cost)==FTW_OK && cost==37);
        CHECK(ftw_map_get_cost(restored,2,0,FTW_TERRAIN,&cost)==FTW_OK && cost==255);
        CHECK(ftw_map_release(restored)==FTW_OK);
        restored=77;
        encoded[0]='!';
        CHECK(ftw_map_create_serialized(encoded,length,&restored)==FTW_INVALID_ARGUMENT && restored==77);
        CHECK(ftw_map_create_serialized("bad",3,&restored)==FTW_INVALID_ARGUMENT && restored==77);
        free(encoded);
        cost=37;
    }
    CHECK(ftw_map_release(handle)==FTW_OK);
    CHECK(ftw_map_release(handle)==FTW_OK);
    CHECK(ftw_map_get_cost(handle,0,0,FTW_MAP,&cost)==FTW_INVALID_HANDLE && cost==37);
    CHECK(ftw_map_create(2,2,&unchanged)==FTW_OK && unchanged!=handle);
    CHECK(ftw_map_release(unchanged)==FTW_OK);
    return 0;
}
