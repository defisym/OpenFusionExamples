// Common Include
#include "common.h"

byte* Buffer;
DWORD StrLength;
LPWSTR lpBase64Str;
deque<byte> RandomTable;

byte GenerateRandom() {
	return rand() % 99;
}