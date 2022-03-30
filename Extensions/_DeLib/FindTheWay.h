#pragma once

// 進んだ道の先　光が見つかるから
// YOU'LL FIND THE WAY

#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <tuple>

#include <sstream>

#include <unordered_map>
#include <map>

#include <iostream>

#include "Base64.h"
#include "StrNum.h"

using namespace std;

namespace FindTheWay {
	using  BYTE = unsigned char;

	struct Coord {
		size_t x;
		size_t y;
	};

	inline bool operator<(const Coord& A, const Coord& B) {
		return (A.x < B.x) && (A.y < B.y);
	}

	inline bool operator==(const Coord& A, const Coord& B) {
		return (A.x == B.x) && (A.y == B.y);
	}

	inline Coord operator+(const Coord& A, const Coord& B) {
		return Coord{ A.x + B.x ,A.y + B.y };
	}

	inline Coord operator+(const Coord& A, const size_t& B) {
		return Coord{ A.x + B ,A.y + B };
	}

	inline Coord operator*(const Coord& A, const size_t& B) {
		return Coord{ A.x * B ,A.y * B };
	}

	inline Coord operator/(const Coord& A, const size_t& B) {
		return Coord{ A.x / B ,A.y / B };
	}

	enum class CoordType {
		X,
		Y,
	};

	enum class MapType {
		MAP,
		TERRAIN,
		DYNAMIC,
	};

	constexpr auto MAPTYPENUM = 3;

	inline ostream& operator<<(ostream& out, MapType type) {
		switch (type)
		{
		case FindTheWay::MapType::MAP:
			out << "Map";
			break;
		case FindTheWay::MapType::TERRAIN:
			out << "Terrain";
			break;
		case FindTheWay::MapType::DYNAMIC:
			out << "Dynamic";
			break;
		}

		return out;
	}

	inline wostream& operator<<(wostream& out, MapType type) {
		switch (type)
		{
		case FindTheWay::MapType::MAP:
			out << L"Map";
			break;
		case FindTheWay::MapType::TERRAIN:
			out << L"Terrain";
			break;
		case FindTheWay::MapType::DYNAMIC:
			out << L"Dynamic";
			break;
		}

		return out;
	}

	using Path = vector<Coord>;
	using Paths = unordered_map<wstring, Path>;
	using Heuristic = std::function<size_t(Coord, Coord)>;

	using Area = vector<vector<Coord>>;

	using Exception = unsigned char;

	// try {
	// 	// Invalid Size
	// 	FindTheWayClass* pFTW = new FindTheWayClass(0, 0);
	// }
	// catch (FindTheWay::Exception& e) {
	// 	// Do something...
	// 	// cout << e << endl;
	// }

	// Map size invalid
	constexpr unsigned char INVALID_SIZE = 0;
	// Map data invalid, e.g. non-base64 string
	constexpr unsigned char INVALID_DATA = 1;
	// Base64 encode failed
	constexpr unsigned char BASE64_FAILED = 2;

	inline ostream& operator<<(ostream& out, Exception type) {
		switch (type)
		{
		case INVALID_SIZE:
			out << "INVALID_SIZE";
			break;
		case INVALID_DATA:
			out << "INVALID_DATA";
			break;
		case BASE64_FAILED:
			out << "BASE64_FAILED";
			break;
		}

		return out;
	}

	inline wostream& operator<<(wostream& out, Exception type) {
		switch (type)
		{
		case INVALID_SIZE:
			out << L"INVALID_SIZE";
			break;
		case INVALID_DATA:
			out << L"INVALID_DATA";
			break;
		case BASE64_FAILED:
			out << L"BASE64_FAILED";
			break;
		}

		return out;
	}

	constexpr auto PATH_RESERVE = 50;
	constexpr auto SAVEDPATH_RESERVE = 10;

	constexpr auto AREA_RESERVE = 50;

	constexpr auto MAP_PATH = 0;
	constexpr auto MAP_OBSTACLE = 255;

