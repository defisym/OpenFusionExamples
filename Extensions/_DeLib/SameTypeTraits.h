#pragma once

#include <concepts>

template<typename T, typename... Ts>
concept SameType = (std::same_as<T, Ts> && ...);

template<typename T, typename... Ts>
concept SameTypeDecay = (std::same_as<std::decay_t<T>, std::decay_t<Ts>> && ...);

template<typename T, typename... Ts>
concept SameTypeCVRef = (std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<Ts>> && ...);
