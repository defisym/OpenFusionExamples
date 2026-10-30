#pragma once

#include <cstdint>

namespace FindTheWay {
    using MapCost = unsigned char;
    // Application-defined category; zero means unspecified. Not a movement profile.
    using TerrainType = std::uint32_t;

    class MapData {
        TerrainType terrainType = 0;
        MapCost cost = 0;
        bool obstacle = false;
    public:
        constexpr MapData() = default;
        constexpr MapData(MapCost cost, TerrainType terrainType, bool obstacle)
            : terrainType(terrainType), cost(cost), obstacle(obstacle) {}

        constexpr MapCost GetCost() const { return cost; }
        constexpr TerrainType GetTerrainType() const { return terrainType; }
        constexpr bool IsObstacle() const { return obstacle; }
        constexpr void SetCost(MapCost value) { cost = value; }
        constexpr void SetTerrainType(TerrainType value) { terrainType = value; }
        constexpr void SetObstacle(bool value) { obstacle = value; }

        constexpr bool operator==(const MapData&) const = default;
    };
}
