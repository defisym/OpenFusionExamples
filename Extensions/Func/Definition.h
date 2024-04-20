#pragma once

enum class DataType { VALUE, STRING };

struct Data {
	float Val = 0.0;
	std::wstring Str = L"0";

	DataType Type = DataType::VALUE;
	bool Converted = true;
	bool IsNumber = true;
	bool IsNumberChecked = true;

	bool bDefault = false;

	Data() {
		this->bDefault = true;
	}

	Data(float Val) {
		this->Val = Val;
		this->Str = std::wstring();
		this->Converted = false;
	}
	Data(const std::wstring& Str) {
		this->Str = Str;
		this->Type = DataType::STRING;
		this->Converted = false;
		this->IsNumber = false;
		this->IsNumberChecked = false;
	}
	Data(const std::wstring_view& Str) :Data(std::wstring(Str)) {}
};

typedef std::vector<std::wstring> VEC;
typedef VEC* LPVEC;

typedef std::unordered_map<std::wstring, size_t> LIDX;	//loop index
typedef LIDX* LPLIDX;

typedef std::vector<Data> PARAMVEC;
typedef PARAMVEC* LPPARAMVEC;

typedef std::vector<PARAMVEC> PARAMSTACK;
typedef PARAMSTACK* LPPARAMSTACK;

typedef std::unordered_map<std::wstring, std::wstring> MANGLINGNAME;
typedef MANGLINGNAME* LPMANGLINGNAME;

typedef std::unordered_map<std::wstring, Data> PARAMMAP;
typedef PARAMMAP* LPPARAMMAP;

typedef std::unordered_map<std::wstring, PARAMMAP> TPARAM;
typedef TPARAM* LPTPARAM;

typedef std::wstring STRING;
typedef STRING* LPSTRING;

constexpr auto DefaultVecSize = 20;

constexpr auto Delimiter = L'|';

constexpr auto ONFUNC = 0;
constexpr auto ONITOBJ = 9;
