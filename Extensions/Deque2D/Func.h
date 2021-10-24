#ifndef _FUNC_
#define _FUNC_

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(LPTSTR& Tar, LPCTSTR Str) {
	release_arr(Tar);
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPTSTR& Tar, const std::wstring& Str) {
	NewStr(Tar, Str.c_str());
}

//if you input \r\n in MMF, fusion will convert it to \\r\\n, which not match \r\n, so we convert it back here
inline const std::wstring NewLineEscape(const wchar_t* Src) {
	std::wregex NewLineEscape(_T("(\\\\r\\\\n)"));
	return std::regex_replace(Src, NewLineEscape, L"\r\n").c_str();
}

inline bool StrIsNum(const wchar_t* Src) {
	std::wregex Regex(RegStr_IsNum);
	return std::regex_match(Src, Regex);
}

inline bool StrIsNum(const std::wstring& Src) {
	std::wregex Regex(RegStr_IsNum);
	return std::regex_match(Src, Regex);
}

inline long ReturnFloat(LPRDATA rdPtr, float Val) {
	if (Val == (int)Val) {
		return (int)Val;
	}
	else {
		//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
		rdPtr->rHo.hoFlags |= HOF_FLOAT;

		//Return the float without conversion
		return *((long*)&Val);
	}
}

#define ReturnFloat(Val) ReturnFloat(rdPtr,Val)

#endif // !_FUNC_

