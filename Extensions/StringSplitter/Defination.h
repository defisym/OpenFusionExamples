#ifndef _DEFINATION_
#define _DEFINATION_

#define Spliter rdPtr->S

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define ONIT_SSV	0
#define ONIT_KWPV	1
#define ONIT_MSS	2

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);

#define valid(X) (X != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

//Need not to release currents cause they are pointers pointed to Split class's variable value
#define release_str() release_arr(rdPtr->SplitStrVecLoopName);release_arr(rdPtr->KeyWordPairVecLoopName);release_arr(rdPtr->SubStringVecLoopName);

#define ResertPtr(X) X=nullptr;

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

#endif // !_DEFINATION_
