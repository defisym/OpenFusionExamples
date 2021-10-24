#include "GlobalFunc.h"

byte* Buffer;
DWORD StrLength;
LPWSTR lpBase64Str;
deque<byte> RandomTable;
RandGenerator<int> R(0, 99);

byte GenerateRandom() {
	//return rand() % 99;
	return (byte)R.GetRandNumber();
}

int _GetRandomNumber()
{
	int result = RandomTable[0];
	RandomTable.pop_front();
	RandomTable.push_back(GenerateRandom());
	return result;
}