	template <typename T>
	constexpr auto Range(T X) { return (max(MAP_PATH, min(MAP_OBSTACLE, X))); }

	constexpr auto STEP_UNREACHABLE = 65536;

	constexpr auto STEP_GENERALCOST_NORMAL = 10;
	constexpr auto STEP_GENERALCOST_DIAGONAL = 14;	// 1.414

	constexpr auto COORD_INVALID = 65536;
	constexpr auto REAL_INVALID = 2147483647;

	constexpr auto DELIMETER = ':';

	class FindTheWayClass {
	private:
		size_t width = 0;
		size_t height = 0;

		size_t mapSize = 0;

		size_t girdSize = 1;
		size_t girdOffestX = 0;
		size_t girdOffestY = 0;

		bool updateMap = false;

		BYTE* map = nullptr;
		BYTE* terrain = nullptr;
		BYTE* dynamic = nullptr;

		//BYTE* pMap[MAPTYPENUM];

		struct Offset {
			char x;
			char y;
			char generalCost;
		};

		// Neighbour offset
		//| [ -1 , -1 ] | [ 0 , -1 ] | [ 1 , -1 ] |
		//| [ -1 , 0 ]  | [ 0 , 0 ]  | [ 1 , 0 ]  |
		//| [ -1 , 1 ]  | [ 0 , 1 ]  | [ 1 , 1 ]  |

		using Neighbour = vector<Offset>;

		const Neighbour normalNeighbour = { {1,0,STEP_GENERALCOST_NORMAL}
										,{0,1,STEP_GENERALCOST_NORMAL}
										,{-1,0,STEP_GENERALCOST_NORMAL}
										,{0,-1,STEP_GENERALCOST_NORMAL} };

		const Neighbour diagonalNeighbour = { {1,0,STEP_GENERALCOST_NORMAL}
										,{1,1,STEP_GENERALCOST_DIAGONAL}		// diagonal
										,{0,1,STEP_GENERALCOST_NORMAL}
										,{-1,1,STEP_GENERALCOST_DIAGONAL}		// diagonal
										,{-1,0,STEP_GENERALCOST_NORMAL}
										,{-1,-1,STEP_GENERALCOST_DIAGONAL}		// diagonal
										,{0,-1,STEP_GENERALCOST_NORMAL}
										,{1,-1,STEP_GENERALCOST_DIAGONAL} };	// diagonal

		bool pathAvailable = false;
		Path path;

		Paths savedPath;

		using StashPathKey = tuple<Coord, Coord>;

		struct StashPathKeyCompare {
			bool operator()(StashPathKey lKey, StashPathKey rKey)  const noexcept {
				auto& [lKeyS, lKeyD] = lKey;
				auto& [rKeyS, rKeyD] = rKey;

				return (lKeyS < rKeyS) && (lKeyD < rKeyD);
			};
		};

		using StashPath = std::map<StashPathKey, Path, StashPathKeyCompare>;

		StashPath stashPath;

		struct Point {
			Coord coord;

			Point* parent = nullptr;

			int priority;
			size_t cost;
		};

		using Set = vector<Point>;

		Set open_set;
		Set close_set;
		Set point_set;

		Base64<wstring> base64;

		Area area;
		Set cur_set;
		Set continue_set;

		vector<size_t> ranges;

		using StashAreaKey = tuple<Coord, size_t, size_t, size_t>;

		struct StashAreaKeyCompare {
			bool operator()(StashAreaKey lKey, StashAreaKey rKey)  const noexcept {
				auto& [lc, lr, lsr, lm] = lKey;
				auto& [rc, rr, rsr, rm] = rKey;

				return (lc < rc) && (lr < rr) && (lsr < rsr) && (lm < rm);
			};
		};

		using StashArea = std::map<StashAreaKey, Area, StashAreaKeyCompare>;

		StashArea stashArea;


		inline size_t pOffset(size_t x, size_t y) {
			return y * width + x;
		}

