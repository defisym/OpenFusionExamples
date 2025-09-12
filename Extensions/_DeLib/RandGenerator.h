#pragma once

#include "RandomBase.h"

// -----------------------------------------------
// Type alias for a random number generator
// -----------------------------------------------

// Usage:
// auto rand = RandGenerator(min, max);
// int value = rand.Generate();
using RandGenerator = RandomBase<int, std::mt19937, std::uniform_int_distribution<int>>;
