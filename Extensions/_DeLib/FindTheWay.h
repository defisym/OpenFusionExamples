// ReSharper disable CppClangTidyClangDiagnosticShadow

#pragma once

// 進んだ道の先　光が見つかるから
// YOU'LL FIND THE WAY

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <functional>
#include <algorithm>
#include <numbers>
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

namespace FindTheWay {
	using BYTE = unsigned char;

	enum class CoordType {
		X,
		Y,
	};

	using CoordValueType = long;

	struct Coord {
		CoordValueType x = 0;
		CoordValueType y = 0;

		Coord() = default;

		template<typename Number>
			requires std::is_arithmetic_v<Number>
		Coord(Number x, Number y) {
			this->x = static_cast<CoordValueType>(x);
			this->y = static_cast<CoordValueType>(y);
		}

		Coord(double x, double y) {
			this->x = lround(x);
			this->y = lround(y);
		}

		inline bool operator<(const Coord& B) const {
			return (this->x < B.x) && (this->y < B.y);
		}

		inline bool operator==(const Coord& B) const {
			return (this->x == B.x) && (this->y == B.y);
		}

		inline Coord operator+(const Coord& B) const {
			return { this->x + B.x ,this->y + B.y };
		}

		inline Coord operator-(const Coord& B) const {
			return { this->x - B.x ,this->y - B.y };
		}

		template<typename Number>
			requires std::is_arithmetic_v<Number>
		inline Coord operator+(const Number& B) const {
			return { this->x + B ,this->y + B };
		}

		template<typename Number>
			requires std::is_arithmetic_v<Number>
		inline Coord operator-(const Number& B) const {
			return { this->x - B ,this->y - B };
		}

		template<typename Number>
			requires std::is_arithmetic_v<Number>
		inline Coord operator*(const Number& B) const {
			return { this->x * B ,this->y * B };
		}

		template<typename Number>
			requires std::is_arithmetic_v<Number>
		inline Coord operator/(const Number& B) const{
			return { this->x / B ,this->y / B };
		}
	};

	enum class MapType {
		MAP,
		TERRAIN,
		DYNAMIC,
	};

	constexpr auto MAPTYPENUM = 3;