		inline bool PosValid(size_t x, size_t y) {
			return (x < width) && (y < height) && ((y * width + x) < (mapSize));
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
			case FindTheWay::MapType::DYNAMIC:
				return dynamic;
				break;
			default:
				return nullptr;
				break;
			}
		}

		//inline BYTE* _GetMapPointer(MapType type) {
		//	auto pos = (int)type;		

		//	return pos < MAPTYPENUM ? *(pMap+pos) : nullptr;
		//}

		inline BYTE* GetMapPosPointer(size_t x, size_t y, BYTE* pMap) {
			if (pMap == nullptr || !PosValid(x, y)) {
				return nullptr;
			}

			return pMap + y * width + x;
		}

		inline BYTE* GetMapPosPointer(size_t x, size_t y, MapType type) {
			return GetMapPosPointer(x, y, GetMapPointer(type));
		}

		inline void ClearMap(BYTE* pMap, BYTE cost = MAP_PATH, bool needUpdate = true) {
			memset(pMap, cost, mapSize);

			updateMap = needUpdate;
		}

		inline void SetMap(size_t x, size_t y, BYTE cost, BYTE* pMap, bool needUpdate = true) {
			BYTE* pMapPos = GetMapPosPointer(x, y, pMap);

			if (pMapPos == nullptr) {
				return;
			}

			*pMapPos = Range(cost);

			updateMap = needUpdate;
		}

		inline BYTE GetMap(size_t x, size_t y, BYTE* pMap) {
			BYTE* pMapPos = GetMapPosPointer(x, y, pMap);

			if (pMapPos == nullptr) {
				return MAP_OBSTACLE;
			}
			else {
				return *pMapPos;
			}
		}

		inline Path* GetPathPointer(wstring* name = nullptr) {
			return ((name != nullptr) && (savedPath.count(*name) != 0) ? &savedPath[*name] : &path);
		}

		inline void Reserve() {
			path.reserve(PATH_RESERVE);
			savedPath.reserve(SAVEDPATH_RESERVE);

			open_set.reserve(2 * PATH_RESERVE);
			close_set.reserve(2 * PATH_RESERVE);
			point_set.reserve(8 * this->mapSize);

			area.reserve(AREA_RESERVE);
			cur_set.reserve(2 * PATH_RESERVE);
			continue_set.reserve(2 * PATH_RESERVE);
		}

		inline void Allocate(size_t width, size_t height) {
			this->width = width;
			this->height = height;

			this->mapSize = width * height;

			if (mapSize == 0) {
				throw INVALID_SIZE;
			}

			map = new BYTE[mapSize];
			terrain = new BYTE[mapSize];
			dynamic = new BYTE[mapSize];

			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(dynamic, 0, mapSize);

			//pMap[(int)MapType::MAP] = map;
			//pMap[(int)MapType::TERRAIN] = terrain;
			//pMap[(int)MapType::DYNAMIC] = dynamic;
		}

		inline void Free() {
			delete[] map;
			delete[] terrain;
			delete[] dynamic;

			map = nullptr;
			terrain = nullptr;
			dynamic = nullptr;
		}

	public:
		FindTheWayClass(size_t width, size_t height) {
			Allocate(width,height);
			Reserve();
		}
		FindTheWayClass(const wstring& base64) {
			SetMap(base64);
			Reserve();
		}

		~FindTheWayClass() {
			Free();
		}

		inline void Reset() {
			path.clear();
			savedPath.clear();
			stashPath.clear();

			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(dynamic, 0, mapSize);

			updateMap = true;
			pathAvailable = false;
		}

		inline void SetGirdSize(size_t girdSize = 1, size_t girdOffestX = 0, size_t girdOffestY = 0) {
			this->girdSize = girdSize ? girdSize : 1;		// Protection for 0

			this->girdOffestX = girdOffestX;
			this->girdOffestY = girdOffestY;
		}

		inline Coord GetGirdCoord(const Coord& realCoord) {
			return realCoord / girdSize;
		}

