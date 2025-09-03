#pragma once

#include "RandomBase.h"

using RandGenerator = RandomBase<int, std::mt19937, std::uniform_int_distribution<int>>;
