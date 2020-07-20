// Common Include
#include "common.h"

byte* Buffer;
DWORD StrLength;
LPWSTR lpBase64Str;
deque<byte> RandomTable;

byte GenerateRandom() {
	return rand() % 99;
}

int _GetRandomNumber()
{
	int result = RandomTable[0];
	RandomTable.pop_front();
	RandomTable.push_back(GenerateRandom());
	return result;
}
