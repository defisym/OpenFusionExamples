#pragma once

#define ONSETMAPBYCOLLISION	3
#define ONITERATESTEP		6
#define ONITERATEAREA		14
#define ONMAPCHANGE			17
#define ONCREATEZOC			18

#define RetIfMapInvalid(X) if (rdPtr->pFTW == nullptr) { return X; }
#define RetIfSetMapDirectly(type,X) if (type == MapType::MAP) { return X; }
