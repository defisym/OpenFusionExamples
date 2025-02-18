#pragma once

#include <type_traits>

template<typename T>
concept ArithmeticConcept = std::is_arithmetic_v<T>;