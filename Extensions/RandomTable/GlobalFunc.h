#pragma once

#ifndef _GLOBALFUNC_
#define	_GLOBALFUNC_

#include <deque>

#include <wincrypt.h>
#pragma comment(lib,"crypt32.lib")

using namespace std;

extern byte* Buffer;
extern DWORD StrLength;
extern LPWSTR lpBase64Str;
extern deque<byte> RandomTable;

byte GenerateRandom();
int _GetRandomNumber();

#endif // !_GLOBALFUNC_

