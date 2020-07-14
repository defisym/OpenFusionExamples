#pragma once

#ifndef _GLOBALFUNC_
#define	_GLOBALFUNC_

#include <deque>

using namespace std;

#define MAXSIZE 50
extern 	deque<char> RandomTable;

char GenerateRandom();

void PushWithRandomLoop();

#endif // !_GLOBALFUNC_

