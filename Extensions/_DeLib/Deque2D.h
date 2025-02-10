#pragma once

#include <deque>
#include <string>
#include <variant>

#include <algorithm>

#include "StrNum.h"

#include "Split.h"
#include "RandGenerator.h"

namespace Deque2D {
	enum class DataType {
		INT, DOUBLE, STRING,
	};

	typedef std::variant<int, double, std::wstring> Data;

	typedef std::deque<Data> DataDeq;
	typedef std::deque<DataDeq> DataDeq2D;

	constexpr auto CHARSTREND = L'\0';
	constexpr auto CHARDELIMITER = L',';
	constexpr auto CHARSTRQUOTATIONMARK = L'\"';

	constexpr auto STREND = L'\0';
	constexpr auto DELIMITER = L',';
	constexpr auto STRQUOTATIONMARK = L'\"';

	class Deque2D {
	private:
		DataDeq2D Dat;

		typedef struct ElementPos {
			size_t ArrayPos;
			size_t DataPos;
		}Pos;

		typedef struct SearchResult {
			Data Data;
			Pos Pos;
		}SearchResult;

		std::vector<SearchResult> SearchResultVec;

		inline size_t GetValidArrayPos(size_t ArrayPos) const {
			return (std::min)(this->ArrayBackPos(), ArrayPos);
		}
		inline size_t GetValidDataPos(size_t ArrayPos, size_t DataPos) const {
			return (std::min)(this->DataBackPos(ArrayPos), DataPos);
		}

		inline Pos GetValidPos(size_t ArrayPos, size_t DataPos) const {
			Pos ValidPos;
			ValidPos.ArrayPos = GetValidArrayPos(ArrayPos);
			ValidPos.DataPos = GetValidDataPos(ValidPos.ArrayPos, DataPos);

			return ValidPos;
		}

		//process quota escape
		static inline std::wstring EscapeDoubleToSingle(const std::wstring& src) {
			std::wstring escape;

			for (size_t pos = 0; pos != src.length(); pos++) {
				const bool end = (pos == src.length() - 1);

				const auto cur = src[pos];
				const auto next = !end ? src[pos + 1] : CHARSTREND;

				escape += src[pos];

				if ((cur == CHARSTRQUOTATIONMARK) && (next == CHARSTRQUOTATIONMARK)) {
					pos++;
				}
			}

			return escape;
		}

		static inline std::wstring EscapeSingleToDouble(const std::wstring& src) {
			std::wstring escape;

			for (const wchar_t pos : src) {
				const auto cur = pos;

				escape += pos;
				if (cur == CHARSTRQUOTATIONMARK) {
					escape += pos;
				}
			}

			return escape;
		}

		//Get token between tokenstart and tokenstart (don't include Src[tokenend])
		inline void GetToken(const std::wstring& Src, size_t tokenstart, size_t tokenend) {
			std::wstring Token;

			//String
			if (Src[tokenstart] == CHARSTRQUOTATIONMARK) {
				Token = EscapeDoubleToSingle(Src.substr(tokenstart + 1, tokenend - tokenstart - 2));
				this->DataPushBack(this->ArrayBackPos(), Token);
			}
			//Number
			else {
				Token = Src.substr(tokenstart, tokenend - tokenstart);
				this->DataPushBack(this->ArrayBackPos(), StrIsFloat(Token) ? _stod(Token) : _stoi(Token));
			}
		}

