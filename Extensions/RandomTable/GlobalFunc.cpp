// Common Include
#include "common.h"

deque<byte> RandomTable;

byte GenerateRandom() {
	return rand() % 99;
}

LPWSTR Base64Encode()
{	
	DWORD dwNeed;
	LPWSTR lpBase64Str;
	byte Input[MAXSIZE];

	for (auto t = 0; t != MAXSIZE; t++) {
		Input[t] = RandomTable[t];
	}
	
	CryptBinaryToString(Input,MAXSIZE, CRYPT_STRING_BASE64,NULL, &dwNeed);

	lpBase64Str = (LPWSTR)malloc(dwNeed);
	ZeroMemory(lpBase64Str, dwNeed);

	CryptBinaryToString(Input, MAXSIZE, CRYPT_STRING_BASE64, lpBase64Str, &dwNeed);

	return lpBase64Str;
}
