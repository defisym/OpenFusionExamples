#pragma once
#include "FindTheWay.h"

namespace FindTheWay {
    // Compatibility layer for existing Fusion numeric events. Core APIs remain current.
    // Own this object through FindTheWayFusion, not a deleting base-class pointer.
    class FindTheWayFusion : public FindTheWayClass {
        static MapData DecodeCost(int cost, TerrainType metadata = 0) {
            const auto value = static_cast<MapCost>(Range(cost));
            return {value, metadata, value == MAP_OBSTACLE};
        }
        static MapCost EncodeCost(const MapData& cell) {
            return cell.IsObstacle() ? MAP_OBSTACLE
                : (cell.GetCost() == MAP_OBSTACLE ? MAP_OBSTACLE - 1 : cell.GetCost());
        }
    public:
        using FindTheWayClass::FindTheWayClass;
        void SetMap(const std::wstring& encoded) { FindTheWayClass::SetMap(encoded); }
        std::wstring GetMap() { return FindTheWayClass::GetMap(); }
        void SetMap(size_t x, size_t y, int cost, MapType type, bool update = true) {
            const auto metadata = GetMapData(x, y, type, false).GetTerrainType();
            FindTheWayClass::SetMap(x, y, DecodeCost(cost, metadata), type, update);
        }
        void ClearMap(MapType type, int cost = 0, bool update = true) {
            FindTheWayClass::ClearMap(type, DecodeCost(cost), update);
        }
        MapCost GetMap(size_t x, size_t y, MapType type, bool update = true) {
            return EncodeCost(FindTheWayClass::GetMap(x, y, type, update));
        }
        void IterateMap(MapType type,
            const std::function<void(size_t, size_t, MapCost)>& callback) const {
            FindTheWayClass::IterateMap(type, [&](size_t x, size_t y, const MapData& cell) {
                callback(x, y, EncodeCost(cell));
            });
        }
    };
}
