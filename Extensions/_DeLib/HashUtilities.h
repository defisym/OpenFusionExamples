#pragma once

#include <cstddef>

// ------------------------------------------
// Hasher
// ------------------------------------------
//   Calculate hash of given object
//     inline static size_t Hasher(const Object& o) {
//         size_t seed = ElementNum;
//         
//         seed ^= o.ele_1 + HASHER_MOVE(seed);
//         seed ^= o.ele_2 + HASHER_MOVE(seed);
//         ...
//         seed ^= o.ele_ElementNum + HASHER_MOVE(seed);
//         
//         return seed;
//     }
static constexpr auto HASHER_MAGICNUMBER = 0x9e3779b9;
static constexpr auto HASHER_MOVE(size_t seed) { return HASHER_MAGICNUMBER + (seed << 6) + (seed >> 2); }
