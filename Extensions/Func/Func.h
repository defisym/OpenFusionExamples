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

inline void UpdateReturn(LPRDATA rdPtr, std::wstring& Param) {
	std::wregex Regex(L"\\|");

	//iterate lines
	std::wsregex_token_iterator pos(Param.begin(), Param.end(), Regex, -1);
	std::wsregex_token_iterator end;

	for (; pos != end; pos++) {
		//current str
		std::wstring Tmp = pos->str();
		rdPtr->FuncReturn->emplace_back(Tmp);
	}
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

#define ReturnFloat(Val) ReturnFloat(rdPtr, Val)

inline void CallFuncCore(LPRDATA rdPtr, std::wstring& FuncName, std::wstring& Param) {
	rdPtr->FuncNameStack->emplace_back(FuncName);
	rdPtr->FuncParamStack->emplace_back();
	UpdateParam(rdPtr, Param);
	rdPtr->FuncReturn->clear();

	(*rdPtr->RecursiveIndex)[FuncName] += 1;

	//Note: if your MMF version is under R293.9, you need to use code below instead of just a single line CallEvent(ONFUNC) to avoid crash

	//LPRH pRh = rdPtr->rHo.hoAdRunHeader;
	//expression* saveExpToken = pRh->rh4.rh4ExpToken;
	//CallEvent(ONFUNC);
	//pRh->rh4.rh4ExpToken = saveExpToken;

	//Call Func;
	CallEvent(ONFUNC);

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncParamStack->pop_back();
	rdPtr->FuncTempParamStack->erase(FuncName);

	(*rdPtr->RecursiveIndex)[FuncName] -= 1;

	if ((*rdPtr->RecursiveIndex)[FuncName] == 0) {
		rdPtr->RecursiveIndex->erase(FuncName);
	}
}

#define CallFuncCore(FuncName, Param) CallFuncCore(rdPtr, FuncName, Param)

#endif // !_FUNC_

