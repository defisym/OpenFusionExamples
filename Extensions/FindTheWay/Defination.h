#ifndef _DEFINATION_
#define _DEFINATION_

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define ONMAPCHANGE	0

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

#endif // !_DEFINATION_
