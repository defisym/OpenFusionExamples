#pragma once

#include <random>

using namespace std;

template <class T>
class RandGenerator {
private:
	random_device rd;
	
	//default_random_engine eng(rd());
	mt19937 eng{ rd() };

	std::uniform_int_distribution<T>* uniform_dist;

public:
	inline RandGenerator(T a, T b) {
		uniform_dist = new std::uniform_int_distribution<T>(a, b);
	}
	inline T GetRandNumber() {
		return (*uniform_dist)(eng);
	}

};

