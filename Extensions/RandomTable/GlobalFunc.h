#pragma once

#ifndef _GLOBALFUNC_
#define	_GLOBALFUNC_

#include <deque>

#include <wincrypt.h>
#pragma comment(lib,"crypt32.lib")

using namespace std;

//#define MAXSIZE 50
#define MAXSIZE 5

extern 	deque<byte> RandomTable;

byte GenerateRandom();

LPWSTR Base64Encode();

#endif // !_GLOBALFUNC_

