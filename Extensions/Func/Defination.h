#ifndef _DEFINATION_
#define _DEFINATION_

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

typedef std::unordered_map<std::wstring, size_t> LIDX;
typedef LIDX* LPLIDX;

typedef std::vector<Data> PARAMVEC;
typedef PARAMVEC* LPPARAMVEC;

typedef std::vector<PARAMVEC> PARAMSTACK;
typedef PARAMSTACK* LPPARAMSTACK;

typedef std::unordered_map<std::wstring, std::unordered_map<std::wstring, Data>> TPARAM;
typedef TPARAM* LPTPARAM;

typedef std::wstring STRING;
typedef STRING* LPSTRING;

#define DefaultVecSize 20
#define DoubleStrSize 50+1

#define OStr rdPtr->OutPut

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define Delimiter _T("|")

#define ONFUNC	0

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);

#define valid(X) (X != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

#define ResertPtr(X) X=nullptr;

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

//You need "\\+" to escape +
#define RegStr_IsNum _T("\\+[0-9]+(.[0-9]+)?|-[0-9]+(.[0-9]+)?|[0-9]+(.[0-9]+)?")

#define GetParam(Pos) rdPtr->FuncParamStack->back().at(Pos)

#define HasTempParam(FuncName, ParamName) (((*rdPtr->FuncTempParamStack).count(FuncName) != 0)&&((*rdPtr->FuncTempParamStack)[FuncName].count(ParamName) != 0))
#define TempParam(FuncName, ParamName) (*rdPtr->FuncTempParamStack)[FuncName][ParamName]

#define Return(Pos) rdPtr->FuncReturn->at(Pos)

#endif // !_DEFINATION_
