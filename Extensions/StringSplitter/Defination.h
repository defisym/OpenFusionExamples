#ifndef _DEFINATION_
#define _DEFINATION_

#define OStr rdPtr->Str

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define ONIT_SSV	0
#define ONIT_KWM	1
#define ONIT_MSS	2

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);

#define valid(X) (X != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

#define release_str() release_arr(rdPtr->Str);release_arr(rdPtr->SplitStrVecLoopName);release_arr(rdPtr->CurrentSplitStr);release_arr(rdPtr->KeyWordMapLoopName);release_arr(rdPtr->CurrentKeyWord);release_arr(rdPtr->SubStringVecLoopName);release_arr(rdPtr->CurrentSubString);

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

#endif // !_DEFINATION_
