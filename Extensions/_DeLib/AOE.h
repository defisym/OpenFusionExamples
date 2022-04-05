#pragma once

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <vector>

// AOE
namespace AOE{
	using namespace std;

	constexpr auto RESERVE = 50;

	class AOEClass {
	public:
		struct offset {
			int x;
			int y;

			inline offset operator+(const offset& A) {
				return offset{ A.x + this->x ,A.y + this->y };
			}

			inline bool operator==(const offset& A) {
				return A.x == this->x && A.y == this->y;
			}
		};

		using coord = offset;

	private:
		const vector<offset> dirOffset = { {1, 0}, { 0,-1 }, { -1,0 }, { 0,1 } };

		coord start;
		size_t dir;

		vector<coord> open_set;
		vector<coord> close_set;

		inline void GetAOE_1_X(size_t size, vector<coord>* output) {
			for (size_t i = 0; i < size; i++) {
				output->emplace_back(start + dirOffset[dir]);
			}
		}
		inline void GetAOE_2_X(size_t type, vector<coord>* output) {
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

					output->emplace_back(start + coord{ x,y });
				}
			}
		}
		inline void GetAOE_3_X(size_t size, vector<coord>* output) {
			open_set.clear();			
			close_set.clear();

			open_set.emplace_back(start);

			for (size_t i = 0; i <= size; i++) {
				while (!open_set.empty()) {
					coord base = open_set.back();
					open_set.pop_back();

					close_set.emplace_back(base);
					output->emplace_back(start + base);

					for (auto& it : dirOffset) {
						coord cur = coord{ it.x + base.x,it.y + base.y };

						if (std::find(close_set.begin(), close_set.end(), cur) != close_set.end()) {
							open_set.emplace_back(cur);
						}
					}
				}
			}
		}
		inline void GetAOE_4_X(size_t size, vector<coord>* output) {
			output->emplace_back(start + offset{ 1,-1 });
			output->emplace_back(start + offset{ -1,-1 });
			output->emplace_back(start + offset{ -1,1 });
			output->emplace_back(start + offset{ 1,1 });
		}

	public:
		AOEClass(){
			open_set.reserve(RESERVE);
			close_set.reserve(RESERVE);
		}
		~AOEClass(){

		}

		inline void GetAOE(coord start, size_t dir, size_t type, vector<coord>* output) {
			output->clear();

			this->start = start;
			this->dir = max(0, min(3, dir));

			if (type / 10 == 2) {
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
				GetAOE_3_X(1, output);
				return;
			case 32:
				GetAOE_3_X(2, output);
				return;
			case 41:
				GetAOE_4_X(0, output);
				return;
			}
		}
	};
}