		inline Coord GetRealCoord(const Coord& girdCoord) {
			return girdCoord * girdSize + (size_t)(girdSize >> 1);	// girdCoord * girdSize + (size_t)(girdSize / 2);
		}

		inline size_t GetGirdSize() {
			return girdSize;
		}

		inline void ClearMap(MapType type, BYTE cost = MAP_PATH, bool needUpdate = true) {
			ClearMap(GetMapPointer(type), cost, needUpdate);
		}

		inline void SetMap(size_t x, size_t y, BYTE cost, MapType type, bool needUpdate = true) {
			SetMap(x, y, cost, GetMapPointer(type), needUpdate);
		}

		inline void SetMap(const wstring& base64) {
			// Format: width:height:terrain:dynamic
			size_t start = 0;
			size_t end = 0;

			auto GetSubStr = [base64](size_t& start, size_t& end)->wstring {
				if (end != wstring::npos) {
					end = base64.find(DELIMETER, start);
					wstring result = base64.substr(start, end - start);

					start = end + 1;

					return result;
				}
				else {
					return wstring();
				}
			};

			auto width = _stoi(GetSubStr(start, end));
			auto height = _stoi(GetSubStr(start, end));

			Free();
			Allocate(width, height);
			
			try {
				this->base64.base64_decode_to_pointer(GetSubStr(start, end), terrain, mapSize);
				this->base64.base64_decode_to_pointer(GetSubStr(start, end), dynamic, mapSize);
			}
			catch (decltype(BASE64_DECODEERROR)) {
				throw INVALID_DATA;
			}

			//DWORD dwNeed = mapSize;

			//if (!CryptStringToBinary(GetSubStr(start, end).c_str(), 0, CRYPT_STRING_BASE64, terrain, &dwNeed, NULL, NULL)) {
			//	throw INVALID_DATA;
			//}
			//if (!CryptStringToBinary(GetSubStr(start, end).c_str(), 0, CRYPT_STRING_BASE64, dynamic, &dwNeed, NULL, NULL)) {
			//	throw INVALID_DATA;
			//}

			UpdateMap();
		}

		inline BYTE GetMap(size_t x, size_t y, MapType type, bool forceUpdate = true) {
			if (forceUpdate && updateMap && type == MapType::MAP) {
				UpdateMap();
			}

			return GetMap(x, y, GetMapPointer(type));
		}

		inline wstring GetMap() {
			wstring base64;

			base64 += _itos(width).c_str();
			base64 += DELIMETER;
			base64 += _itos(height).c_str();

			base64 += this->base64.base64_encode(terrain, mapSize);
			base64 += this->base64.base64_encode(dynamic, mapSize);

			return base64;
		}

		inline void UpdateMap() {
			for (size_t i = 0; i < mapSize; i++) {
				*(map + i) = Range(*(terrain + i) + *(dynamic + i));
			}

			updateMap = false;

			// Path invalidation when map changes, clear path
			path.clear();
			savedPath.clear();
			stashPath.clear();

			area.clear();
			stashArea.clear();

			pathAvailable = false;
		}

		inline bool CoordObstacle(size_t x, size_t y, MapType type) {
			return GetMap(x, y, type) == MAP_OBSTACLE;
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

			if (!curPathAvaliable || pPath->size() <= pos) {
				return COORD_INVALID;
			}

			return type == CoordType::X ? (*pPath)[pos].x : (*pPath)[pos].y;
		}

		inline size_t GetWidth() {
			return width;
		}

		inline size_t GetHeight() {
			return height;
		}

