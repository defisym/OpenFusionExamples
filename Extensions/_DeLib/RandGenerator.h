#pragma once

#include <random>

#include "ArithmeticTraits.h"

template <ArithmeticConcept T>
class RandGenerator {
private:
	std::random_device rd;
	
	//default_random_engine eng(rd());
	std::mt19937 eng{ rd() };

	std::uniform_int_distribution<T>* uniform_dist;

public:
	~RandGenerator() {
		delete uniform_dist;
	}

	inline RandGenerator(T a, T b) {
		uniform_dist = new std::uniform_int_distribution<T>(a, b);
	}
	inline T GenerateRandNumber() {
		return (*uniform_dist)(eng);
	}

};

