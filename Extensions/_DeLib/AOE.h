#pragma once

#include <vector>

#include "FindTheWay.h"

// AOE
namespace AOE{
	constexpr auto RESERVE = 50;

	class AOEClass {
		inline static const std::vector<Coord> dirOffset = { {1, 0},
			{ 0,-1 },
			{ -1,0 },
			{ 0,1 } };

		Coord start = { 0,0 };
		size_t dir = 0;

		std::vector<Coord> open_set;
		std::vector<Coord> close_set;
		std::vector<Coord> cur_set;

		inline void GetAOE_1_X(const size_t size, std::vector<Coord>* output) const {
			for (size_t i = 1; i <= size; i++) {
				output->emplace_back(start + Coord{ dirOffset[dir].x * i,dirOffset[dir].y * i });
			}
		}
		inline void GetAOE_2_X(const size_t type, std::vector<Coord>* output) const {
			int vSize = 0;
			int hSize = 0;

			switch (type) {
			case 21:
				vSize = 1;
				break;
			case 22:
				vSize = 2;
				break;
			case 23:
				vSize = 1;
				break;
			case 24:
				vSize = 2;
				break;
			case 25:
				vSize = 3;
				break;
			case 26:
				vSize = 3;
				break;
			}

			for (int v = 0; v < vSize; v++) {
				switch (type) {
				case 21:
					hSize = 3;
					break;
				case 22:
					hSize = 3;
					break;
				case 23:
					hSize = 5;
					break;
				case 24:
					hSize = 5;
					break;
				case 25:
					hSize = 5 - 2 * v;
					break;
				case 26:
					hSize = 1 + 2 * v;
					break;
				}

				for (int h = 0; h < hSize; h++) {
					int x = 0;
					int y = 0;

					if (dir == 1 || dir == 3) {
						x = h - (hSize >> 1);
						y = -1 * (1 + v) * (2 - dir);
					}
					else {
						x = (1 + v) * (1 - dir);
						y = h - (hSize >> 1);
					}

					output->emplace_back(start + Coord{ x,y });
				}
			}
		}
		inline void GetAOE_3_X(const size_t size, std::vector<Coord>* output) {
			open_set.clear();			
			close_set.clear();
			cur_set.clear();

			cur_set.emplace_back(start);

			for (size_t i = 0; i <= size; i++) {
				swap(open_set, cur_set);

				while (!open_set.empty()) {
					auto base = open_set.back();
					open_set.pop_back();

					auto find = [](std::vector<Coord>& c, Coord& p) {
						return std::ranges::find(c, p) != c.end();
					};

					if (!find(close_set,base)) {
						close_set.emplace_back(base);
					}

					for (auto& it : dirOffset) {
						auto cur = Coord{ it.x + base.x,it.y + base.y };

						if (!find(close_set, cur)) {
							cur_set.emplace_back(cur);
						}
					}
				}
			}

			for (size_t i = 1; i < close_set.size(); i++) {
				output->emplace_back(close_set[i]);
			}

			return;
		}
		inline void GetAOE_4_X(size_t size, std::vector<Coord>* output) const {
			output->emplace_back(start + Coord{ 1,-1 });
			output->emplace_back(start + Coord{ -1,-1 });
			output->emplace_back(start + Coord{ -1,1 });
			output->emplace_back(start + Coord{ 1,1 });
		}

	public:
		AOEClass(){
			open_set.reserve(RESERVE);
			close_set.reserve(RESERVE);
			cur_set.reserve(RESERVE);
		}
		~AOEClass(){

		}

		inline void GetAOE(const Coord start, const size_t dir, const size_t type, std::vector<Coord>* output) {
			output->clear();

			this->start = start;
            this->dir = ::Range(dir, 0u, 3u);

			switch (type / 10) {
			case 2:
				GetAOE_2_X(type, output);
				return;
			}

			switch (type) {
			case 11:
				GetAOE_1_X(2, output);
				return;
			case 12:
				GetAOE_1_X(4, output);
				return;
			case 31:
				GetAOE_3_X(2, output);
				return;
			case 32:
				GetAOE_3_X(3, output);
				return;
			case 41:
				GetAOE_4_X(0, output);
				return;
			}
		}
	};
}
