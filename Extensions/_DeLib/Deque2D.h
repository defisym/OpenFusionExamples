#pragma once

#include <deque>
#include <string>
#include <variant>

#include <algorithm>

#include "StrNum.h"

#include "Split.h"
#include "RandGenerator.h"

using namespace std;

enum class DataType {
	INT, DOUBLE, STRING,
};

typedef variant<int, double, wstring> Data;

typedef deque<Data> DataDeq;
typedef deque<DataDeq> DataDeq2D;

#define CHARSTREND L'\0'
#define CHARDELIMITER L','
#define CHARSTRQUOTATIONMARK L'\"'

#define STREND L'\0'
#define DELIMITER L','
#define STRQUOTATIONMARK L'\"'

class Deque2D {
private:
	DataDeq2D Dat;

	typedef struct ElementPos{
		size_t ArrayPos;
		size_t DataPos;
	}Pos;

	typedef struct SearchResult {
		Data Data;
		Pos Pos;
	}SearchResult;

	vector<SearchResult> SearchResultVec;

	inline size_t GetValidArrayPos(size_t ArrayPos) {
		return min(this->ArrayBackPos(), ArrayPos);
	}
	inline size_t GetValidDataPos(size_t ArrayPos, size_t DataPos) {
		return min(this->DataBackPos(ArrayPos), DataPos);
	}

	inline Pos GetValidPos(size_t ArrayPos, size_t DataPos) {
		Pos ValidPos;
		ValidPos.ArrayPos = GetValidArrayPos(ArrayPos);
		ValidPos.DataPos = GetValidDataPos(ValidPos.ArrayPos, DataPos);

		return ValidPos;
	}

	//process quota escape
	inline std::wstring EscapeDoubleToSingle(const std::wstring& Src) {
		std::wstring Escape;

		for (size_t pos = 0; pos != Src.length(); pos++) {
			bool End = (pos == Src.length() - 1);

			auto Cur = Src[pos];
			auto Next = !End ? Src[pos + 1] : CHARSTREND;

			Escape += Src[pos];

			if ((Cur == CHARSTRQUOTATIONMARK) && (Next == CHARSTRQUOTATIONMARK)) {
				pos++;
			}
		}

		return Escape;
	}
	inline std::wstring EscapeSingleToDouble(const std::wstring& Src) {
		std::wstring Escape;

		for (size_t pos = 0; pos != Src.length(); pos++) {
			auto Cur = Src[pos];

			Escape += Src[pos];
			if (Cur == CHARSTRQUOTATIONMARK) {
				Escape += Src[pos];
			}
		}

		return Escape;
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
	inline void Load(const wstring& FilePath, const wstring& Key, bool Unicode = true) {
		//Spliter is used here to convert code page, inorder to support UTF-8 or ANSI
		Split Spliter;
		Spliter.LoadFile(FilePath, Key, Unicode);

		this->Load(Spliter.GetSplitData());
	}
	//Load from string
	inline void Load(const wstring& Src) {
		if (Src.length() == 0) {
			return;
		}

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
				tokenstart = pos+1;				

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
			if (!InStrQuota && (End||(Cur == CHARDELIMITER))) {
				GetToken(Src, tokenstart, pos + End);
				tokenstart = pos + 1;
			}
		}
	}

	//Save to file
	inline void Save(const wstring& FilePath, const wstring& Key, bool Unicode = true) {
		wstring Output = this->Save();
		UINT CodePage = Unicode ? CP_UTF8 : CP_ACP;

		DWORD dbuffsize = WideCharToMultiByte(CodePage, 0, Output.c_str(), -1, NULL, 0, NULL, FALSE);
		if (dbuffsize == (size_t)(-1)) {
			return;
		}

		char* dbuff = new char[dbuffsize + 1];
		memset(dbuff, 0, dbuffsize + 1);

		if (!WideCharToMultiByte(CodePage, 0, Output.c_str(), -1, dbuff, dbuffsize, NULL, FALSE)) {
			return;
		}

		Split Spliter;
		Spliter.SetEncryptStr(dbuff, strlen(dbuff));
		delete[] dbuff;

		if (Key != L"") {
			Spliter.GenerateKey(Key.c_str());
			Spliter.Encrypt();
			Spliter.SaveFile(FilePath.c_str());
		}
		else {
			Spliter.SaveFile(FilePath.c_str(), true);
		}
	}
	//Save to string
	inline wstring Save() {
		wstring Output;

		for (auto it2D = Dat.begin(); it2D != Dat.end(); it2D++) {
			for (auto it = (*it2D).begin(); it != (*it2D).end(); it++) {
				switch ((*it).index()) {
					//int
				case 0:
					Output += to_wstring(get<int>((*it)));
					break;
					//double
				case 1:
					Output += to_wstring(get<double>((*it)));
					break;
					//wstring
				case 2:
					Output += STRQUOTATIONMARK + EscapeSingleToDouble(get<wstring>((*it))) + STRQUOTATIONMARK;
					break;
				}

				//end() point to the next value
				if (it != (*it2D).end() - 1) {
					Output += DELIMITER;
				}
				else if (it2D != Dat.end() - 1) {
					Output += NEWLINE;
				}
			}
		}

		return Output;
	}

