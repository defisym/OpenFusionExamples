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

#define END L'\0'
#define DELIMITER L','
#define STRQUOTATIONMARK L'\"'

class Deque2D {
private:
	DataDeq2D Dat;

	typedef struct {
		size_t ArrayPos;
		size_t DataPos;
	}Pos;

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

public:
	//Save&Load
	//Load from file
	inline void Load(const wstring& FilePath, const wstring& Key, bool Unicode = true) {
		//Spliter is used here to convert code page, inorder to support UTF-8 or ANSI
		Split Spliter;
		Spliter.SetUnicode(Unicode);
		Spliter.OpenFile(FilePath.c_str());

		if (Key != L"") {
			Spliter.GenerateKey(Key.c_str());
			Spliter.Decrypt();
			Spliter.LoadData(Spliter.GetOutputStr());
		}
		else {
			Spliter.LoadData(Spliter.GetInputStr());
		}

		this->Load(Spliter.GetSplitData());
	}
	//Load from string
	inline void Load(const wstring& Src) {
		Dat.clear();

		//prase
		size_t start = Src.find_first_not_of(NEWLINE), end = start;
		
		while (start != std::wstring::npos) {
			// Find next occurence of delimiter
			end = Src.find(NEWLINE, start);
			
			// Process
			this->ArrayPushBack();

			std::wstring Line = Src.substr(start, end - start);

			size_t tokenstart = 0, tokenend = 0, pos = 0;
			bool InStrQuota = false;
			bool CurInQuota = false;
			
			for (size_t pos = 0; pos !=Line.length(); pos++) {
				bool End = (pos == Line.length() - 1);
				
				auto Cur = Line[pos];
				auto Next= End?Line[pos+1]:END;


				if ((Line[pos] == STRQUOTATIONMARK)&& (Line[pos+1] != STRQUOTATIONMARK)) {
					if (!InStrQuota) {
						tokenstart++;
					}

					InStrQuota = !InStrQuota;
				}

				if (!InStrQuota && ((Line[pos] == DELIMITER) || (pos == Line.length() - 1))) {
					tokenend = pos;
					std::wstring Token = Line.substr(tokenstart, tokenend - tokenstart);
					tokenstart = pos + 1;
				}
			}

			// Skip all occurences of the delimiter to find new start
			start = Src.find_first_not_of(NEWLINE, end);
		}



		//Split Spliter;
		//Spliter.InitSplit(NEWLINE);
		//Spliter.LoadData(Src);
		//Spliter.SplitData();

		//wstring StringRegex = L"(\")(.*)(\")";
		//wstring DoubleRegex = L".*\\..*";

		//for (size_t i = 0; i < Spliter.GetSplitSize(); i++) {
		//	this->ArrayPushBack();

		//	Split SpliterLine;
		//	SpliterLine.InitSplit(DELIMITER);
		//	SpliterLine.LoadData(Spliter[i]);
		//	SpliterLine.SplitData();

		//	for (size_t j = 0; j < SpliterLine.GetSplitSize(); j++) {
		//		//is string
		//		if (SpliterLine.StringMatchRegex(SpliterLine[j], StringRegex.c_str(), false)) {
		//			this->DataPushBack(this->ArrayBackPos(), SpliterLine.ReplaceStr(SpliterLine[j], StringRegex.c_str(), L"$2"));
		//		}
		//		//is double
		//		else if (SpliterLine.StringMatchRegex(SpliterLine[j], DoubleRegex.c_str(), false)) {
		//			this->DataPushBack(this->ArrayBackPos(), stod(SpliterLine[j]));
		//		}
		//		//is int
		//		else {
		//			this->DataPushBack(this->ArrayBackPos(), stoi(SpliterLine[j]));
		//		}
		//	}

		//}
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
					Output += STRQUOTATIONMARK + get<wstring>((*it)) + STRQUOTATIONMARK;
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
};

