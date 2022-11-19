#pragma once

enum class DataType { VALUE, STRING };

typedef struct Data {
	float Val = 0.0;
	std::wstring Str;

	DataType Type = DataType::VALUE;
	bool Converted = false;
	bool IsNumber = false;
	bool IsNumberChecked = false;
}Data;

//#define Data_Val(Val) Data{ Val, std::wstring(DoubleStrSize, L'\0'), DataType::VALUE, false, true, true}
#define Data_Val(Val) Data{ Val, std::wstring(), DataType::VALUE, false, true, true}
#define Data_Str(Str) Data{ 0, Str, DataType::STRING, false, false, false}

typedef std::vector<std::wstring> VEC;
typedef VEC* LPVEC;

typedef std::unordered_map<std::wstring, size_t> LIDX;	//loop index
typedef LIDX* LPLIDX;

typedef std::vector<Data> PARAMVEC;
typedef PARAMVEC* LPPARAMVEC;

typedef std::vector<PARAMVEC> PARAMSTACK;
typedef PARAMSTACK* LPPARAMSTACK;

typedef std::unordered_map<std::wstring, Data> PARAMMAP;
typedef PARAMMAP* LPPARAMMAP;

typedef std::unordered_map<std::wstring, PARAMMAP> TPARAM;
typedef TPARAM* LPTPARAM;

typedef std::wstring STRING;
typedef STRING* LPSTRING;

#define DefaultVecSize 20

#define OStr rdPtr->OutPut

#define Delimiter _T("|")

#define ONFUNC	0
#define ONITOBJ	9

#define GetParam(Pos) rdPtr->FuncParamStack->back().at(Pos)

#define HasTempParam(FuncName, ParamName) (((*rdPtr->FuncTempParam).count(GetFuncNameWithRecursiveID(FuncName)) != 0)	\
								&&((*rdPtr->FuncTempParam)[GetFuncNameWithRecursiveID(FuncName)].count(ParamName) != 0))
#define TempParam(FuncName, ParamName) (*rdPtr->FuncTempParam)[GetFuncNameWithRecursiveID(FuncName)][ParamName]

//#define Return(Pos) rdPtr->FuncReturn->at(Pos)
#define Return(Pos) GetReturn(rdPtr, Pos)