	inline std::ostream& operator<<(std::ostream& out, MapType type) {
		switch (type) {
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

	inline std::wostream& operator<<(std::wostream& out, MapType type) {
		switch (type) {
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

	using Path = std::vector<Coord>;
	using CoordSet = Path;	// for zoc
	using Paths = std::unordered_map<std::wstring, Path>;
	using Heuristic = std::function<size_t(Coord, Coord)>;
	using UpdateMapCallBack = std::function<void(void*)>;

	using Area = std::vector<CoordSet>;

	using Exception = unsigned char;

	// try {
	// 	// Invalid Size
	// 	FindTheWayClass* pFTW = new FindTheWayClass(0, 0);
	//	// Reg callback if needed
	//	pFTW->SetUpdateMapCallBack(UpdateMapCallBackFunc, rdPtr);
	// 
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

	inline std::ostream& operator<<(std::ostream& out, const Exception type) {
		switch (type) {
		case INVALID_SIZE:
			out << "INVALID_SIZE";
			break;
		case INVALID_DATA:
			out << "INVALID_DATA";
			break;
		case BASE64_FAILED:
			out << "BASE64_FAILED";
			break;
		default:
			out << "UNKNOWN_ERROR";
		}

		return out;
	}

	inline std::wostream& operator<<(std::wostream& out, const Exception type) {
		switch (type) {
		case INVALID_SIZE:
			out << L"INVALID_SIZE";
			break;
		case INVALID_DATA:
			out << L"INVALID_DATA";
			break;
		case BASE64_FAILED:
			out << L"BASE64_FAILED";
			break;
		default:
			out << "UNKNOWN_ERROR";
		}

		return out;
	}

	constexpr auto PATH_RESERVE = 50;
	constexpr auto SAVEDPATH_RESERVE = 10;
	constexpr auto UNIT_RESERVE = 50;

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

	constexpr auto DELIMITER = ':';

	constexpr auto DEFAULT_IGNOREFLAG = 0b01001U;
	constexpr auto REPEL_IGNOREFLAG = 0b11100U;
	constexpr auto DEFAULT_EXTRARANGESTARTPOS = 65536;

	constexpr auto ATTACK = true;
	constexpr auto MOVE = false;
	constexpr auto REPEL = false;

	class FindTheWayClass {
	private:
		size_t width = 0;
		size_t height = 0;

		size_t mapSize = 0;

		size_t gridSize = 1;
		size_t isoGridWidth = 1;
		size_t isoGridHeight = 1;

		size_t gridOffsetX = 0;
		size_t gridOffsetY = 0;

		bool isometric = false;

		bool updateMap = false;

		UpdateMapCallBack updateCallBack = nullptr;
		void* updateCallBackCoef = nullptr;

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

		using Neighbour = std::vector<Offset>;

		inline static const Neighbour normalNeighbour = { {1,0,STEP_GENERALCOST_NORMAL}
										,{0,1,STEP_GENERALCOST_NORMAL}
										,{-1,0,STEP_GENERALCOST_NORMAL}
										,{0,-1,STEP_GENERALCOST_NORMAL} };

		inline static const Neighbour diagonalNeighbour = { {1,0,STEP_GENERALCOST_NORMAL}
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

		// do not stash in debug mode
#ifdef _DEBUG
		bool stash = false;
#endif

#ifdef NDEBUG
		bool stash = true;
#endif

		using CoordHash = size_t;
		using CoordSetHash = size_t;
		using IgnoreFlagHash = size_t;

		using StashPathKey = std::tuple<CoordHash, CoordHash, IgnoreFlagHash, CoordSetHash, CoordSetHash, CoordSetHash>;
		StashPathKey stashPathKey;

		//struct StashPathKeyCompare {
		//	bool operator()(StashPathKey lKey, StashPathKey rKey)  const noexcept {
		//		auto& [lKeyS, lKeyD] = lKey;
		//		auto& [rKeyS, rKeyD] = rKey;

		//		return (lKeyS < rKeyS) && (lKeyD < rKeyD);
		//	};
		//};

		using StashPath = std::map<StashPathKey, Path>;
		StashPath stashPath;

		struct Point;
		using Set = std::vector<Point>;

		struct Point {
			Coord coord;

			size_t parentID = -1;

			int priority = 0;
			size_t cost = 0;

			inline operator Coord() const {
				return coord;
			}

			inline Point(Coord coord) {
				this->coord = coord;
			}

			inline Point(Coord coord, size_t cost) {
				this->coord = coord;
				this->cost = cost;
			}

			inline Point* GetParent(Set& s) const {
				if (parentID == static_cast<size_t>(-1)) { return nullptr; }

				// doesn't check it here, as it should be valid
				return &s[parentID];
			}
		};

		Set open_set;
		Set close_set;
		Set point_set;
		CoordSet unit_set;

		static inline auto find(const Path& path, const Coord& coord) -> bool {
			return std::ranges::find(path, coord) != path.end();
		}

		static inline auto find(Set& v, Point& p) -> Point* {
			const auto it = std::ranges::find_if(v,
				[&p] (const Point& it)->bool {
					return it.coord == p.coord;
			});

			return it != v.end() ? &(*it) : nullptr;
		}

		static inline auto rfind(Set& v, Point& p) -> Point* {
			const auto it = std::find_if(v.rbegin(), v.rend(),
				[&p] (const Point& it)->bool {
					return it.coord == p.coord;
			});

			return it != v.rend() ? &(*it) : nullptr;
		}

		static inline auto findIdx(const Set& v, const Point& p) {
			for (size_t idx = 0; idx < v.size(); idx++) {
				if (v[idx].coord == p.coord) {
					return idx;
				}
			}

			return static_cast<size_t>(-1);
		}

		static inline auto findSet(const CoordSet& v, const Coord& p) {
			return std::ranges::find_if(v,
				[&p] (auto& it)->bool {
					return it == p;
				}) != v.end();
		}

		static inline auto findSet(const CoordSet& v, Point& p) {
			const auto it = std::ranges::find_if(v,
				[&p] (const Coord& it)->bool {
					return it == p.coord;
				});
			return it != v.end() ? &(*it) : nullptr;
		}

		static inline auto addSet(CoordSet& v, Coord& p) {
			if (!findSet(v, p)) {
				v.emplace_back(p);

				return true;
			}

			return false;
		}

		static inline auto addSet(CoordSet& v, Point& p) {
			return addSet(v, p.coord);
		}

		static inline auto addSet(CoordSet& v, CoordSet& src) {
			for(auto& it: src) {
				addSet(v, it);
			}
		}

		Base64<std::wstring> base64;

		Area area;
		Set cur_set;
		Set continue_set;
		CoordSet extra_set;

		std::vector<size_t> ranges;

		struct AreaRange {
			size_t range = 0;
			size_t startRange = 0;
			bool attack = false;
			bool allRange = false;
			size_t allRangeAttackRange = 0;
		};

		using AreaRangeHash = size_t;
		using IsLine = bool;

		using StashAreaKey = std::tuple<IsLine, CoordHash, AreaRangeHash, IgnoreFlagHash, CoordSetHash, CoordSetHash, CoordSetHash>;
		using StashAreaValue = std::tuple<Area, size_t>;

		AreaRange settings;
		StashAreaKey stashAreaKey;

		//struct StashAreaKeyCompare {
		//	bool operator()(StashAreaKey lKey, StashAreaKey rKey)  const noexcept {
		//		auto& [lc, lr, lsr, lm] = lKey;
		//		auto& [rc, rr, rsr, rm] = rKey;
		//
		//		return (lc < rc) && (lr < rr) && (lsr < rsr) && (lm < rm);
		//	};
		//};

		using StashArea = std::map<StashAreaKey, StashAreaValue>;

		StashArea stashArea;

		inline static size_t Hasher(const AreaRange& c) {
			size_t seed = 5;

			seed ^= c.range + HASHER_MOVE(seed);
			seed ^= c.startRange + HASHER_MOVE(seed);
			seed ^= c.attack + HASHER_MOVE(seed);
			seed ^= c.allRange + HASHER_MOVE(seed);
			seed ^= c.allRangeAttackRange + HASHER_MOVE(seed);

			return seed;
		}

		inline static size_t Hasher(const Coord& c) {
			size_t seed = 2;

			seed ^= c.x + HASHER_MOVE(seed);
			seed ^= c.y + HASHER_MOVE(seed);

			return seed;
		}

		inline static size_t Hasher(const CoordSet* c) {
			if (c == nullptr) {
				return 0;
			}

			size_t seed = c->size();

			for (auto& it : *c) {
				seed ^= Hasher(it) + HASHER_MOVE(seed);
			}

			return seed;
		}

		inline size_t pOffset(const size_t x, const size_t y) const {
			return y * width + x;
		}

		inline bool PosValid(const size_t x, const size_t y) const {
			return (x < width) && (y < height) && ((y * width + x) < (mapSize));
		}

		inline BYTE* GetMapPointer(const MapType type) const {
			switch (type) {
			case FindTheWay::MapType::MAP:
				return map;
			case FindTheWay::MapType::TERRAIN:
				return terrain;
			case FindTheWay::MapType::DYNAMIC:
				return dynamic;
			}

			return nullptr;
		}

		inline BYTE* GetMapPosPointer(const size_t x, const size_t y, BYTE* pMap) const {
			if (pMap == nullptr || !PosValid(x, y)) {
				return nullptr;
			}

			return pMap + y * width + x;
		}

		inline BYTE* GetMapPosPointer(const size_t x, const size_t y, const MapType type) const {
			return GetMapPosPointer(x, y, GetMapPointer(type));
		}

		inline void ClearMap(BYTE* pMap, const BYTE cost = MAP_PATH, const bool needUpdate = true) {
			memset(pMap, Range(cost), mapSize);

			updateMap = needUpdate;
		}

		inline void SetMap(const size_t x, const size_t y, const BYTE cost, BYTE* pMap, const bool needUpdate = true) {
			BYTE* pMapPos = GetMapPosPointer(x, y, pMap);

			if (pMapPos == nullptr) {
				return;
			}

			*pMapPos = static_cast<BYTE>(Range(cost));

			updateMap = needUpdate;
		}

		inline BYTE GetMap(const size_t x, const size_t y, BYTE* pMap) const {
			const BYTE* pMapPos = GetMapPosPointer(x, y, pMap);

			if (pMapPos == nullptr) {
				return MAP_OBSTACLE;
			}

			return *pMapPos;
		}

		inline Path* GetPathPointer(const std::wstring* name = nullptr) {
			return ((name != nullptr) && (savedPath.contains(*name)) ? &savedPath [*name] : &path);
		}

		inline static size_t ZeroProtection(const size_t v) {
			return v ? v : 1;		// Protection for 0
		}

		inline void Reserve() {
			path.reserve(PATH_RESERVE);
			savedPath.reserve(SAVEDPATH_RESERVE);

			open_set.reserve(2 * PATH_RESERVE);
			close_set.reserve(2 * PATH_RESERVE);
			point_set.reserve(2 * PATH_RESERVE);
			unit_set.reserve(UNIT_RESERVE);

			area.reserve(AREA_RESERVE);
			cur_set.reserve(2 * AREA_RESERVE);
			continue_set.reserve(2 * AREA_RESERVE);
			extra_set.reserve(AREA_RESERVE);
		}

		inline void Allocate(const size_t width, const size_t height) {
			this->width = width;
			this->height = height;

			this->mapSize = width * height;

			if (mapSize == 0) {
				throw INVALID_SIZE;
			}

			map = new BYTE [mapSize];
			terrain = new BYTE [mapSize];
			dynamic = new BYTE [mapSize];

			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(dynamic, 0, mapSize);

			//pMap[(int)MapType::MAP] = map;
			//pMap[(int)MapType::TERRAIN] = terrain;
			//pMap[(int)MapType::DYNAMIC] = dynamic;
		}

		inline void ClearData() {
			updateMap = false;

			path.clear();
			savedPath.clear();
			pathAvailable = false;

			area.clear();

			ClearStash();
		}

		inline void ClearMap() {
			memset(map, 0, mapSize);
			memset(terrain, 0, mapSize);
			memset(dynamic, 0, mapSize);

			ClearData();
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
		FindTheWayClass(const size_t width, const size_t height) {
			Allocate(width, height);
			Reserve();
		}

		explicit FindTheWayClass(const std::wstring& base64) {
			SetMap(base64);
			Reserve();
		}

		~FindTheWayClass() {
			Free();
		}

		inline void SetIsometric(const bool isometric) {
			if (this->isometric == isometric) { return; }

			this->isometric = isometric;
			SetGridSize(GetIsometricGridSize(gridSize, gridSize / 2), gridOffsetX, gridOffsetY);
		}

		inline size_t GetGridWidth() const {
			return !isometric ? gridSize : isoGridWidth;
		}

		inline size_t GetGridHeight() const {
			return !isometric ? gridSize : isoGridHeight;
		}

		inline size_t GetGridOffsetX() const {
			return gridOffsetX;
		}

		inline size_t GetGridOffsetY() const {
			return gridOffsetY;
		}

		inline void SetGridSize(const size_t gridSize = 1, const size_t gridOffsetX = 0, const size_t gridOffsetY = 0) {
			this->gridOffsetX = gridOffsetX;
			this->gridOffsetY = gridOffsetY;

			auto [isoGridWidth, isoGridHeight] = ParseIsometricGridSize(gridSize);
			this->gridSize = isoGridWidth;

			if (!isometric) { return; }

			// ISOMetric specific
			this->isoGridWidth = isoGridWidth;
			this->isoGridHeight = isoGridHeight;

			ISOConverter = ISOMatrix(isoGridWidth, isoGridHeight,
				gridOffsetX, gridOffsetY);
			ISOConverter.SetMapSize(width, height);
		}

		inline Coord GetGridCoord(const Coord& realCoord) const {
			return isometric ? GetIsometricGridCoord(realCoord) : GetTraditionalGridCoord(realCoord);
		}

		inline Coord GetRealCoord(const Coord& gridCoord) const {
			return isometric ? GetIsometricRealCoord(gridCoord) : GetTraditionalRealCoord(gridCoord);
		}

		inline Coord GetTraditionalGridCoord(const Coord& realCoord) const {
			return (realCoord - Coord{ gridOffsetX, gridOffsetY }) / gridSize;
		}

		inline Coord GetTraditionalRealCoord(const Coord& gridCoord) const {
			return gridCoord * gridSize + gridSize / 2 + Coord{ gridOffsetX ,gridOffsetY };
		}

		static inline auto ParseIsometricGridSize(const size_t gridSize) {
			const auto isoGridWidth = ZeroProtection((gridSize & 0x0000FFFF));
			const auto isoGridHeight = ZeroProtection((gridSize & 0xFFFF0000) >> 16);

			return std::make_tuple(isoGridWidth, isoGridHeight);
		}

		inline static size_t GetIsometricGridSize(size_t isoGridWidth = 1, size_t isoGridHeight = 1) {
			isoGridWidth = ZeroProtection(isoGridWidth & 0xFFFF);
			isoGridHeight = ZeroProtection(isoGridHeight & 0xFFFF);

			return (isoGridHeight << 16) | isoGridWidth;
		}

		// Get the best fit map size
		inline static auto CalcMapSize(const size_t width, const size_t height,
			const size_t gridSize, const bool isometric = false) {
			auto [isoGridWidth, isoGridHeight] = ParseIsometricGridSize(gridSize);

			if (!isometric) {
				return std::make_tuple(width / isoGridWidth, height / isoGridHeight);
			}

			// ISOMetric: Width = GridWidth / 2 * (MapWidth + MapHeight)
			//			  Height = GridHeight / 2 * (MapWidth + MapHeight)
			const auto wh_1 = width * 2 / isoGridWidth;
			const auto wh_2 = height * 2 / isoGridHeight;

			const auto wh = (std::min)(wh_1, wh_2);
			const bool bEven = wh % 2 == 0;

			const auto mapHeight = wh / 2;
			const auto mapWidth = mapHeight + bEven;

			return std::make_tuple(mapWidth, mapHeight);
		}

	private:
		// 1. Rotate -> 45 degrees
		// 2. Sacle
		// 3. Use traditional method
		class ISOMatrix {
			size_t width = 0;
			size_t height = 0;

			size_t gridWidth = 1;
			size_t gridHeight = 1;

			size_t gridOffsetX = 0;
			size_t gridOffsetY = 0;

			double matrix[2][2];
			double reverseMatrix[2][2];

			Coord ISOGridOffset;

			static inline auto Matrix(const double m[2][2], const double x, const double y) {
				return std::make_tuple(m[0][0] * x + m[0][1] * y,
					m[1][0] * x + m[1][1] * y);
			}

		public:
			ISOMatrix() = default;
			ISOMatrix(const size_t gridWidth, const size_t gridHeight,
				const size_t gridOffsetX = 0, const size_t gridOffsetY = 0) {
				this->gridWidth = gridWidth;
				this->gridHeight = gridHeight;

				this->gridOffsetX = gridOffsetX;
				this->gridOffsetY = gridOffsetY;

				// rotate 45 degree
				// | cos, -sin |
				// | sin,  cos |
				constexpr auto cos = 1 / std::numbers::sqrt2;
				constexpr auto sin = cos;

				// scale
				// | scaleX, 0		|
				// | 0,		 scaleY |
				constexpr auto scaleX = std::numbers::sqrt2 / 2;
				const auto scaleY = static_cast<double>(gridHeight) / (std::numbers::sqrt2 * gridWidth);

				// total
				// scale * rotate
				constexpr auto a = scaleX * cos;
				constexpr auto b = scaleX * -1 * sin;
				const auto c = scaleY * sin;
				const auto d = scaleY * cos;

				const double reverseScale = 1 / (a * c - b * d);

				// matrix
				matrix[0][0] = a;
				matrix[0][1] = b;
				matrix[1][0] = c;
				matrix[1][1] = d;

				//reverse matrix
				reverseMatrix[0][0] = reverseScale * d;
				reverseMatrix[0][1] = reverseScale * -1 * b;
				reverseMatrix[1][0] = reverseScale * -1 * c;
				reverseMatrix[1][1] = reverseScale * a;
			}

			inline void SetMapSize(const size_t width, const size_t height) {
				this->width = width;
				this->height = height;

				ISOGridOffset = { gridOffsetX + gridWidth * height / 2, gridOffsetY + 0u };
			}

			inline const Coord& GetISOGridOffset() const {
				return ISOGridOffset;
			}

			// convert treats ( 0, 0 ) as origin -> no grid offset
			inline auto ToISO(const double x, const double y) const {
				return Matrix(matrix, x, y);
			}
			// convert treats ( 0, 0 ) as origin -> no grid offset
			inline auto ToTradition(const double x, const double y) const {
				return Matrix(reverseMatrix, x, y);
			}
		};

		// update when grid size is updated
		ISOMatrix ISOConverter;

	public:
		inline Coord GetIsometricGridCoord(const Coord& realCoord) const {
			const auto& ISOGridOffset = ISOConverter.GetISOGridOffset();
			const auto [x, y] = ISOConverter.ToTradition(realCoord.x - ISOGridOffset.x,
				realCoord.y - ISOGridOffset.y);
			const auto tradition = GetTraditionalGridCoord({ x + gridOffsetX,
				y + gridOffsetY });

			return tradition;
		}

		inline Coord GetIsometricRealCoord(const Coord& gridCoord) const {
			const auto& ISOGridOffset = ISOConverter.GetISOGridOffset();
			const auto tradition = GetTraditionalRealCoord(gridCoord);
			const auto [x, y] = ISOConverter.ToISO(tradition.x - gridOffsetX,
				tradition.y - gridOffsetY);

			return { x + ISOGridOffset.x,
				y + ISOGridOffset.y };
		}

		inline void ClearMap(const MapType type, const BYTE cost = MAP_PATH, const bool needUpdate = true) {
			ClearMap(GetMapPointer(type), cost, needUpdate);
		}

		inline void SetMap(const size_t x, const size_t y, const BYTE cost, const MapType type, const bool needUpdate = true) {
			SetMap(x, y, cost, GetMapPointer(type), needUpdate);
		}

		inline void SetMap(const std::wstring& base64) {
			// Format: width:gridHeight:terrain:dynamic
			size_t start = 0;
			size_t end = 0;

			auto GetSubStr = [base64] (size_t& start, size_t& end)->std::wstring {
				if (end != std::wstring::npos) {
					end = base64.find(DELIMITER, start);
					std::wstring result = base64.substr(start, end - start);

					start = end + 1;

					return result;
				}
				else {
					return std::wstring{};
				}
			};

			const auto width = _stoi(GetSubStr(start, end));
			const auto height = _stoi(GetSubStr(start, end));

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

		inline BYTE GetMap(const size_t x, const size_t y, const MapType type, const bool forceUpdate = true) {
			if (forceUpdate && updateMap && type == MapType::MAP) {
				UpdateMap();
			}

			return GetMap(x, y, GetMapPointer(type));
		}

		inline std::wstring GetMap() {
			std::wstring base64;

			base64 += _itos(width);
			base64 += DELIMITER;
			base64 += _itos(height);

			base64 += this->base64.base64_encode(terrain, mapSize);
			base64 += this->base64.base64_encode(dynamic, mapSize);

			return base64;
		}

		inline void UpdateMap() {
			for (size_t i = 0; i < mapSize; i++) {
				*(map + i) = static_cast<BYTE>(Range(*(terrain + i) + *(dynamic + i)));
			}

			// Path/Area invalidation when map changes, clear
			ClearData();

			//Callback
			if (updateCallBack != nullptr) {
				updateCallBack(updateCallBackCoef);
			}
		}

		inline void SetUpdateMapCallBack(const UpdateMapCallBack& f, void* p) {
			updateCallBack = f;
			updateCallBackCoef = p;
		}

		inline void IterateMap(const MapType type,
			const std::function<void(const size_t x, const size_t y, const BYTE cost)>& cb) const {
			const auto pMap = GetMapPointer(type);

			if (pMap == nullptr) { return; }

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					const BYTE* pMapPos = GetMapPosPointer(x, y, pMap);

					if (pMapPos == nullptr) {
						continue;
					}

					cb(x, y, *pMapPos);
				}
			}			
		}

		inline bool CoordObstacle(const size_t x, const size_t y, const MapType type) {
			return GetMap(x, y, type) == MAP_OBSTACLE;
		}

		inline bool GetPathValidity(const std::wstring* name = nullptr) const {
			return ((name == nullptr) && this->pathAvailable) || ((name != nullptr) && (savedPath.contains(*name)));
		}

		inline size_t GetDistance(const std::wstring* name = nullptr) {
			const Path* pPath = GetPathPointer(name);
			const bool curPathAvaliable = GetPathValidity(name);

			return curPathAvaliable ? pPath->size() : STEP_UNREACHABLE;
		}

		inline CoordValueType GetCoordAtPath(const size_t pos, const CoordType type, const std::wstring* name = nullptr) {
			const Path* pPath = GetPathPointer(name);
			const bool curPathAvaliable = GetPathValidity(name);

			if (!curPathAvaliable || pPath->size() <= pos) {
				return COORD_INVALID;
			}

			return type == CoordType::X ? (*pPath) [pos].x : (*pPath) [pos].y;
		}

		inline size_t GetMapWidth() const {
			return width;
		}

		inline size_t GetMapHeight() const {
			return height;
		}

		inline void OutPutMap(const MapType type = MapType::MAP) {
			if (updateMap) { UpdateMap(); }

			std::cout << "MapType : " << type << '\n';
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					Coord cur = { x,y };

					if (find(path, cur)) {
						std::cout << '-';
					}
					else {
						std::cout << (((*GetMapPosPointer(x, y, type)) != MAP_OBSTACLE) ? '#' : '*');
					}
					std::cout << ' ';
				}
				std::cout << '\n';
			}
		}

		inline std::wstring OutPutMapStr(const MapType type = MapType::MAP,
			const bool showPath = true,
			const std::wstring* name = nullptr) {
			if (updateMap) { UpdateMap(); }

			std::wstringstream ss;

			ss << L"MapType : " << type << '\n';
			ss << L"Obstacle = \'*\', Path = \'#\'" << '\n';

			const Path* pPath = nullptr;
			bool curPathAvaliable = false;

			if (showPath) {
				pPath = GetPathPointer(name);
				curPathAvaliable = GetPathValidity(name);

				if (curPathAvaliable) {
					const std::wstring displayName = name == nullptr ? L"Last Path" : *name;
					ss << "Step of \"" << displayName << R"(" = '-')" << '\n';
				}
				else if (name != nullptr) {
					ss << "Target Path \"" << *name << "\" Invalid" << '\n';
				}
			}

			ss << L"----------" << '\n';

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					if (showPath && curPathAvaliable && find(*pPath, Coord { x,y })) {
						ss << L'-';
					}
					else {
						ss << (((*GetMapPosPointer(x, y, type)) != MAP_OBSTACLE) ? L'#' : L'*');
					}
					ss << ' ';
				}
				ss << '\n';
			}

			return ss.str();
		}

		inline void SaveLastPath(const std::wstring& name) {
			if (!pathAvailable) { return; }

			savedPath[name] = path;
		}

		static inline size_t GetEuclideanDistancePow(const Coord start, const Coord destination) {
			return static_cast<size_t>(pow(start.x - destination.x, 2) 
				+ pow(start.y - destination.y, 2));
		}

		static inline size_t GetEuclideanDistance(const Coord start, const Coord destination) {
			return static_cast<size_t>(sqrt(GetEuclideanDistancePow(start, destination)));
		}

		static inline size_t GetManhattanDistance(const Coord start, const Coord destination) {
			return abs(static_cast<int>(start.x - destination.x))
			+ abs(static_cast<int>(start.y - destination.y));
		}

		inline bool PointObstacle(const Coord p) {
			return CoordObstacle(p.x, p.y, MapType::MAP);
		}

		inline BYTE GetPointCost(const Coord p) {
			return GetMap(p.x, p.y, MapType::MAP);
		}

		inline void ForceFind(const Coord start, const Coord destination, const bool diagonal = false, const bool checkDiagonalCorner = true
			, CoordSet* ally = nullptr, CoordSet* enemy = nullptr, CoordSet* zoc = nullptr
			, const size_t ignoreFlag = DEFAULT_IGNOREFLAG
			, const Heuristic& h = FindTheWayClass::GetManhattanDistance) {
			// Still try to find a way even start & destination is obstacle
			// ......By setting them to path
			if (updateMap) { UpdateMap(); }

			const BYTE startCost = GetMap(start.x, start.y, this->map);
			const BYTE destinationCost = GetMap(destination.x, destination.y, this->map);

			SetMap(start.x, start.y, MAP_PATH, this->map, false);
			SetMap(destination.x, destination.y, MAP_PATH, this->map, false);

			Find(start, destination, diagonal, checkDiagonalCorner, ally, enemy, zoc, ignoreFlag, h);

			SetMap(start.x, start.y, startCost, this->map, false);
			SetMap(destination.x, destination.y, destinationCost, this->map, false);
		}

		inline void Find(const Coord start, const Coord destination, const bool diagonal = false, const bool checkDiagonalCorner = true
			, CoordSet* ally = nullptr, CoordSet* enemy = nullptr, CoordSet* zoc = nullptr
			, size_t ignoreFlag = DEFAULT_IGNOREFLAG
			, const Heuristic& heuristic = GetManhattanDistance) {
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

			if (updateMap) { UpdateMap(); }

			pathAvailable = false;

			if (stash) {
				stashPathKey = std::make_tuple(Hasher(start),
					Hasher(destination), 
					ignoreFlag, 
					Hasher(ally),
					Hasher(enemy), 
					Hasher(zoc));

				if (stashPath.contains(stashPathKey)) {
					pathAvailable = true;
					path = stashPath [stashPathKey];

					return;
				}
			}

			path.clear();

			if ((GetMap(start.x, start.y, this->map) == MAP_OBSTACLE)
				|| (GetMap(destination.x, destination.y, this->map) == MAP_OBSTACLE)) {
				return;
			}

			open_set.clear();
			close_set.clear();
			point_set.clear();

			open_set.emplace_back(start);

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;
			const auto neighbourSize = pNeighbour->size();

			const auto [moveIgnoreZoc,
				moveIgnoreAlly,
				moveIgnoreEnemy,
				attackIgnoreAlly,
				attackIgnoreEnemy
			] = ParseIgnoreFlag(ignoreFlag);

			auto addSet = [&] (CoordSet* src) {
				if (src == nullptr) { return; }

				FindTheWayClass::addSet(unit_set, *src);
			};

			unit_set.clear();

			if (!moveIgnoreAlly) {
				addSet(ally);
			}

			if (!moveIgnoreEnemy) {
				addSet(enemy);
			}

			if (!moveIgnoreZoc && zoc != nullptr) {
				for (auto& it : *zoc) {
					if (!findSet(unit_set, it)
						&& !findSet(*ally, it)				// current point not unit
						&& !findSet(*enemy, it)
						&& it != start && it != destination) {
						unit_set.emplace_back(it);
					}
				}
			}

			while (!open_set.empty()) {
				// Descending
				std::ranges::sort(open_set,
					[] (const Point& A, const Point& B) ->bool {
						return A.priority > B.priority;
					});

				if (open_set.back().coord == destination) {
					Point* parent = &open_set.back();

					while (parent != nullptr) {
						path.emplace_back(parent->coord);
						parent = parent->GetParent(point_set);
					}

					std::ranges::reverse(path);
					pathAvailable = true;

					if (stash) { stashPath[stashPathKey] = path; }

					return;
				}

				Point base = open_set.back();
				open_set.pop_back();

				close_set.emplace_back(base);

				for (size_t i = 0; i < neighbourSize; i++) {
					auto neighCoord = Coord{ base.coord.x + (*pNeighbour)[i].x
						,base.coord.y + (*pNeighbour)[i].y };

					const BYTE curCost = GetMap(neighCoord.x, neighCoord.y, this->map);
					auto neighPoint = Point(neighCoord, base.cost + curCost + (*pNeighbour)[i].generalCost);

					if (curCost == MAP_OBSTACLE) { continue; }

					if (findSet(unit_set, neighCoord)) { continue; }

					if (diagonal && checkDiagonalCorner		// check corner
						&& (i % 2 == 1)) {					// when checking diagonal points
						const size_t prev = i - 1;
						const size_t next = (i + 1) % neighbourSize;

						if ((GetMap(base.coord.x + (*pNeighbour)[prev].x
									, base.coord.y + (*pNeighbour)[prev].y
									, this->map) == MAP_OBSTACLE)
							&& (GetMap(base.coord.x + (*pNeighbour)[next].x
									   , base.coord.y + (*pNeighbour)[next].y
									   , this->map) == MAP_OBSTACLE)) {
							continue;
						}
					}

					if (find(close_set, neighPoint) != nullptr) { continue; }

					auto updatePoint = [&] (Point& cur) {
						cur.cost = neighPoint.cost;
						cur.priority = static_cast<int>(heuristic(neighPoint.coord, destination) + neighPoint.cost);

						const auto baseIdx = findIdx(point_set, base);

						// put base point into point_set
						if (baseIdx == static_cast<size_t>(-1)) {
							point_set.emplace_back(base);
							cur.parentID = point_set.size() - 1;
						}
						else {
							cur.parentID = baseIdx;
						}
					};

					Point* next = find(open_set, neighPoint);

					if (next == nullptr) {
						updatePoint(neighPoint);
						open_set.emplace_back(neighPoint);
					}
					else if (next->cost > neighPoint.cost) {
						updatePoint(*next);
					}
				}
			}
		}

		static inline void GenerateSet(Coord start, CoordSet* set, const bool add = false, bool diagonal = true) {
			if (!add) { set->clear(); }

			set->emplace_back(start);
		}

		static inline void GenerateZoc(const Coord start, CoordSet* zoc, const bool add = false, const bool diagonal = true) {
			if (!add) { zoc->clear(); }

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;

			for (auto& it : *pNeighbour) {
				auto cur = Coord{ static_cast<size_t>(start.x + it.x),
					static_cast<size_t>(start.y + it.y) };

				if (std::ranges::find(*zoc, cur) == zoc->end()) {
					zoc->emplace_back(cur);
				}
			}
		}

		static inline void GenerateZoc(const CoordSet& start, CoordSet* zoc, const bool add = false, bool diagonal = true) {
			if (!add) { zoc->clear(); }

			for (const auto& it : start) {
				GenerateZoc(it, zoc, true);
			}
		}

		inline std::wstring OutPutAreaStr(const Coord start, const size_t range
			, const CoordSet* ally = nullptr, const CoordSet* enemy = nullptr
			, const CoordSet* zoc = nullptr
			, bool allRange = false, size_t extraRangeStartPos = 0) {
			std::wstringstream ss;
			if (!extraRangeStartPos) { extraRangeStartPos = range; }

			const auto temp = new BYTE [mapSize];
			memcpy(temp, map, mapSize);

			auto out = [&] (const BYTE cost)->wchar_t {
				switch (cost) {
				case MAP_OBSTACLE:
					return L'*';
				case 254:
					return L'+';
				case 125:
					return L'□';
				case 124:
					return L'■';
				case 50:
					return L'Z';
				case 49:
					return L'A';
				case 48:
					return L'E';
				default:
					return L'#';
				}
			};

			auto updateSet = [&] (const CoordSet* p, const BYTE cost) {
				if (p != nullptr) {
					for (const auto& it : *p) {
						if (const auto pMapPos = GetMapPosPointer(it.x, it.y, temp)) { *pMapPos = cost; }
					}
				}
			};

			if (updateMap) { UpdateMap(); }

			for (size_t it = 0; it < area.size(); it++) {
				if (it < extraRangeStartPos) {
					for (const auto& it_C : area [it]) {
						*GetMapPosPointer(it_C.x, it_C.y, temp) = 125;
					}
				}
				else {
					for (auto& it_C : area [it]) {
						if (it_C == Coord { 5, 4 }) {
							std::cout << "1" << '\n';
						}
						*GetMapPosPointer(it_C.x, it_C.y, temp) = 124;
					}
				}
			}

			updateSet(zoc, 50);
			updateSet(ally, 49);
			updateSet(enemy, 48);

			*GetMapPosPointer(start.x, start.y, temp) = 254;

			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					ss << out(*GetMapPosPointer(x, y, temp)) << ' ';
				}
				ss << '\n';
			}