	inline size_t GetSearchResultVecSize() {
		return SearchResultVec.size();
	}

	inline size_t GetSearchResultArrayPos(size_t index) {
		return SearchResultVec.at(index).Pos.ArrayPos;
	}
	inline size_t GetSearchResultDataPos(size_t index) {
		return SearchResultVec.at(index).Pos.DataPos;
	}

	inline int GetSearchResultDataInt(size_t index) {
		int ret = 0;

		try {
			ret = get<int>(SearchResultVec.at(index).Data);
		}
		catch (bad_variant_access) {
			//
		}
		
		return ret;
	}
	inline double GetSearchResultDataDouble(size_t index) {
		double ret = 0.0;

		try {
			ret = get<double>(SearchResultVec.at(index).Data);
		}
		catch (bad_variant_access) {
			//
		}

		return ret;
	}
	inline wstring GetSearchResultDataString(size_t index) {
		wstring ret = L"";

		try {
			ret = get<wstring>(SearchResultVec.at(index).Data);
		}
		catch (bad_variant_access) {
			//
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
	inline bool ArrayPosValid(size_t ArrayPos) {
		if (Dat.empty()) {
			return false;
		}
		else if (ArrayPos != GetValidArrayPos(ArrayPos)) {
			return false;
		}
		else {
			return true;
		}
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

	inline size_t ArraySize() {
		return Dat.size();
	}
	inline size_t ArrayBackPos() {
		return ArraySize() - 1;
	}

	inline DataDeq ArrayPopFront() {
		if (Dat.empty()) {
			return DataDeq();
		}

		DataDeq Ret = Dat.front();
		Dat.pop_front();
		return Ret;
	}
	inline DataDeq ArrayPopBack() {
		if (Dat.empty()) {
			return DataDeq();
		}

		DataDeq Ret = Dat.back();
		Dat.pop_back();
		return Ret;
	}

	inline DataDeq ArrayAt(size_t ArrayPos) {
		if (Dat.empty()) {
			return DataDeq();
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

		sort(Dat.begin(), Dat.end(), [=](const DataDeq& a, const DataDeq& b) { return descend ? a[DataPos] > b[DataPos]: a[DataPos] < b[DataPos]; });
	}
	inline void ArrayShuffle() {
		std::random_device rd;
		std::mt19937 eng{ rd() };

		shuffle(Dat.begin(), Dat.end(), eng);
	}

	inline void ArraySearch(Data Data) {
		SearchResultVec.clear();

		size_t ArrayPos = 0;

		for (auto& it : Dat) {
			DataSearch(Data,ArrayPos);
			ArrayPos++;
		}
	}
	inline void ArraySearchStr(wstring Regex) {
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

	inline size_t DataSize(size_t ArrayPos) {
		if (ArrayPosValid(ArrayPos)) {
			return Dat[ArrayPos].size();
		}

		return 0;
	}
	inline size_t DataBackPos(size_t ArrayPos) {
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
			}
			catch (bad_variant_access) {
				//
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
			}
			catch (bad_variant_access) {
				//
			}
		}

		return ret;
	}
	inline const double* DataAtDoublePtr(size_t ArrayPos, size_t DataPos) {
		Pos ValidPos = GetValidPos(ArrayPos, DataPos);

		if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
			return get_if<double>(&Dat[ValidPos.ArrayPos][ValidPos.DataPos]);;
		}
		else {
			return nullptr;
		}
	}

	inline wstring DataAtString(size_t ArrayPos, size_t DataPos) {
		Pos ValidPos = GetValidPos(ArrayPos, DataPos);

		wstring ret = L"";

		if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
			try {
				ret = get<wstring>(Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
			}
			catch (bad_variant_access) {
				//
			}
		}

		return ret;
	}
	inline const wstring* DataAtStringPtr(size_t ArrayPos, size_t DataPos) {
		Pos ValidPos = GetValidPos(ArrayPos, DataPos);

		if (DataPosValid(ValidPos.ArrayPos, ValidPos.DataPos)) {
			return get_if<wstring>(&Dat[ValidPos.ArrayPos][ValidPos.DataPos]);
		}
		else {
			return nullptr;
		}
	}

	inline void DataSort(size_t ArrayPos, bool descend = true) {
		if (ArrayPosValid(ArrayPos)) {
			sort(Dat[ArrayPos].begin(), Dat[ArrayPos].end(), [=](const Data& a, const Data& b) { return descend ? a > b:a < b; });
		}
	}
	inline void DataShuffle(size_t ArrayPos) {
		if (ArrayPosValid(ArrayPos)) {
			std::random_device rd;
			std::mt19937 eng{ rd() };

			shuffle(Dat[ArrayPos].begin(), Dat[ArrayPos].end(), eng);
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
	inline void DataSearchStr(wstring Regex, size_t ArrayPos) {
		size_t DataPos = 0;
		std::wregex Reg(Regex);

		if (ArrayPosValid(ArrayPos)) {
			for (auto& it : Dat[ArrayPos]) {				
				if ((it.index() == (size_t)DataType::STRING) && std::regex_match(get<wstring>(it), Reg)) {
					this->SearchResultVec.emplace_back(SearchResult{ it,Pos{ArrayPos,DataPos} });
				}

				DataPos++;
			}
		}
	}
};

