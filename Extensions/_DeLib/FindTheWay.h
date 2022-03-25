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

#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")

#include "StrNum.h"

using namespace std;

namespace FindTheWay {
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
		}

		return out;
	}

	template <typename T>
	constexpr auto Range(T X) { return (max(0, min(255, X))); }

	constexpr auto PATH_RESERVE = 50;
	constexpr auto SAVEDPATH_RESERVE = 10;

	constexpr auto MAP_PATH = 0;
	constexpr auto MAP_OBSTACLE = 255;

	constexpr auto STEP_UNREACHABLE = 65536;

	constexpr auto STEP_GENERALCOST_NORMAL = 10;
	constexpr auto STEP_GENERALCOST_DIAGONAL = 14;	// 1.414

	constexpr auto COORD_INVALID = 65536;

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

		struct offset {
			char x;
			char y;
			char generalCost;
		};

		using Neighbour = vector<offset>;

		const Neighbour normalNeighbour = { {-1,0,STEP_GENERALCOST_NORMAL}
										,{1,0,STEP_GENERALCOST_NORMAL}
										,{0,-1,STEP_GENERALCOST_NORMAL}
										,{0,1,STEP_GENERALCOST_NORMAL} };

		const Neighbour diagonalNeighbour = { {-1,0,STEP_GENERALCOST_NORMAL}
												,{1,0,STEP_GENERALCOST_NORMAL}
												,{0,-1,STEP_GENERALCOST_NORMAL}
												,{0,1,STEP_GENERALCOST_NORMAL}
												// diagonal part
												,{-1,-1,STEP_GENERALCOST_DIAGONAL}
												,{1,-1,STEP_GENERALCOST_DIAGONAL}
												,{-1,1,STEP_GENERALCOST_DIAGONAL}
												,{1,1,STEP_GENERALCOST_DIAGONAL} };

		bool pathAvailable = false;
		Path path;

		Paths savedPath;

		using StashPathKey = tuple<Coord, Coord>;

		struct StashPathKeyCompare
		{
			bool operator()(StashPathKey lKey, StashPathKey rKey)  const noexcept {
				auto& [lKeyS, lKeyD] = lKey;
				auto& [rKeyS, rKeyD] = rKey;

				return (lKeyS < rKeyS) && (lKeyD < rKeyD);
			};
		};

		using Stash = std::map<StashPathKey, Path, StashPathKeyCompare>;

		Stash stashPath;

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

		inline BYTE* GetMapPosPointer(size_t x, size_t y, BYTE* pMap) {
			if (pMap == nullptr || !PosValid(x, y)) {
				return nullptr;
			}

			return pMap + y * width + x;
		}

		inline BYTE* GetMapPosPointer(size_t x, size_t y, MapType type) {
			return GetMapPosPointer(x, y, GetMapPointer(type));
		}

		inline void ClearMap(BYTE* pMap, bool needUpdate = true) {
			memset(pMap, 0, mapSize);

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

		inline void Reserve() {
			path.reserve(PATH_RESERVE);
			savedPath.reserve(SAVEDPATH_RESERVE);

			open_set.reserve(2 * PATH_RESERVE);
			close_set.reserve(2 * PATH_RESERVE);
			point_set.reserve(2 * PATH_RESERVE);
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
			Reserve();
			Allocate(width,height);
		}
		FindTheWayClass(const wstring& base64) {
			Reserve();
			SetMap(base64);
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
			this->girdSize = girdSize;

			this->girdOffestX = girdOffestX;
			this->girdOffestY = girdOffestY;
		}

		inline Coord GetGirdCoord(Coord realCoord) {
			return realCoord / girdSize;
		}

		inline Coord GetRealCoord(Coord girdCoord) {
			return girdCoord * girdSize + (size_t)(girdSize / 2);
		}

		inline size_t GetGirdSize() {
			return girdSize;
		}

		inline void ClearMap(MapType type, bool needUpdate = true) {
			ClearMap(GetMapPointer(type), needUpdate);
		}

		inline void SetMap(size_t x, size_t y, BYTE cost, MapType type, bool needUpdate = true) {
			SetMap(x, y, cost, GetMapPointer(type), needUpdate);
		}

		inline void SetMap(const wstring& base64) {
			// Format: width:height:terrian:dynamic
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

			DWORD dwNeed = mapSize;

			if (!CryptStringToBinary(GetSubStr(start, end).c_str(), 0, CRYPT_STRING_BASE64, terrain, &dwNeed, NULL, NULL)) {
				throw INVALID_DATA;
			}
			if (!CryptStringToBinary(GetSubStr(start, end).c_str(), 0, CRYPT_STRING_BASE64, dynamic, &dwNeed, NULL, NULL)) {
				throw INVALID_DATA;
			}

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

			auto Convert = [&](BYTE* pData)->void {
				DWORD dwSize;
				CryptBinaryToString(pData, mapSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwSize);

				auto base64Str = std::make_unique<wchar_t[]>(dwSize);

				if (CryptBinaryToString(pData, mapSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64Str.get(), &dwSize)) {
					base64 += DELIMETER;
					base64 += base64Str.get();
				}
				else {
					throw(BASE64_FAILED);
				}				
			};

			Convert(terrain);
			Convert(dynamic);

			return base64;
		}

		inline void UpdateMap() {
			for (size_t i = 0; i < mapSize; i++) {
				*(map + i) = Range(*(terrain + i) + *(dynamic + i));
			}

			updateMap = false;
			stashPath.clear();
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

		inline wstring OutPutMapStr(MapType type = MapType::MAP) {
			auto find = [](Path& path, Coord& coord)->bool {
				return std::find(path.begin(), path.end(), coord) != path.end();
			};
			
			std::wstringstream ss;

			ss << L"MapType : " << type << endl;
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					Coord cur = { x,y };

					if (find(path, cur)) {
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

		inline void ForceFind(Coord start, Coord destination, bool diagonal = false
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

			Find(start, destination, diagonal, h);

			SetMap(start.x, start.y, startCost, this->map, false);
			SetMap(destination.x, destination.y, destinationCost, this->map, false);
		}

		inline void Find(Coord start, Coord destination, bool diagonal = false
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

			pathAvailable = false;
			path.clear();

			auto stashPathKey = make_tuple(start, destination);

			if (stashPath.count(stashPathKey)) {
				path = stashPath[stashPathKey];

				return;
			}

			if ((GetMap(start.x, start.y, this->map) == MAP_OBSTACLE)
				|| (GetMap(destination.x, destination.y, this->map) == MAP_OBSTACLE)) {
				return;
			}

			open_set.clear();
			close_set.clear();
			point_set.clear();

			open_set.emplace_back(Point{ start,nullptr,0,0 });

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;

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

					close_set.emplace_back(base);
					open_set.erase(open_set.cend() - 1);

					auto find = [](Set& v, Point& p)->Point* {
						auto it = std::find_if(v.begin(), v.end(), [&p](Point& it)->bool { return it.coord == p.coord; });

						return it != v.end() ? &(*it) : nullptr;
					};					

					for (auto& it : *pNeighbour) {
						Coord neighCoord = Coord{ (size_t)(base.coord.x + it.x)
												,(size_t)(base.coord.y + it.y) };

						BYTE curCost = GetMap(neighCoord.x, neighCoord.y, this->map);
						Point neighPoint = Point{ neighCoord, nullptr, 0, base.cost + curCost + it.generalCost };

						if (curCost == MAP_OBSTACLE) {
							continue;
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						auto heuristic = [&](Point& p)->size_t { return h(neighPoint.coord, destination); };
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
	};
}
