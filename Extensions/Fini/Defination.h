#ifndef _DEFINATION_
#define _DEFINATION_

typedef CSimpleIni INI;
typedef CSimpleIni* LPINI;

typedef CSimpleIni::TNamesDepend INILIST;
typedef CSimpleIni::TNamesDepend* LPINILIST;

typedef CSimpleIni::TNamesDepend::const_iterator INIIT;

#define Fini rdPtr->ini
#define OStr rdPtr->Str

#define FLOAT_MAX 50+1

#define Empty_Str	_T("")
#define Default_Str	_T("")
#define Default_Val	_T("0")

#define ONIT_SEC	0
#define ONIT_ITEM	1

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);

#define valid(X) (X != nullptr)

#define invalid(X) if (!valid(Fini)) { return X; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

#define release_str() release_arr(rdPtr->SecLoopName);release_arr(rdPtr->ItemLoopName);release_arr(rdPtr->CurrentSec);release_arr(rdPtr->CurrentItem);release_arr(rdPtr->AutoSaveFilePath);release_arr(rdPtr->AutoSaveKey);

#define release_ini() release_ptr(Fini);release_str();rdPtr->Modified = false;
#define init_ini() release_ini();Fini = new INI;Fini->SetUnicode();

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)
#define InvalidSec(X) if (StrEmpty(Section)) { return X; }
#define InvalidSecItem(X) if (StrEmpty(Section) || StrEmpty(Item)) { return X; }

//You need "\\+" to escape +
#define RegStr_IsNum _T("\\+[0-9]+(.[0-9]+)?|-[0-9]+(.[0-9]+)?|[0-9]+(.[0-9]+)?")

#endif // !_DEFINATION_