	public:
		//Save&Load
		//Load from file
		inline void Load(const std::wstring& FilePath, const std::wstring& Key, bool Unicode = true) {
			//Spliter is used here to convert code page, inorder to support UTF-8 or ANSI
			Split Spliter;
			Spliter.LoadFile(FilePath, Key, Unicode);

			this->Load(Spliter.GetSplitData());
		}
		//Load from string
		inline void Load(const std::wstring& Src) {
            if (Src.empty()) { return; }

			//Reset
			Dat.clear();
			this->ArrayPushBack();

			//Start phrase
			size_t tokenstart = 0, tokenend = 0;

			bool InStrQuota = false;
			bool CurInQuota = false;

			for (size_t pos = 0; pos != Src.length(); pos++) {
				//define vairable values
				bool End = (pos == Src.length() - 1);

				auto Cur = Src[pos];
				auto Next = !End ? Src[pos + 1] : CHARSTREND;

				//new line
				if (!InStrQuota && (Cur == L'\r') && (Next == L'\n')) {
					GetToken(Src, tokenstart, pos);
					pos++;
					tokenstart = pos + 1;

					//not end, push value
					if ((pos + 1) != Src.length()) {
						this->ArrayPushBack();
					}

					continue;
				}

				//Quota
				if (!InStrQuota && (Cur == CHARSTRQUOTATIONMARK)) {
					InStrQuota = true;
					continue;
				}

				if (InStrQuota && (Cur == CHARSTRQUOTATIONMARK)) {
					//double quota
					if (Next == CHARSTRQUOTATIONMARK) {
						pos++;

						continue;
					}
					else {
						InStrQuota = false;
					}
				}

				//get token
				if (!InStrQuota && (End || (Cur == CHARDELIMITER))) {
					GetToken(Src, tokenstart, pos + End);
					tokenstart = pos + 1;
				}
			}
		}

		//Save to file
		inline void Save(const std::wstring& FilePath, const std::wstring& Key, bool Unicode = true) {
			std::wstring Output = this->Save();
			UINT CodePage = Unicode ? CP_UTF8 : CP_ACP;

			std::string byteString;
			if(!Split::SaveData(byteString, Output.c_str(), Output.length(), CodePage)){
				return;
			};

			Encryption Encrypt;
			Encrypt.SetEncryptStr(byteString);

			if (Key == L"") {
				Encrypt.GenerateKey(Key.c_str());
				Encrypt.Encrypt();
				const auto result = Encrypt.SaveFile(FilePath.c_str());
			}
			else {
				const auto result = Encrypt.SaveFile(FilePath.c_str(), true);
			}
		}
		//Save to string
		inline std::wstring Save() {
			std::wstring Output;

			for (auto it2D = Dat.begin(); it2D != Dat.end(); ++it2D) {
				for (auto it = it2D->begin(); it != it2D->end(); ++it) {
					switch (it->index()) {
						//int
					case 0:
						Output += std::to_wstring(get<int>((*it)));
						break;
						//double
					case 1:
						Output += std::to_wstring(get<double>((*it)));
						break;
						//wstring
					case 2:
						Output += STRQUOTATIONMARK;
						Output += EscapeSingleToDouble(get<std::wstring>((*it)));
						Output += STRQUOTATIONMARK;
						break;
					default:
						break;
					}

					//end() point to the next value
					if (it != it2D->end() - 1) {
						Output += DELIMITER;
					}
					else if (it2D != Dat.end() - 1) {
						Output += NEWLINE;
					}
				}
			}

			return Output;
		}

		//Search
		inline bool LastSearchMatched() const {
			return !SearchResultVec.empty();
		}

		inline size_t GetSearchResultVecSize() const {
			return SearchResultVec.size();
		}

		inline size_t GetSearchResultArrayPos(size_t index) const {
			return SearchResultVec.at(index).Pos.ArrayPos;
		}
		inline size_t GetSearchResultDataPos(size_t index) const {
			return SearchResultVec.at(index).Pos.DataPos;
		}

		inline int GetSearchResultDataInt(size_t index) const {
			int ret = 0;

			try {
				ret = get<int>(SearchResultVec.at(index).Data);
			} catch (std::bad_variant_access& e) {
				const auto info = e.what();
			}

			return ret;
		}
		inline double GetSearchResultDataDouble(size_t index) const {
			double ret = 0.0;

			try {
				ret = get<double>(SearchResultVec.at(index).Data);
			} catch (std::bad_variant_access& e) {
				const auto info = e.what();
			}

			return ret;
		}
		inline std::wstring GetSearchResultDataString(size_t index) const {
			std::wstring ret;

			try {
				ret = get<std::wstring>(SearchResultVec.at(index).Data);
			} catch (std::bad_variant_access& e) {
				const auto info = e.what();
			}

			return ret;
		}


