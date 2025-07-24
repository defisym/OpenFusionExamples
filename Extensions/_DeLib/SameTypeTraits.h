#pragma once

#include <concepts>

template<typename T, typename... Ts>
concept SameType = (std::same_as<T, Ts> && ...);
