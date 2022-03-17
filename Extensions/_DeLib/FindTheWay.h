#pragma once

//進んだ道の先　光が見つかるから
//YOU'LL FIND THE WAY

#include <functional>
#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace FindTheWay {
	struct Step {
		size_t x;
		size_t y;
	};

	bool operator==(Step& A, Step& B) {
		return (A.x == B.x) && (A.y == B.y);
	}

	enum class StepPos {
		X,
		Y,
	};

	enum class MapType {
		MAP,
		TERRAIN,
		UNIT,
	};

	ostream& operator<<(ostream& out, MapType type) {
		switch (type)
		{
		case FindTheWay::MapType::MAP:
			out << "Map";
			break;
		case FindTheWay::MapType::TERRAIN:
			out << "Terrain";
			break;
		case FindTheWay::MapType::UNIT:
			out << "Unit";
			break;
		}

		return out;
	}

	using Path = vector<Step>;
	using Paths = unordered_map<wstring, Path>;
	using Heuristic = std::function<size_t(Step, Step)>;

	template<typename T>
	constexpr auto Range(T X) { return (max(0, min(255, X))); }

	constexpr auto PATH_RESERVE = 50;
	
	constexpr auto MAP_PATH = 0;
	constexpr auto MAP_OBSTACLE = 255;

	constexpr auto STEP_UNREACHABLE = 65536;
	constexpr auto STEP_GENERALCOST = 10;

	class FindTheWayClass {
	private:
		size_t width = 0;
		size_t height = 0;

		bool updateMap = false;

		BYTE* map = nullptr;
		BYTE* terrain = nullptr;
		BYTE* unit = nullptr;

		bool pathAvailable = false;
		Path path;

		Paths savedPath;

		inline size_t pOffset(size_t x, size_t y) {
			return y * width + x;
		}

		inline bool PosValid(size_t x, size_t y) {			
			return (x < width) && (y < height) && ((y * width + x) < (width * height));
		}

		inline BYTE* GetMapPointer(MapType type) {
			switch (type)
			{
			case FindTheWay::MapType::MAP:
				return map;
				break;
			case FindTheWay::MapType::TERRAIN:
				return terrain;
				break;
			case FindTheWay::MapType::UNIT:
				return unit;
				break;
			default:
				return nullptr;
				break;
			}
		}

		inline BYTE* GetMapPosPointer(size_t x, size_t y, BYTE* pMap) {
			if (!PosValid(x,y)) {
				return nullptr;
			}

			return pMap + y * width + x;
		}

		inline BYTE* GetMapPosPointer(size_t x, size_t y, MapType type) {
			if (!PosValid(x, y)) {
				return nullptr;
			}

			BYTE* pMap = GetMapPointer(type);

			if (pMap == nullptr) {
				return nullptr;
			}

			return pMap + y * width + x;
		}

	public:
		FindTheWayClass(size_t width, size_t height) {
			path.reserve(PATH_RESERVE);

			this->width = width;
			this->height = height;

			size_t size = width * height;

			map = new BYTE[size];
			terrain = new BYTE[size];
			unit = new BYTE[size];

			memset(map, 0, size);
			memset(terrain, 0, size);
			memset(unit, 0, size);
		}
		~FindTheWayClass() {
			Reset();

			delete[] map;
			delete[] terrain;
			delete[] unit;

			map = nullptr;
			terrain = nullptr;
			unit = nullptr;
		}

		inline void Reset() {
			path.clear();

			size_t size = width * height;

			memset(map, 0, size);
			memset(terrain, 0, size);
			memset(unit, 0, size);

			updateMap = true;
			pathAvailable = false;
		}

		inline void ClearMap(MapType type) {
			BYTE* pMap = GetMapPointer(type);
			size_t size = width * height;

			memset(pMap, 0, size);
		}

		inline void SetMap(size_t x, size_t y, BYTE cost, MapType type) {
			BYTE* pMapPos = GetMapPosPointer(x, y, type);

			if (pMapPos == nullptr) {
				return;
			}

			*pMapPos = Range(cost);
			
			updateMap = true;
		}

		inline BYTE GetMap(size_t x, size_t y, MapType type) {
			BYTE* pMapPos = GetMapPosPointer(x, y, type);

			if (pMapPos == nullptr) {
				return MAP_OBSTACLE;
			}
			else {
				return *pMapPos;
			}
		}

		inline bool PosObstacle(size_t x, size_t y, MapType type) {
			return GetMap(x, y, type) == MAP_OBSTACLE;
		}

		inline void UpdateMap() {
			size_t size = width * height;
			for (size_t i = 0; i < size; i++) {
				*(map + i) = Range(*(terrain + i) + *(unit + i));
			}

			updateMap = false;
		}

		inline bool PathAvailable() {
			return pathAvailable;
		}

		inline size_t GetDistance() {
			return pathAvailable ? path.size() : STEP_UNREACHABLE;
		}

		inline size_t GetStepAt(size_t pos, StepPos type) {
			return type == StepPos::X ? path.at(pos).x : path.at(pos).y;
		}

		inline size_t GetWidth() {
			return width;
		}

		inline size_t GetHeight() {
			return height;
		}

		inline void OutPutMap(MapType type) {
			cout << "MapType : " << type << endl;
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {					
					cout << (((*GetMapPosPointer(x, y, type)) == MAP_PATH) ? '#' : '*' )<< ' ';
				}
				cout << endl;
			}
		}

		inline void SavePath(wstring name) {
			if (!pathAvailable) {
				return;
			}

			savedPath[name] = path;
		}

		static inline size_t GetEuclideanDistancePow(Step start, Step destination) {
			return (size_t)(pow(start.x - destination.x, 2) + pow(start.y - destination.y, 2));
		}

		static inline size_t GetEuclideanDistance(Step start, Step destination) {
			return (size_t)sqrt(GetEuclideanDistancePow(start, destination));
		}

		static inline size_t GetManhattanDistance(Step start, Step destination) {
			return abs((int)(start.x - destination.x)) + abs((int)(start.y - destination.y));
		}

		inline bool PointObstacle(Step p) {
			return PosObstacle(p.x, p.y, MapType::MAP);
		}

		inline void Find(Step start, Step destination, Heuristic h = FindTheWayClass::GetManhattanDistance) {
			//*初始化open_set和close_set；
			//* 将起点加入open_set中，并设置优先级为0（优先级最高）；
			//* 如果open_set不为空，则从open_set中选取优先级最高的节点n：
			//	* 如果节点n为终点，则：
			//		* 从终点开始逐步追踪parent节点，一直达到起点；
			//		* 返回找到的结果路径，算法结束；
			//	* 如果节点n不是终点，则：
			//		* 将节点n从open_set中删除，并加入close_set中；
			//		* 遍历节点n所有的邻近节点：
			//		* 如果邻近节点m在close_set中，则：
			//			* 跳过，选取下一个邻近节点
			//		* 如果邻近节点m不在open_set中，则：
			//			* 设置节点m的parent为节点n
			//			* 计算节点m的优先级
			//			* 将节点m加入open_set中
			//		* 如果邻近节点m在open_set中且优先级更低，则：
			//			* 设置节点m的parent为节点n
			//			* 计算节点m的优先级

			struct Point {
				Step step;

				Point* parent = nullptr;
				
				int priority;
				size_t curStep;
			};
			
			using Set = vector<Point>;

			Set open_set;
			Set close_set;
			Set point_set;

			open_set.reserve(2 * PATH_RESERVE);
			close_set.reserve(2 * PATH_RESERVE);
			point_set.reserve(2 * PATH_RESERVE);

			if (updateMap) {
				UpdateMap();
			}

			pathAvailable = false;
			path.clear();

			if (PointObstacle(start) || PointObstacle(destination)) {
				return;
			}

			open_set.emplace_back(Point{ start,nullptr,0,0 });
			
			while (!open_set.empty()) {
				std::sort(open_set.begin(), open_set.end(), [](Point A, Point B) ->bool { return A.priority > B.priority; });				
				
				if (open_set.back().step == destination) {
					Point* parent = &open_set.back();

					while (parent != nullptr) {
						path.emplace_back((*parent).step);
						parent = (*parent).parent;
					}

					pathAvailable = true;

					return;
				}
				else {
					Point base = open_set.back();

					close_set.emplace_back(base);
					open_set.erase(open_set.cend() - 1);

					auto find = [](Set& v, Point& p)->Point* {
						//return std::find(v.begin(), v.end(), p) != v.end();
						for (auto& it : v) {
							if (it.step == p.step) {
								return &it;
							}
						}

						return nullptr;
					};

					struct offset {
						char x;
						char y;
					};

					const vector<offset> neighbour = { {-1,0},{1,0},{0,-1},{0,1} };

					for (auto& it : neighbour) {
						Point neighPoint = Point{ 
							Step{(size_t)(base.step.x + it.x)
							,(size_t)(base.step.y + it.y)}
							, nullptr, 0, base.curStep + 1
							};

						if (PointObstacle(neighPoint.step)) {
							continue;
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						auto heuristic = [&](Point& p)->size_t {return h(neighPoint.step, destination); };
						auto updateParentPointer = [&](Point& cur)->void {
							Point* p = find(point_set, cur);

							if (p == nullptr) {
								point_set.emplace_back(base);
								cur.parent = &point_set.back();
							}
							else {
								cur.parent = p;
							}							
						};

						Point* next = find(open_set, neighPoint);

						if (next == nullptr) {							
							neighPoint.priority = heuristic(neighPoint) + neighPoint.curStep * STEP_GENERALCOST;
							updateParentPointer(neighPoint);							
							
							open_set.emplace_back(neighPoint);
						}
						else if (next->curStep > neighPoint.curStep) {
							next->curStep = neighPoint.curStep;
							next->priority = heuristic(neighPoint) + neighPoint.curStep * STEP_GENERALCOST;

							updateParentPointer(*next);
						}
					}
				}
			}
		}
	};
}
