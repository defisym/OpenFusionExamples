#pragma once

#include <string>

#define Empty_Str	_T("")
#define Default_Str	_T("")

#define StrEqu(X,Y) (wcscmp(X,Y) == 0)
#define StrEmpty(X) StrEqu(X,Empty_Str)

//You need "\\+" to escape +
#define RegStr_IsNum _T("(\\+|\\-)?[0-9]+(\\.[0-9]+)?")

#define CallEvent(X) callRunTimeFunction(rdPtr, RFUNCTION_GENERATEEVENT, X, 0);

#define valid(X) (X != nullptr)

#define InvalidStr(STR,RET) if (!valid(STR)) { return RET; }

#define release_ptr(X) if (valid(X)) {delete X; X = nullptr;}
#define release_arr(X) if (valid(X)) {delete[] X; X = nullptr;}

#define ResertPtr(X) X=nullptr;

#define RAD(DEG) ((PI*DEG)/180)
#define DEG(RAD) ((180*RAD)/PI)

#define MV rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv

#define SetExtUserData(pData) MV->mvSetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib, pData)
#define GetExtUserData() MV->mvGetExtUserData(rdPtr->rHo.hoAdRunHeader->rhApp, hInstLib)

#define TURNCATE_SHORT(v) ((short)(v) & 0x7FFF)

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

// convert string to wstring
inline std::wstring to_wide_string(const std::string& input, UINT codePage = CP_UTF8) {
	const auto size_needed = MultiByteToWideChar(codePage, 0, &input.at(0), (int)input.size(), nullptr, 0);

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(codePage, 0, &input.at(0), (int)input.size(), &result.at(0), size_needed);

	return result;
}

// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input, UINT codePage = CP_UTF8) {
	const auto size_needed = WideCharToMultiByte(codePage, 0, &input.at(0), (int)input.size(), nullptr, 0, nullptr, nullptr);

	std::string result(size_needed, 0);
	WideCharToMultiByte(codePage, 0, &input.at(0), (int)input.size(), &result.at(0), size_needed, nullptr, nullptr);

	return result;
}