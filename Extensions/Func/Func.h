#ifndef _FUNC_
#define _FUNC_

//don't use this func if Str = nullptr, return Default_Str directly
inline void NewStr(LPTSTR& Tar, LPCTSTR Str) {
	release_arr(Tar);
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
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

inline void UpdateParam(LPRDATA rdPtr, std::wstring& Param) {
	std::wregex Regex(L"\\|");

	//iterate lines
	std::wsregex_token_iterator pos(Param.begin(), Param.end(), Regex, -1);
	std::wsregex_token_iterator end;

	for (; pos != end; pos++) {
		//current str
		std::wstring Tmp = pos->str();
		rdPtr->FuncParamStack->back().emplace_back(Tmp);
	}
}

#endif // !_FUNC_