			delete[] temp;

			return ss.str();
		}

		inline size_t GetAbleLineRange(const Coord start, const size_t range, const size_t dir
			, const CoordSet* ally = nullptr, const CoordSet* enemy = nullptr, CoordSet* zoc = nullptr
			, const size_t ignoreFlag = DEFAULT_IGNOREFLAG // Move range ignore ally && Attack range ignore enemy by default
			, const bool attack = false) {                 // Attack = use ignore flag, only consider map collision
														   // Repel = consider both map collision & unit
			if (updateMap && !attack) { UpdateMap(); }

			const auto [moveIgnoreZoc,
				moveIgnoreAlly,
				moveIgnoreEnemy,
				attackIgnoreAlly,
				attackIgnoreEnemy
			] = ParseIgnoreFlag(ignoreFlag);

			auto ignoreCoord = [&] (const Coord& p) {
				if (GetMap(p.x, p.y, this->map) == MAP_OBSTACLE) {
					return false;
				}

				const bool curAlly = ally == nullptr ? false : findSet(*ally, p);
				const bool curEnemy = enemy == nullptr ? false : findSet(*enemy, p);

				if (attack) {
					if (!attackIgnoreAlly && curAlly) {
						return false;
					}
					if (!attackIgnoreEnemy && curEnemy) {
						return false;
					}
				}
				else {		// Repel
					if (curAlly) {
						return false;
					}
					if (curEnemy) {
						return false;
					}
				}

				return true;
			};

			auto& offset = normalNeighbour [dir];
			auto coord = start;
			size_t moveable = 0;

			while (moveable <= range) {
				if (!ignoreCoord(coord)) {
					break;
				}

				coord.x += offset.x;
				coord.y += offset.y;

				moveable++;
			}

			moveable--;			// remove last while add

			return moveable;
		}

		inline void CalcLineArea(Coord start, size_t range, const size_t startRange = 0
			, const CoordSet* ally = nullptr, const CoordSet* enemy = nullptr, CoordSet* zoc = nullptr
			, size_t ignoreFlag = DEFAULT_IGNOREFLAG // Move range ignore ally && Attack range ignore enemy by default
			, const bool attack = false) {
			if (updateMap) { UpdateMap(); }

			if (stash) {
				settings = { range,startRange,attack,false,0 };
				stashAreaKey = std::make_tuple(true, Hasher(start), Hasher(settings), ignoreFlag, Hasher(ally), Hasher(enemy), Hasher(zoc));

				if (stashArea.contains(stashAreaKey)) {
					auto& [sa, sp] = stashArea [stashAreaKey];
					area = sa;

					return;
				}
			}

			area.clear();

			if (GetMap(start.x, start.y, this->map) == MAP_OBSTACLE) {
				return;
			}

			ranges = { 0,0,0,0 };

			for (size_t dir = 0; dir < 4; dir++) {
				ranges [dir] = GetAbleLineRange(start, range, dir, ally, enemy, zoc, ignoreFlag, attack);
			}

			for (size_t nest = 0; nest <= range; nest++) {
				if (nest <= startRange) { continue; }

				area.emplace_back();
				area.back().reserve(4);

				bool updated = false;
				for (size_t it = 0; it < ranges.size(); it++) {
					if (ranges [it] >= nest) {
						area.back().emplace_back(start.x + nest * normalNeighbour [it].x
												 ,start.y + nest * normalNeighbour [it].y);

						updated = true;
					}
				}

				if (!updated) { break; }
			}

			if (stash) {
				stashArea [stashAreaKey] = std::make_tuple(area, 0);
			}

#ifdef _DEBUG
			OutPutAreaStr(start, range);
#endif // _DEBUG
		}

		inline void CalcArea(Coord start, size_t range, const size_t startRange = 0
			, CoordSet* ally = nullptr, CoordSet* enemy = nullptr, CoordSet* zoc = nullptr
			, size_t ignoreFlag = DEFAULT_IGNOREFLAG // Move range ignore ally && Attack range ignore enemy by default
			, const bool attack = false              // ignored if allRange = true
			// allRange: Calc move & attack at the same time
			, bool allRange = false, const size_t allRangeAttackRange = 0, size_t* extraRangeStartPosOut = nullptr
			, const bool diagonal = false) {
			if (updateMap) { UpdateMap(); }

			if (stash) {
				settings = { range,startRange,attack,allRange,allRangeAttackRange };
				stashAreaKey = std::make_tuple(false, Hasher(start), Hasher(settings), ignoreFlag, Hasher(ally), Hasher(enemy), Hasher(zoc));

				if (stashArea.contains(stashAreaKey)) {
					auto& [sa, sp] = stashArea[stashAreaKey];

					area = sa;

					if (extraRangeStartPosOut != nullptr) {
						*extraRangeStartPosOut = sp;
					}

					return;
				}
			}

			area.clear();

			if (GetMap(start.x, start.y, this->map) == MAP_OBSTACLE) {
				return;
			}

			cur_set.clear();
			continue_set.clear();
			extra_set.clear();

			open_set.clear();
			close_set.clear();
			point_set.clear();

			open_set.emplace_back(start);

			const auto pNeighbour = diagonal ? &diagonalNeighbour : &normalNeighbour;
			const auto neighbourSize = pNeighbour->size();

			const size_t totalRange = range + allRange * allRangeAttackRange
				+ allRange;	// Extra loop with allRange due to restart
			size_t extraRangeStartPos = range;

			bool extraRangeCalc = false;	// Extra range calc start
			bool updateAttack = allRange ? false : attack;		// ignore dynamic unit part when calc attack range
			// force to evaluate them in allRange mode

			const auto [moveIgnoreZoc,
				moveIgnoreAlly,
				moveIgnoreEnemy,
				attackIgnoreAlly,
				attackIgnoreEnemy
			] = ParseIgnoreFlag(ignoreFlag);

			//// add first grid?
			//// add start when interval == range
			//if (!updateAttack && startRange == 0) {
			//	area.emplace_back();
			//	area.back().emplace_back(start);
			//}

			for (size_t nest = 0; nest <= totalRange; nest++) {
				const bool addArea = nest > startRange;							// After start range

				const bool nextExtraRange = allRange && (nest == range);      // Update for extra range
				const bool startExtraRange = allRange && (nest == range + 1); // Extra range start, don't add new area

				if (startExtraRange) {
					updateAttack = true;									// Attack when extra range start
					extraRangeCalc = true;
					extraRangeStartPos = area.size();

					if (extraRangeStartPosOut != nullptr) {
						*extraRangeStartPosOut = extraRangeStartPos;
					}

					// add & last point
					if (!continue_set.empty()) {
						std::swap(cur_set, continue_set);
					}

					// add current area edge
					for (auto& it : area.back()) {
						if (std::ranges::find_if(cur_set,
							[&] (const Point& it_c) {
								return it_c.coord == it;
							}) == cur_set.end()) {
							cur_set.emplace_back(it);
						}
					}

					open_set.clear();
				}
				else {
					std::swap(cur_set, open_set);
				}

				if (cur_set.empty()) {
					// cannot move, start attack check in allRange mode
					if (allRange && !startExtraRange) {
						continue;
					}

					break;
				}

				if (addArea && !startExtraRange) {
					area.emplace_back();
					area.back().reserve(1 + area.size() * 4);
				}

				while (!cur_set.empty()) {
					Point base = cur_set.back();
					cur_set.pop_back();

					close_set.emplace_back(base);

					if (addArea && !startExtraRange) {
						area.back().emplace_back(base.coord);
					}

					auto add = [&] (Set& v, Point& p) {
						if (find(v, p) == nullptr) {
							v.emplace_back(p);

							return true;
						}
						else {
							return false;
						}
						};

					auto ignorePoint = [&] (Point& p) {
						const bool curAlly = ally != nullptr && findSet(*ally, p) != nullptr;
						const bool curEnemy = enemy != nullptr && findSet(*enemy, p) != nullptr;

						auto getIgnore = [&] (const bool moveIgnore, const bool attackIgnore) {
							if (updateAttack) {
								if (attackIgnore) {
									return true;
								}
							}
							else {
								if (!moveIgnore) {
									if (allRange && attackIgnore) {
										addSet(extra_set, p);		// Add to attack area if ignored during move
										add(continue_set, base);	// Add parent to continue set in case it's edge
									}
								}

								if (moveIgnore) {
									return true;
								}
							}

							return false;
							};

						return  (!curAlly && !curEnemy)		// currentPoint is not unit, ignore							
							|| (curAlly && getIgnore(moveIgnoreAlly, attackIgnoreAlly))		// ignore ally?
							|| (curEnemy && getIgnore(moveIgnoreEnemy, attackIgnoreEnemy));	// ignore enemy?
						};

					// Zoc : stop search
					// You must need zoc instead of treat them as dynamic, as you need to restart attack search in allRange mode
					if ((!moveIgnoreZoc)											// stop move if doesn't ignore zoc
						&& (!updateAttack) && (!extraRangeCalc)						// during move
						&& zoc != nullptr && findSet(*zoc, base) != nullptr		// current point zoc
						&& findSet(*ally, base) == nullptr						// current point not unit
						&& findSet(*enemy, base) == nullptr
						&& base.coord != start) {									// not start
						if (allRange) {
							add(continue_set, base);							// start calc attack range from zoc
						}

						continue;
					}

					for (size_t i = 0; i < neighbourSize; i++) {
						const auto neighCoord = Coord{ base.coord.x + (*pNeighbour)[i].x,
							base.coord.y + (*pNeighbour)[i].y };

						const BYTE curCost = GetMap(neighCoord.x, neighCoord.y, this->map);
						auto neighPoint = Point(neighCoord, base.cost + curCost + (*pNeighbour)[i].generalCost);

						if (curCost == MAP_OBSTACLE) {
							continue;
						}
						if (!ignorePoint(neighPoint)) {
							continue;
						}

						if (find(close_set, neighPoint) != nullptr) {
							continue;
						}

						if (find(cur_set, neighPoint) == nullptr
							&& find(open_set, neighPoint) == nullptr) {
							if (!nextExtraRange) {
								open_set.emplace_back(neighPoint);
							}
							else {
								add(continue_set, base);
							}
						}
					}
				}
			}

			if (!extra_set.empty()) {
				auto findArea = [&] (const Coord c) {
					for (auto& it : area) {
						for (auto& it_c : it) {
							if (it_c == c) {
								return true;
							}
						}
					}

					return false;
					};

				bool added = false;

				for (auto& it : extra_set) {
					if (!findArea(it)) {
						if (!added) {
							added = true;

							area.emplace_back();
							area.back().reserve(extra_set.size());
						}

						area.back().emplace_back(it);
					}
				}
			}

			if (stash) {
				stashArea[stashAreaKey] = make_tuple(area, extraRangeStartPos);
			}

#ifdef _DEBUG
			OutPutAreaStr(start, range, ally, enemy, zoc, allRange, extraRangeStartPos);
#endif // _DEBUG
		}

		inline const Area& GetArea() {
			return area;
		}

		inline static size_t GetIgnoreFlag(const bool moveIgnoreZoc
			, const bool moveIgnoreAlly
			, const bool moveIgnoreEnemy
			, const bool attackIgnoreAlly
			, const bool attackIgnoreEnemy) {
			return static_cast<size_t>(moveIgnoreZoc) << 4
				| static_cast<size_t>(moveIgnoreAlly) << 3
				| static_cast<size_t>(moveIgnoreEnemy) << 2
				| static_cast<size_t>(attackIgnoreAlly) << 1
				| static_cast<size_t>(attackIgnoreEnemy);
		}

		inline static auto ParseIgnoreFlag(const size_t ignoreFlag = DEFAULT_IGNOREFLAG) {
			const bool moveIgnoreZoc = ignoreFlag & 0b10000;			// Move through zoc
			const bool moveIgnoreAlly = ignoreFlag & 0b01000;			// Move through ally
			const bool moveIgnoreEnemy = ignoreFlag & 0b00100;			// Move through enemy	
			const bool attackIgnoreAlly = ignoreFlag & 0b00010;			// Attack ally (e.g., heal)	
			const bool attackIgnoreEnemy = ignoreFlag & 0b00001;		// Attack enemy

			return std::make_tuple(moveIgnoreZoc,
				moveIgnoreAlly,
				moveIgnoreEnemy,
				attackIgnoreAlly,
				attackIgnoreEnemy);
		}

		inline bool GetStash() const {
			return stash;
		}
		inline void SetStash(const bool state = true) {
			stash = state;
		}
		inline void ClearStash() {
			stashPath.clear();
			stashArea.clear();
		}
		inline size_t GetPathStashSize() const {
			return stashPath.size();
		}
		inline size_t GetAreaStashSize() const {
			return stashArea.size();
		}
	};
}