		inline void OutPutMap(MapType type = MapType::MAP) {
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
						cout << (((*GetMapPosPointer(x, y, type)) != MAP_OBSTACLE) ? '#' : '*');
					}
					cout << ' ';
				}
				cout << endl;
			}
		}

		inline wstring OutPutMapStr(MapType type = MapType::MAP, bool showPath = true, wstring* name = nullptr) {
			auto find = [](const Path& path, const Coord& coord)->bool {
				return std::find(path.begin(), path.end(), coord) != path.end();
			};
			
			std::wstringstream ss;

			ss << L"MapType : " << type << endl;
			ss << L"Obstacle = \'*\', Path = \'#\'" << endl;

			Path* pPath = nullptr;
			bool curPathAvaliable = false;

			if (showPath) {
				pPath = GetPathPointer(name);
				curPathAvaliable = GetPathValidity(name);

				if (curPathAvaliable) {
					wstring displayName = name == nullptr ? L"Last Path" : *name;
					ss << "Step of \"" << displayName << "\" = \'-\'" << endl;
				}
				else if (name != nullptr) {
					ss << "Target Path \"" << *name << "\" Invalid" << endl;
				}
			}

			ss << L"----------" << endl;

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					if (showPath && curPathAvaliable && find(*pPath, Coord{ x,y })) {
						ss << L'-';
					}
					else {
						ss << (((*GetMapPosPointer(x, y, type)) != MAP_OBSTACLE) ? L'#' : L'*');
					}
					ss << ' ';
				}
				ss << endl;
			}

			return ss.str();
		}

		inline void SaveLastPath(wstring name) {
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

		inline BYTE GetPointCost(Coord p) {
			return GetMap(p.x, p.y, MapType::MAP);
		}

		inline void ForceFind(Coord start, Coord destination, bool diagonal = false, bool checkDiagonalCorner = true
			, const Heuristic& h = FindTheWayClass::GetManhattanDistance) {
			// Still try to find a way even start & destination is obstacle
			// ......By setting them to path
			if (updateMap) {
				UpdateMap();
			}

			BYTE startCost = GetMap(start.x, start.y, this->map);
			BYTE destinationCost = GetMap(destination.x, destination.y, this->map);

			SetMap(start.x, start.y, MAP_PATH, this->map, false);
			SetMap(destination.x, destination.y, MAP_PATH, this->map, false);

			Find(start, destination, diagonal, checkDiagonalCorner, h);

			SetMap(start.x, start.y, startCost, this->map, false);
			SetMap(destination.x, destination.y, destinationCost, this->map, false);
		}

		inline void Find(Coord start, Coord destination, bool diagonal = false, bool checkDiagonalCorner = true
			, const Heuristic& h = FindTheWayClass::GetManhattanDistance) {
			// A*寻路
			// https://www.redblobgames.com/pathfinding/a-star/introduction.html
			// *初始化open_set和close_set；
			// * 将起点加入open_set中，并设置优先级为0（优先级最高）；
			// * 如果open_set不为空，则从open_set中选取优先级最高的节点n：
			// 	* 如果节点n为终点，则：
			// 		* 从终点开始逐步追踪parent节点，一直达到起点；
			// 		* 返回找到的结果路径，算法结束；
			// 	* 如果节点n不是终点，则：
			// 		* 将节点n从open_set中删除，并加入close_set中；
			// 		* 遍历节点n所有的邻近节点：
			// 		* 如果邻近节点m在close_set中，则：
			// 			* 跳过，选取下一个邻近节点
			// 		* 如果邻近节点m不在open_set中，则：
			// 			* 设置节点m的parent为节点n
			// 			* 计算节点m的优先级
			// 			* 将节点m加入open_set中
			// 		* 如果邻近节点m在open_set中且优先级更低，则：
			// 			* 设置节点m的parent为节点n
			// 			* 计算节点m的优先级

			if (updateMap) {
				UpdateMap();
			}

			auto stashPathKey = make_tuple(start, destination);
			pathAvailable = stashPath.count(stashPathKey);

			if (pathAvailable) {
				path = stashPath[stashPathKey];

				return;
			}

			path.clear();

			if ((GetMap(start.x, start.y, this->map) == MAP_OBSTACLE)
				|| (GetMap(destination.x, destination.y, this->map) == MAP_OBSTACLE)) {
				return;
			}

			open_set.clear();
			close_set.clear();
			point_set.clear();

			open_set.emplace_back(Point{ start,nullptr,0,0 });

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;
			auto neighbourSize = pNeighbour->size();

			while (!open_set.empty()) {
				// Descending
				std::sort(open_set.begin(), open_set.end(), [](Point A, Point B) ->bool { return A.priority > B.priority; });

				if (open_set.back().coord == destination) {
					Point* parent = &open_set.back();

					while (parent != nullptr) {
						path.emplace_back((*parent).coord);
						parent = (*parent).parent;
					}

					std::reverse(path.begin(), path.end());
					pathAvailable = true;

					stashPath[stashPathKey] = path;

					return;
				}
				else {
					Point base = open_set.back();
					open_set.pop_back();

					close_set.emplace_back(base);					

					auto find = [](Set& v, Point& p)->Point* {
						auto it = std::find_if(v.begin(), v.end(), [&p](Point& it)->bool { return it.coord == p.coord; });
						return it != v.end() ? &(*it) : nullptr;
					};
					auto rfind = [](Set& v, Point& p)->Point* {
						auto it = std::find_if(v.rbegin(), v.rend(), [&p](Point& it)->bool { return it.coord == p.coord; });
						return it != v.rend() ? &(*it) : nullptr;
					};

					for (size_t i = 0; i < neighbourSize; i++) {
						Coord neighCoord = Coord{ (size_t)(base.coord.x + (*pNeighbour)[i].x)
						,(size_t)(base.coord.y + (*pNeighbour)[i].y) };

						BYTE curCost = GetMap(neighCoord.x, neighCoord.y, this->map);
						Point neighPoint = Point{ neighCoord, nullptr, 0, base.cost + curCost + (*pNeighbour)[i].generalCost };

						if (curCost == MAP_OBSTACLE) {
							continue;
						}

						if (diagonal && checkDiagonalCorner		// check corner
							&&(i % 2 == 1)) {					// when checking diagonal points
							size_t prev = i - 1;
							size_t next = (i + 1) % neighbourSize;

							if ((GetMap(base.coord.x + (*pNeighbour)[prev].x
									, base.coord.y + (*pNeighbour)[prev].y
									, this->map) == MAP_OBSTACLE)
							&& (GetMap(base.coord.x + (*pNeighbour)[next].x
								, base.coord.y + (*pNeighbour)[next].y
								, this->map) == MAP_OBSTACLE)) {
								continue;
							}
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						auto heuristic = [&](Point& p)->size_t { return h(neighPoint.coord, destination); };
						auto updateParentPointer = [&](Point& cur)->void {
							Point* p = find(point_set, cur);

							if (p == nullptr) {
								//point_set.emplace_back(base);
								//cur.parent = &point_set.back();

								Point* pBase = find(point_set, base);

								if (pBase == nullptr) {
									point_set.emplace_back(base);
									cur.parent = &point_set.back();
								}
								else {
									cur.parent = pBase;
								}
							}
							else {
								cur.parent = p;
							}
						};

						Point* next = find(open_set, neighPoint);

						if (next == nullptr) {
							neighPoint.priority = heuristic(neighPoint) + neighPoint.cost;
							updateParentPointer(neighPoint);

							open_set.emplace_back(neighPoint);
						}
						else if (next->cost > neighPoint.cost) {
							next->cost = neighPoint.cost;
							next->priority = heuristic(neighPoint) + neighPoint.cost;

							updateParentPointer(*next);
						}
					}
				}
			}
		}

		inline void GenerateZoc(Coord start, Path* zoc, bool diagonal = true) {
			zoc->clear();

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;

			for (auto& it : *pNeighbour) {
				zoc->emplace_back(Coord{ (size_t)(start.x + it.x),(size_t)(start.y + it.y) });
			}
		}

		inline size_t GetMoveableRange(Coord start, size_t range, size_t dir) {
			if (updateMap) {
				UpdateMap();
			}

			auto offset = normalNeighbour[dir];
			auto coord = start;
			size_t moveable = 0;

			while ((moveable <= range) && (GetMap(coord.x, coord.y, this->map) != MAP_OBSTACLE)) {
				coord.x += offset.x;
				coord.y += offset.y;
				
				moveable++;
			}

			moveable--;			// remove last while add

			return moveable;
		}

		inline void GetLineArea(Coord start, size_t range, size_t startRange = 0) {
			if (updateMap) {
				UpdateMap();
			}

			auto stashAreaKey = make_tuple(start, range, startRange, 1);

			if (stashArea.count(stashAreaKey)) {
				area = stashArea[stashAreaKey];

				return;
			}

			area.clear();

			if (GetMap(start.x, start.y, this->map) == MAP_OBSTACLE) {
				return;
			}

			ranges = { 0,0,0,0 };

			for (size_t i = 0; i < 4; i++) {
				ranges[i] = GetMoveableRange(start, range, i);
			}

			for (size_t nest = 0; nest <= range; nest++) {
				if (!(nest > startRange)) {
					continue;
				}

				area.emplace_back();
				area.back().reserve(4);

				bool updated = false;
				for (size_t it = 0; it < ranges.size(); it++) {
					if (ranges[it] >= nest) {
						area.back().emplace_back(Coord{ start.x + nest * normalNeighbour[it].x
													,start.y + nest * normalNeighbour[it].y });

						updated = true;
					}
				}

				if (!updated) {
					break;
				}
			}

			stashArea[stashAreaKey] = area;

#ifdef _DEBUG
			std::wstringstream ss;

			BYTE* temp = new BYTE[mapSize];
			memcpy(temp, map, mapSize);

			auto out = [&](BYTE cost)->wchar_t {
				switch (cost) {
				case MAP_OBSTACLE:
					return L'O';
				case 254:
					return L'+';
				case 125:
					return L'M';
				case 124:
					return L'A';
				case 50:
					return L'Z';
				default:
					return L'P';
				}
			};

			for (auto& it : area) {
				for (auto& it_C : it) {
					*GetMapPosPointer(it_C.x, it_C.y, temp) = 124;
				}
			}

			*GetMapPosPointer(start.x, start.y, temp) = 254;

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					ss << out(*GetMapPosPointer(x, y, temp)) << ' ';
				}
				ss << endl;

			}

			auto stro = OutPutMapStr(MapType::MAP);
			auto str = ss.str();

			//std::wcout << stro << endl;
			//std::wcout << str << endl;

			delete[] temp;

			updateMap = true;
#endif
		}

		inline void GetArea(Coord start, size_t range, size_t startRange = 0, Path* zoc = nullptr
			, bool allRange = false, size_t allRangeAttackRange = 0
			, bool diagonal = false) {
			if (updateMap) {
				UpdateMap();
			}

			bool stash = zoc == nullptr && allRange == false;
			auto stashAreaKey = make_tuple(start, range, startRange, 0);

			if (stash) {
				if (stashArea.count(stashAreaKey)) {
					area = stashArea[stashAreaKey];

					return;
				}
			}

			area.clear();

			if (GetMap(start.x, start.y, this->map) == MAP_OBSTACLE) {
				return;
			}

			cur_set.clear();
			continue_set.clear();

			open_set.clear();
			close_set.clear();
			point_set.clear();

			open_set.emplace_back(Point{ start,nullptr,0,0 });

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;
			auto neighbourSize = pNeighbour->size();

			size_t totalRange = range + allRange * allRangeAttackRange
							+ allRange;	// Extra loop with allRange due to restart from continue_set
			size_t extraRangeStartPos = range;

			bool extraRangeCalc = false;	// Extra range calc start

			for (size_t nest = 0; nest <= totalRange; nest++) {
				bool addArea = nest > startRange;

				bool updateContiuneSet = allRange && (nest == range);		// Update contiune_set for extra range
				bool extraRangeStart = allRange && (nest == range + 1);		// Extra range start, don't add new area

				if (extraRangeStart) {
					extraRangeCalc = true;					
					extraRangeStartPos = area.size();

					std::swap(cur_set, continue_set);
					open_set.clear();
				}
				else {
					std::swap(cur_set, open_set);
				}

				if (addArea && !extraRangeStart) {
					area.emplace_back();
					area.back().reserve(1 + area.size() * 4);
				}

				if (cur_set.empty()) {
					break;
				}

				while (!cur_set.empty()) {
					Point base = cur_set.back();
					cur_set.pop_back();

					close_set.emplace_back(base);

					if (addArea && !extraRangeStart) {
						area.back().emplace_back(base.coord);
					}

					auto find = [](Set& v, Point& p)->Point* {
						auto it = std::find_if(v.begin(), v.end(), [&p](Point& it)->bool { return it.coord == p.coord; });
						return it != v.end() ? &(*it) : nullptr;
					};

					auto findZoc = [](Path& v, Point& p) {
						auto it = std::find_if(v.begin(), v.end(), [&p](Coord& it)->bool { return it == p.coord; });
						return it != v.end() ? &(*it) : nullptr;
					};

					auto addContinue = [&](Point& p) {
						if (find(continue_set, p) == nullptr) {
							continue_set.emplace_back(p);

							return true;
						}
						else {
							return false;
						}
					};

					if ((!extraRangeCalc) && zoc != nullptr && findZoc(*zoc, base) != nullptr) {
						if (allRange) {
							addContinue(base);
						}

						continue;
					}

					for (size_t i = 0; i < neighbourSize; i++) {
						Coord neighCoord = Coord{ (size_t)(base.coord.x + (*pNeighbour)[i].x)
						,(size_t)(base.coord.y + (*pNeighbour)[i].y) };

						BYTE curCost = GetMap(neighCoord.x, neighCoord.y, this->map);
						Point neighPoint = Point{ neighCoord, nullptr, 0, base.cost + curCost + (*pNeighbour)[i].generalCost };

						if (curCost == MAP_OBSTACLE) {
							continue;
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						if (find(open_set, neighPoint) == nullptr) {
							if (updateContiuneSet) {
								addContinue(base);
							}
							else {
								open_set.emplace_back(neighPoint);
							}
						}
					}
				}
			}

			if (stash) {
				stashArea[stashAreaKey] = area;
			}

#ifdef _DEBUG
			std::wstringstream ss;

			BYTE* temp = new BYTE[mapSize];
			memcpy(temp, map, mapSize);

			auto out = [&](BYTE cost)->wchar_t {
				switch (cost) {
				case MAP_OBSTACLE:
					return L'O';
				case 254:
					return L'+';
				case 125:
					return L'M';
				case 124:
					return L'A';
				case 50:
					return L'Z';
				default:
					return L'P';
				}
			};

			if (zoc != nullptr) {
				for (auto& it : *zoc) {
					*GetMapPosPointer(it.x, it.y, temp) = 50;
					*GetMapPosPointer(it.x, it.y, map) = 50;
				}
			}

			for (size_t it = 0; it < area.size(); it++) {
				if (it < extraRangeStartPos) {
					for (auto& it_C : area[it]) {
						*GetMapPosPointer(it_C.x, it_C.y, temp) = 125;
					}
				}
				else {
					for (auto& it_C : area[it]) {
						*GetMapPosPointer(it_C.x, it_C.y, temp) = 124;
					}
				}
			}

			*GetMapPosPointer(start.x, start.y, temp) = 254;

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					ss << out(*GetMapPosPointer(x, y, temp)) << ' ';
				}
				ss << endl;

			}

			auto stro = OutPutMapStr(MapType::MAP);
			auto str = ss.str();

			//std::wcout << stro << endl;
			//std::wcout << str << endl;

			delete[] temp;

			updateMap = true;
#endif
		}
	};
}
