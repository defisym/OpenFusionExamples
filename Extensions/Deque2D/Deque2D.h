#pragma once

#include <deque>
#include <string>
#include <variant>

#include <algorithm>

#include "Split.h"
#include "RandGenerator.h"

using namespace std;

enum class DataType {
	INT,DOUBLE,STRING,
};

typedef variant<int, double, wstring> Data;

typedef deque<Data> DataDeq;
typedef deque<DataDeq> DataDeq2D;

#define DELIMITER L","
#define STRQUOTATIONMARK L"\""

class Deque2D {
private:
	DataDeq2D Dat;
	
	inline size_t GetValidArrayPos(size_t ArrayPos) {
		return min(this->ArrayBackPos(), ArrayPos);
	}
	inline size_t GetValidDataPos(size_t ArrayPos,size_t Pos) {
		return min(this->DataBackPos(ArrayPos), Pos);
	}

public:
	//Save&Load
	//Load from file
	inline void Load(const wstring& FilePath, const wstring& Key, bool Unicode = true) {
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

		Split Spliter;
		Spliter.InitSplit(NEWLINE);
		Spliter.LoadData(Src);
		Spliter.SplitData();

		wstring StringRegex = L"(\")(.*)(\")";
		wstring NumberRegex = L".*\\..*";

		for (size_t i = 0; i < Spliter.GetSplitSize(); i++) {
			this->ArrayPushBack();

			Split SpliterLine;
			SpliterLine.InitSplit(DELIMITER);
			SpliterLine.LoadData(Spliter[i]);
			SpliterLine.SplitData();

			for (size_t j = 0; j < SpliterLine.GetSplitSize(); j++) {
				//is string
				if (SpliterLine.StringMatchRegex(SpliterLine[j], StringRegex.c_str(), false)) {
					this->DataPushBack(this->ArrayBackPos(), SpliterLine.ReplaceStr(SpliterLine[j], StringRegex.c_str(), L"$2"));
				}
				//is double
				else if (SpliterLine.StringMatchRegex(SpliterLine[j], NumberRegex.c_str(), false)) {
					this->DataPushBack(this->ArrayBackPos(), stod(SpliterLine[j]));
				}
				//is int
				else {
					this->DataPushBack(this->ArrayBackPos(), stoi(SpliterLine[j]));
				}
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

	//Array
	inline void ArrayClear() {
		Dat.clear();
	}
	inline void ArrayErase(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		Dat.erase(Dat.begin() + ValidArrayPos);
	}

	inline void ArrayPushBack() {
		Dat.emplace_back();
	}
	inline void ArrayPushFront() {
		Dat.emplace_front();
	}

	inline void ArrayInsert(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat.insert(Dat.begin() + ValidArrayPos, DataDeq());
	}
	inline void ArrayInsert(size_t ArrayPos, DataDeq D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat.insert(Dat.begin() + ValidArrayPos, D);
	}

	inline void ArraySetAt(size_t ArrayPos, DataDeq D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat[ValidArrayPos] = D;
	}

	inline size_t ArraySize() {
		return Dat.size();
	}
	inline size_t ArrayBackPos() {
		return ArraySize() - 1;
	}

	inline DataDeq ArrayPopFront() {
		DataDeq Ret = Dat.front();
		Dat.pop_front();
		return Ret;
	}
	inline DataDeq ArrayPopBack() {
		DataDeq Ret = Dat.back();
		Dat.pop_back();
		return Ret;
	}

	inline DataDeq ArrayAt(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		return Dat.at(ValidArrayPos);
	}

	inline void ArraySort(size_t Pos, bool descend = true) {
		//not sort if array pos don't has value
		for (size_t it = 0; it != ArraySize(); it++) {
			if (Pos != GetValidDataPos(it, Pos)) {
				break;
			}
		}

		sort(Dat.begin(), Dat.end(), [=](const DataDeq& a, const DataDeq& b) { return descend ? a[Pos] > b[Pos]: a[Pos] < b[Pos]; });
	}
	inline void  ArrayShuffle() {
		std::random_device rd;
		std::mt19937 eng{ rd() };

		shuffle(Dat.begin(), Dat.end(), eng);
	}

	//Data
	inline void DataClear(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat[ValidArrayPos].clear();
	}
	inline void DataErase(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		Dat[ValidArrayPos].erase(Dat[ValidArrayPos].begin() + ValidDataPos);
	}

	inline void DataPushBack(size_t ArrayPos, Data D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat[ValidArrayPos].emplace_back(D);
	}
	inline void DataPushFront(size_t ArrayPos, Data D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Dat[ValidArrayPos].emplace_front(D);
	}

	inline void DataInsert(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		Dat[ValidArrayPos].insert(Dat[ValidArrayPos].begin() + ValidDataPos, Data());
	}
	inline void DataInsert(size_t ArrayPos, size_t Pos, Data D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		Dat[ValidArrayPos].insert(Dat[ValidArrayPos].begin() + ValidDataPos, D);
	}

	inline void DataSetAt(size_t ArrayPos, size_t Pos, Data D) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		Dat[ValidArrayPos][ValidDataPos] = D;
	}

	inline size_t DataSize(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		return Dat[ValidArrayPos].size();
	}
	inline size_t DataBackPos(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		return DataSize(ValidArrayPos) - 1;
	}

	inline Data DataPopFront(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Data Ret = Dat[ValidArrayPos].front();
		Dat[ValidArrayPos].pop_front();
		return Ret;
	}
	inline Data DataPopBack(size_t ArrayPos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);

		Data Ret = Dat[ValidArrayPos].back();
		Dat[ValidArrayPos].pop_back();
		return Ret;
	}

	inline Data DataAt(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		return Dat[ValidArrayPos][ValidDataPos];
	}
	inline size_t DataAtType(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		return Dat[ValidArrayPos][ValidDataPos].index();
	}

	inline int DataAtInt(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		int ret;

		try {
			ret = get<int>(Dat[ValidArrayPos][ValidDataPos]);
		}
		catch (const std::bad_variant_access& e) {
			ret = 0;
		}

		return ret;
	}
	inline const int* DataAtIntPtr(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		return get_if<int>(&Dat[ValidArrayPos][ValidDataPos]);
	}

	inline double DataAtDouble(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		double ret;

		try {
			ret = get<double>(Dat[ValidArrayPos][ValidDataPos]);
		}
		catch (const std::bad_variant_access& e) {
			ret = 0.0;
		}

		return ret;
	}
	inline const double* DataAtDoublePtr(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		return get_if<double>(&Dat[ValidArrayPos][ValidDataPos]);
	}

	inline wstring DataAtString(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		wstring ret;

		try {
			ret = get<wstring>(Dat[ValidArrayPos][ValidDataPos]);
		}
		catch (const std::bad_variant_access& e) {
			ret = L"";
		}

		return ret;
	}
	inline const wstring* DataAtStringPtr(size_t ArrayPos, size_t Pos) {
		size_t ValidArrayPos = GetValidArrayPos(ArrayPos);
		size_t ValidDataPos = GetValidDataPos(ValidArrayPos, Pos);

		return get_if<wstring>(&Dat[ValidArrayPos][ValidDataPos]);
	}

	inline void DataSort(size_t ArrayPos, bool descend = true) {
		if (ArrayPos != GetValidArrayPos(ArrayPos)) {
			return;
		}

		sort(Dat[ArrayPos].begin(), Dat[ArrayPos].end(), [=](const Data& a, const Data& b) { return descend ? a > b:a < b; });
	}
	inline void DataShuffle(size_t ArrayPos) {
		if (ArrayPos != GetValidArrayPos(ArrayPos)) {
			return;
		}

		std::random_device rd;
		std::mt19937 eng{ rd() };		

		shuffle(Dat[ArrayPos].begin(), Dat[ArrayPos].end(), eng);
	}
};

