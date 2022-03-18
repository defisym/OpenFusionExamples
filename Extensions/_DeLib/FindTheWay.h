#pragma once

//進んだ道の先　光が見つかるから
//YOU'LL FIND THE WAY

#include <functional>
#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace FindTheWay {
	struct Coord {
		size_t x;
		size_t y;
	};

	bool operator==(const Coord& A, const Coord& B) {
		return (A.x == B.x) && (A.y == B.y);
	}

	enum class CoordType {
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

	using Path = vector<Coord>;
	using Paths = unordered_map<wstring, Path>;
	using Heuristic = std::function<size_t(Coord, Coord)>;

	template<typename T>
	constexpr auto Range(T X) { return (max(0, min(255, X))); }

	constexpr auto PATH_RESERVE = 50;
	constexpr auto SAVEDPATH_RESERVE = 10;
	
	constexpr auto MAP_PATH = 0;
	constexpr auto MAP_OBSTACLE = 255;

	constexpr auto STEP_UNREACHABLE = 65536;
	constexpr auto STEP_GENERALCOST = 10;

	constexpr auto COORD_INVALID = 65536;

	class FindTheWayClass {
	private:
		size_t width = 0;
		size_t height = 0;

		size_t mapSize = 0;
		size_t girdSize = 1;

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
			savedPath.reserve(SAVEDPATH_RESERVE);

			this->width = width;
			this->height = height;

			this->mapSize = width * height;

			map = new BYTE[mapSize];
			terrain = new BYTE[mapSize];
			unit = new BYTE[mapSize];

			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(unit, 0, mapSize);
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
			savedPath.clear();

			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(unit, 0, mapSize);

			updateMap = true;
			pathAvailable = false;
		}

		inline size_t GetGirdCoord(size_t realCoord) {
			return realCoord / girdSize;
		}

		inline size_t GetRealCoord(size_t girdCoord) {
			return girdCoord * girdSize + (size_t)(girdSize / 2);
		}

		inline size_t GetGirdSize() {
			return girdSize;
		}

		inline void SetGirdSize(size_t girdSize) {
			this->girdSize = girdSize;
		}

		inline void ClearMap(MapType type) {
			BYTE* pMap = GetMapPointer(type);
			mapSize = width * height;

			memset(pMap, 0, mapSize);

			updateMap = true;
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

		inline void UpdateMap() {			
			for (size_t i = 0; i < mapSize; i++) {
				*(map + i) = Range(*(terrain + i) + *(unit + i));
			}

			updateMap = false;
		}

		inline bool CoordObstacle(size_t x, size_t y, MapType type) {
			return GetMap(x, y, type) == MAP_OBSTACLE;
		}

		inline Path* GetPathPointer(wstring* name = nullptr) {
			return ((name != nullptr) && (savedPath.count(*name) != 0) ? &savedPath[*name] : &path); 
		}

		inline bool GetPathValidity(wstring* name = nullptr) {
			return ((name == nullptr) && this->pathAvailable) || ((name != nullptr) && (savedPath.count(*name) != 0));
		}

		inline size_t GetDistance(wstring* name = nullptr) {
			Path* pPath = GetPathPointer(name);
			bool curPathAvaliable = GetPathValidity(name);

			return curPathAvaliable ? pPath->size() : STEP_UNREACHABLE;
		}

		inline size_t GetCoordAtPath(size_t pos, CoordType type, wstring* name = nullptr) {
			Path* pPath = GetPathPointer(name);
			bool curPathAvaliable = GetPathValidity(name);

			if (!curPathAvaliable) {
				return COORD_INVALID;
			}

			return type == CoordType::X ? pPath->at(pos).x : pPath->at(pos).y;
		}

		inline size_t GetWidth() {
			return width;
		}

		inline size_t GetHeight() {
			return height;
		}

		inline void OutPutMap(MapType type) {
			auto find = [](Path& path, Coord& coord)->bool {
				return std::find(path.begin(), path.end(), coord) != path.end();
			};

			cout << "MapType : " << type << endl;
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					Coord cur = { x,y };

					if (find(path, cur)) {
						cout << '-';
					}
					else {
						cout << (((*GetMapPosPointer(x, y, type)) == MAP_PATH) ? '#' : '*');
					}
					cout << ' ';
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

		static inline size_t GetEuclideanDistancePow(Coord start, Coord destination) {
			return (size_t)(pow(start.x - destination.x, 2) + pow(start.y - destination.y, 2));
		}

		static inline size_t GetEuclideanDistance(Coord start, Coord destination) {
			return (size_t)sqrt(GetEuclideanDistancePow(start, destination));
		}

		static inline size_t GetManhattanDistance(Coord start, Coord destination) {
			return abs((int)(start.x - destination.x)) + abs((int)(start.y - destination.y));
		}

		inline bool PointObstacle(Coord p) {
			return CoordObstacle(p.x, p.y, MapType::MAP);
		}

		inline void Find(Coord start, Coord destination, Heuristic h = FindTheWayClass::GetManhattanDistance) {
			//A*寻路
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
				Coord coord;

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
				
				if (open_set.back().coord == destination) {
					Point* parent = &open_set.back();

					while (parent != nullptr) {
						path.emplace_back((*parent).coord);
						parent = (*parent).parent;
					}

					std::reverse(path.begin(), path.end());
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
							if (it.coord == p.coord) {
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
							Coord{(size_t)(base.coord.x + it.x)
							,(size_t)(base.coord.y + it.y)}
							, nullptr, 0, base.curStep + 1
							};

						if (PointObstacle(neighPoint.coord)) {
							continue;
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						auto heuristic = [&](Point& p)->size_t {return h(neighPoint.coord, destination); };
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