		//Data Operation

		//If position is invaild
		// fallback to valid pos in range when retriving values[0,size()-1]
		// stop process in other cases

		// Array
		//  ArrayAt(size_t ArrayPos)

		// Data
		//  DataAt(size_t ArrayPos, size_t DataPos)	
		//  DataAtType(size_t ArrayPos, size_t DataPos)

		//  DataAtInt(size_t ArrayPos, size_t DataPos)
		//  DataAtIntPtr(size_t ArrayPos, size_t DataPos)

		//  DataAtDouble(size_t ArrayPos, size_t DataPos)
		//  DataAtDoublePtr(size_t ArrayPos, size_t DataPos)

		//  DataAtString(size_t ArrayPos, size_t DataPos)
		//  DataAtStringPtr(size_t ArrayPos, size_t DataPos)


		//Array
		inline bool ArrayPosValid(size_t ArrayPos) const {
			if (Dat.empty()) {
				return false;
			}

			if (ArrayPos != GetValidArrayPos(ArrayPos)) {
				return false;
			}

			return true;
		}

		inline void ArrayClear() {
			Dat.clear();
		}
		inline void ArrayErase(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				Dat.erase(Dat.begin() + ArrayPos);
			}
		}

		inline void ArrayPushBack() {
			Dat.emplace_back();
		}
		inline void ArrayPushFront() {
			Dat.emplace_front();
		}

