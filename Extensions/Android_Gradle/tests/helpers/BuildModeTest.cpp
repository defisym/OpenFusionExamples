#include "FindTheWay.h"
int main() {
    FindTheWay::FindTheWayClass map(1,1);
    return map.GetStash() == (EXPECT_STASH != 0) ? 0 : 1;
}
