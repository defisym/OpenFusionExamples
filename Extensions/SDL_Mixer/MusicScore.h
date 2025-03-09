#pragma once

#include <map>
#include <vector>

namespace MusicScore {
	using MusicalNotes = std::vector<float>;

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

	inline size_t GetSize(const MusicalNotes* pNote) {
		return pNote != nullptr
			? pNote->size()
			: 0;
	}

	inline float GetNote(int pos, const MusicalNotes* pNote, float base = 0) {
		if (pNote == nullptr) {
			return 0;
		}

		return (*pNote)[pos % pNote->size()] - base;
	}

	inline MusicalNotes GetNote(std::wstring& score) {
		const std::vector<wchar_t> frontChars = { L' ',L'{',L',' };
		const std::vector<wchar_t> backChars = { L' ',L'}',L',' };

		TrimStr(score, frontChars, backChars);

		constexpr auto arrayDelimiter = L',';

		const auto elements = SplitString<wchar_t>(score, arrayDelimiter);

		MusicScore::MusicalNotes notes;

		for (auto& note : elements) {
			notes.push_back(_stof(note));
		}

		return notes;
	}

}
