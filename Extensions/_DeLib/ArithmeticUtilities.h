#pragma once

#include "ArithmeticTraits.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <limits>

// ------------------------------------------
// Range
// ------------------------------------------

template<ArithmeticConcept T>
inline T Range(T v, T minv, T maxv) {
	return (std::max)(minv, (std::min)(maxv, v));
}

template<ArithmeticConcept T>
inline void UpdateRange(T& v, T minv, T maxv) {
	v = (std::max)(minv, (std::min)(maxv, v));
}

// ------------------------------------------
// float compare
// ------------------------------------------

// https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
template<typename T>
inline bool NearlyEqualCore(const T a, const T b,
						// those defaults are arbitrary and could be removed
						const T epsilon,
						const T abs_th) {
	//assert(std::numeric_limits<float>::epsilon() <= epsilon);
	//assert(epsilon < 1.f);

	if (a == b) return true;  // NOLINT(clang-diagnostic-float-equal)

	const auto diff = std::abs(a - b);
	// or even faster:
	//const auto norm = (std::min)(std::abs(a + b),
	const auto norm = (std::min)(std::abs(a) + std::abs(b),
		(std::numeric_limits<T>::max)());
	// keeping this commented out until I update figures below
	return diff < (std::max)(abs_th, epsilon * norm);
}

inline bool NearlyEqualFLT(const float a, const float b,
						// those defaults are arbitrary and could be removed
						const float epsilon = 128 * FLT_EPSILON,
						const float abs_th = FLT_MIN) {
	return NearlyEqualCore<float>(a, b, epsilon, abs_th);
}

inline bool NearlyEqualDBL(const double a, const double b,
						// those defaults are arbitrary and could be removed
						const double epsilon = 128 * DBL_EPSILON,
						const double abs_th = DBL_MIN) {
	return NearlyEqualCore<double>(a, b, epsilon, abs_th);
}

inline bool NearlyEqualLDBL(const long double a, const long double b,
						// those defaults are arbitrary and could be removed
						const long double epsilon = 128 * LDBL_EPSILON,
						const long double abs_th = LDBL_MIN) {
	return NearlyEqualCore<long double>(a, b, epsilon, abs_th);
}
