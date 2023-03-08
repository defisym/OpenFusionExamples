#pragma once

#include <map>
#include <vector>

namespace MusicScore {
	enum Score {
		Loop,
		NightLightsUpTheNight,
		CrystalPrelude,
	};

	using MusicalNotes=std::vector<float>;

	// base on input
	const static MusicalNotes loop = { 0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1 };

	// base on do
	const static  MusicalNotes nightLightsUpTheNight =
	{-2,-1,1,2,3, 2,1,2,2,1,2,3,
		-2,-1,1,2,3, 2,3,2,1,1,-1,2,3,
		5,3,3,2,3, 5,3,5, 6,6,5,3,
		3,3,1,2,3,2, 2,2,2,6,3,3,5,
		8,7,6,3,5, 3,2,2,3,5
	};
	// base on do
	const static  MusicalNotes crystalPrelude=
	{ -7,-6,-5,-3, 1,2,3,5,
		5,3,2,1, -3,-5,-6,-7
	};

	inline const MusicalNotes* GetElement(Score score) {
		switch (score) {
		case Loop:
			return &loop;
		case NightLightsUpTheNight:
			return &nightLightsUpTheNight;
		case CrystalPrelude:
			return &crystalPrelude;
		}

		return nullptr;
	}

	inline size_t GetSize(Score score) {
		const auto pNote = GetElement(score);

		return pNote != nullptr
			? pNote->size()
			: 0;
	};

	inline float GetNote(int pos, Score score = Loop, float base = 0) {
		const auto pNote = GetElement(score);

		if(pNote==nullptr) {
			return 0;
		}
		
		return (*pNote)[pos % pNote->size()] - base;
	}

}
