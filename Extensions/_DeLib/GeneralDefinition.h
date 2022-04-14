#pragma once

#include <string>

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

#define valid(X) (X != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

#define RAD(DEG) ((PI*DEG)/180)
#define DEG(RAD) ((180*RAD)/PI)

#define IDENTIFIER_ACTIVE 1230131283

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(LPTSTR & Tar, LPCTSTR Str) {
	release_arr(Tar);
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPTSTR & Tar, const std::wstring & Str) {
	NewStr(Tar, Str.c_str());
}