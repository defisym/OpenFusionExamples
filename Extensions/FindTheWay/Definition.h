#pragma once

constexpr auto ONSETMAPBYCOLLISION = 3;
constexpr auto ONITERATESTEP = 6;
constexpr auto ONITERATEAREA = 14;
constexpr auto ONMAPCHANGE = 17;
constexpr auto ONCREATEZOC = 18;
constexpr auto ONITERATEMAP = 28;

#define RetIfMapInvalid(X) if (rdPtr->pFTW == nullptr) { return X; }
#define RetIfSetMapDirectly(type,X) if ((type) == MapType::MAP) { return X; }