		inline void ArrayInsert(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				Dat.insert(Dat.begin() + ArrayPos, DataDeq());
			}
		}
		inline void ArrayInsert(size_t ArrayPos, DataDeq D) {
			if (ArrayPosValid(ArrayPos)) {
				Dat.insert(Dat.begin() + ArrayPos, D);
			}
		}

		inline void ArrayMove(size_t SrcArrayPos, size_t DesArrayPos) {
			if (ArrayPosValid(SrcArrayPos) && ArrayPosValid(DesArrayPos)) {
				ArrayInsert(DesArrayPos, ArrayAt(SrcArrayPos));

				if (SrcArrayPos > DesArrayPos) {
					ArrayErase(SrcArrayPos + 1);
				}
				else {
					ArrayErase(SrcArrayPos);
				}

			}
		}
		inline void ArraySwap(size_t SrcArrayPos, size_t DesArrayPos) {
			if (ArrayPosValid(SrcArrayPos) && ArrayPosValid(DesArrayPos)) {
				Dat[SrcArrayPos].swap(Dat[DesArrayPos]);
			}
		}
		inline void ArrayCopy(size_t SrcArrayPos, size_t DesArrayPos) {
			if (ArrayPosValid(SrcArrayPos) && ArrayPosValid(DesArrayPos)) {
				Dat[DesArrayPos] = Dat[SrcArrayPos];
			}
		}

		inline void ArraySetAt(size_t ArrayPos, DataDeq D) {
			if (ArrayPosValid(ArrayPos)) {
				Dat[ArrayPos] = D;
			}
		}

		inline size_t ArraySize() const {
			return Dat.size();
		}
		inline size_t ArrayBackPos() const {
			return ArraySize() - 1;
		}

		inline DataDeq ArrayPopFront() {
			if (Dat.empty()) {
				return {};
			}

			DataDeq Ret = Dat.front();
			Dat.pop_front();
			return Ret;
		}
		inline DataDeq ArrayPopBack() {
			if (Dat.empty()) {
				return {};
			}

			DataDeq Ret = Dat.back();
			Dat.pop_back();
			return Ret;
		}

		inline DataDeq ArrayAt(size_t ArrayPos) {
			if (Dat.empty()) {
				return {};
			}

			return Dat[GetValidArrayPos(ArrayPos)];
		}

		inline void ArraySort(size_t DataPos, bool descend = true) {
			//not sort if data pos doesn't has value
			for (size_t it = 0; it != ArraySize(); it++) {
				if (!DataPosValid(it, DataPos)) {
					return;
				}
			}

			std::ranges::sort(Dat, [=] (const DataDeq& a, const DataDeq& b) { return descend ? a[DataPos] > b[DataPos]: a[DataPos] < b[DataPos]; });
		}
		inline void ArrayShuffle() {
			std::random_device rd;
			std::mt19937 eng{ rd() };

			std::ranges::shuffle(Dat, eng);
		}

		inline void ArraySearch(Data Data) {
			SearchResultVec.clear();

			size_t ArrayPos = 0;

			for (auto& it : Dat) {
				DataSearch(Data, ArrayPos);
				ArrayPos++;
			}
		}
		inline void ArraySearchStr(std::wstring Regex) {
			SearchResultVec.clear();

			size_t ArrayPos = 0;

			for (auto& it : Dat) {
				DataSearchStr(Regex, ArrayPos);
				ArrayPos++;
			}
		}

		//Data
		inline bool DataPosValid(size_t ArrayPos, size_t DataPos) {
			if (!ArrayPosValid(ArrayPos)) {
				return false;
			}
			else if (Dat[ArrayPos].empty()) {
				return false;
			}
			else if (DataPos != GetValidDataPos(ArrayPos, DataPos)) {
				return false;
			}
			else {
				return true;
			}
		}

		inline void DataClear(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				Dat[ArrayPos].clear();
			}
		}
		inline void DataErase(size_t ArrayPos, size_t DataPos) {
			if (DataPosValid(ArrayPos, DataPos)) {
				Dat[ArrayPos].erase(Dat[ArrayPos].begin() + DataPos);
			}
		}

		inline void DataPushBack(size_t ArrayPos, Data D) {
			if (ArrayPosValid(ArrayPos)) {
				Dat[ArrayPos].emplace_back(D);
			}
		}
		inline void DataPushFront(size_t ArrayPos, Data D) {
			if (ArrayPosValid(ArrayPos)) {
				Dat[ArrayPos].emplace_front(D);
			}
		}

		inline void DataInsert(size_t ArrayPos, size_t DataPos) {
			if (DataPosValid(ArrayPos, DataPos)) {
				Dat[ArrayPos].insert(Dat[ArrayPos].begin() + DataPos, Data());
			}
		}
		inline void DataInsert(size_t ArrayPos, size_t DataPos, Data D) {
			if (DataPosValid(ArrayPos, DataPos)) {
				Dat[ArrayPos].insert(Dat[ArrayPos].begin() + DataPos, D);
			}
		}

		inline void DataMove(size_t ArrayPos, size_t SrcDataPos, size_t DesDataPos) {
			if (DataPosValid(ArrayPos, SrcDataPos) && DataPosValid(ArrayPos, DesDataPos)) {
				DataInsert(ArrayPos, DesDataPos, DataAt(ArrayPos, SrcDataPos));

				if (SrcDataPos > DesDataPos) {
					DataErase(ArrayPos, SrcDataPos + 1);
				}
				else {
					DataErase(ArrayPos, SrcDataPos);
				}

			}
		}
		inline void DataSwap(size_t ArrayPos, size_t SrcDataPos, size_t DesDataPos) {
			if (DataPosValid(ArrayPos, SrcDataPos) && DataPosValid(ArrayPos, DesDataPos)) {
				Dat[SrcDataPos].swap(Dat[DesDataPos]);
			}
		}
		inline void DataCopy(size_t ArrayPos, size_t SrcDataPos, size_t DesDataPos) {
			if (DataPosValid(ArrayPos, SrcDataPos) && DataPosValid(ArrayPos, DesDataPos)) {
				Dat[DesDataPos] = Dat[SrcDataPos];
			}
		}

		inline void DataSetAt(size_t ArrayPos, size_t DataPos, Data D) {
			if (DataPosValid(ArrayPos, DataPos)) {
				Dat[ArrayPos][DataPos] = D;
			}
		}

		inline size_t DataSize(size_t ArrayPos) const {
			if (ArrayPosValid(ArrayPos)) {
				return Dat[ArrayPos].size();
			}

			return 0;
		}
		inline size_t DataBackPos(size_t ArrayPos) const {
			if (ArrayPosValid(ArrayPos)) {
				return DataSize(ArrayPos) - 1;
			}

			return 0;
		}

		inline Data DataPopFront(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				if (Dat[ArrayPos].empty()) {
					return 0;
				}

				Data Ret = Dat[ArrayPos].front();
				Dat[ArrayPos].pop_front();
				return Ret;
			}

			return 0;
		}
		inline Data DataPopBack(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				if (Dat[ArrayPos].empty()) {
					return 0;
				}

				Data Ret = Dat[ArrayPos].back();
				Dat[ArrayPos].pop_back();
				return Ret;
			}

			return 0;
		}

		inline Data DataAt(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return Dat[ValidPos.ArrayPos][ValidPos.DataPos];
			}
			else {
				return 0;
			}
		}
		inline Data* DataAtPtr(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return &Dat[ValidPos.ArrayPos][ValidPos.DataPos];
			}
			else {
				return nullptr;
			}
		}

		inline size_t DataAtType(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return Dat[ValidPos.ArrayPos][ValidPos.DataPos].index();
			}
			else {
				return -1;
			}
		}

		inline int DataAtInt(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			int ret = 0;

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				try {
					ret = get<int>(Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
				} catch (std::bad_variant_access& e) {
					const auto info = e.what();
				}
			}

			return ret;
		}
		inline const int* DataAtIntPtr(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return get_if<int>(&Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
			}
			else {
				return nullptr;
			}
		}

		inline double DataAtDouble(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			double ret = 0.0;

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				try {
					ret = get<double>(Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
				} catch (std::bad_variant_access& e) {
					const auto info = e.what();
				}
			}

			return ret;
		}
		inline const double* DataAtDoublePtr(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return get_if<double>(&Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
			}
			else {
				return nullptr;
			}
		}

		inline std::wstring DataAtString(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			std::wstring ret = L"";

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				try {
					ret = get<std::wstring>(Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
				} catch (std::bad_variant_access& e) {
					const auto info = e.what();
				}
			}

			return ret;
		}
		inline const std::wstring* DataAtStringPtr(size_t ArrayPos, size_t DataPos) {
			Pos ValidPos = GetValidPos(ArrayPos, DataPos);

			if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
				return get_if<std::wstring>(&Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
			}
			else {
				return nullptr;
			}
		}

		inline void DataSort(size_t ArrayPos, bool descend = true) {
			if (ArrayPosValid(ArrayPos)) {
				std::ranges::sort(Dat[ArrayPos], [=] (const Data& a, const Data& b) { return descend ? a > b:a < b; });
			}
		}
		inline void DataShuffle(size_t ArrayPos) {
			if (ArrayPosValid(ArrayPos)) {
				std::random_device rd;
				std::mt19937 eng{ rd() };

				std::ranges::shuffle(Dat[ArrayPos], eng);
			}
		}

		inline void DataSearch(Data Data, size_t ArrayPos) {
			size_t DataPos = 0;

			if (ArrayPosValid(ArrayPos)) {
				for (auto& it : Dat[ArrayPos]) {
					if (it == Data) {
						this->SearchResultVec.emplace_back(SearchResult{ it,Pos{ArrayPos,DataPos} });
					}

					DataPos++;
				}
			}
		}
		inline void DataSearchStr(std::wstring Regex, size_t ArrayPos) {
			size_t DataPos = 0;
			wregex Reg(Regex);

			if (ArrayPosValid(ArrayPos)) {
				for (auto& it : Dat[ArrayPos]) {
					if ((it.index() == static_cast<size_t>(DataType::STRING)) && regex_match(get<std::wstring>(it), Reg)) {
						this->SearchResultVec.emplace_back(SearchResult{ it,Pos{ArrayPos,DataPos} });
					}

					DataPos++;
				}
			}
		}
	};
}