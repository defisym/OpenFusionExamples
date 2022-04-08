#pragma once

// Origin version by Retriever2
// https://community.clickteam.com/threads/59029-Accessing-backdrops-from-extensions?p=441276&viewfull=1#post441276

#include <functional>
#include <string>
#include <vector>

#include "ccxhdr.h"

constexpr auto SPRS_RESERVE = 50;

class Collision {
private:
    LPRH rhPtr;

    npWin win;
    std::vector<npSpr> sprs;

    inline bool SpriteBackdrop(npSpr spr, int layer) {
        auto flags = GetFlags(spr);

        if (!(flags & SF_BACKGROUND) || !(flags & SF_OBSTACLE)) {
            return false;
        }

        auto lo = GetLO(spr);

        if (lo == nullptr) {
            return false;
        }

        if (layer != LAYER_ALL && lo->loLayer != layer) {
            return false;
        }

        return true;
    };

public:
    Collision(LPRH rhPtr) {
        this->rhPtr = rhPtr;
        this->win = rhPtr->rh4.rh4Mv->mvIdEditWin;

        sprs.reserve(SPRS_RESERVE);
    }
    ~Collision() {

    }

    inline DWORD GetFlags(npSpr spr) {
        return GetSpriteFlags(win, spr);
    }
    inline LPLO GetLO(npSpr spr) {
        return (LPLO)GetSpriteExtra(win, spr);
    }

    inline void GetSpriteList(int x, int y, int layer, int flag, bool backdrop = true) {
        sprs.clear();

        x = x - rhPtr->rh3.rh3DisplayX;
        y = y - rhPtr->rh3.rh3DisplayY;

        npSpr spr = nullptr;

        while ((spr = SpriteCol_TestPoint(win, spr, layer, x, y, flag)) != nullptr) {
            if (backdrop && !SpriteBackdrop(spr, layer)) {
                continue;
            }

            sprs.emplace_back(spr);
        }
    }

    inline void GetBackdropList(int x, int y, int layer, int flag) {
        GetSpriteList(x, y, layer, flag, true);
    }
};
