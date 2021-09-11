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

#endif // !_DEFINATION_
