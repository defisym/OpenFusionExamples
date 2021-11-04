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

//convert double to string, 2X faster than to_string
inline std::wstring _dtos(double Val) {
	//auto size = swprintf(nullptr, 0, L"%f", Val);
	auto size = 50;
	std::wstring ret(size + 1, '\0');

	swprintf(&ret[0], size + 1, L"%f", Val);

	return ret;
}

inline std::wstring _ftos(float Val) {
	return _dtos(Val);
}

//convert string to double, 5X faster than std::stod
inline double _stod(const wchar_t* p) {
	double r = 0.0;
	bool neg = false;

	if (*p == L'-') {
		neg = true;
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		r = (r * 10.0) + (*p - L'0');
		++p;
	}

	if (*p == L'.') {
		double f = 0.0;
		int n = 0;
		++p;

		while (*p >= L'0' && *p <= L'9') {
			f = (f * 10.0) + (*p - L'0');
			++p;
			++n;
		}

		r += f / std::pow(10.0, n);
	}

	if (*p != L'\0') {
		return 0.0;
	}

	if (neg) {
		r = -r;
	}

	return r;
}

inline float _stof(const std::wstring& p) {
	return (float)_stod(p.c_str());
}

//convert data
inline void Data_StoV(Data& Data) {
	if (Data.Type == DataType::STRING && !Data.Converted) {
		Data.Val = _stof(Data.Str);
		Data.Converted = true;
	}
}

inline void Data_VtoS(Data& Data) {
	if (Data.Type == DataType::VALUE && !Data.Converted) {
		Data.Str = _ftos(Data.Val);
		Data.Converted = true;
	}
}

//chekc if a string is number
inline bool StrIsNum(const wchar_t* p) {
	if (*p == L'-') {
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		++p;
	}

	if (*p == L'.') {
		++p;

		while (*p >= L'0' && *p <= L'9') {
			++p;
		}

	}

	if (*p != L'\0') {
		return false;
	}

	return true;
}

inline bool StrIsNum(const std::wstring& p) {
	return StrIsNum(p.c_str());
}

//chekc if data can be convert to number
inline bool DataIsNum(Data& Data) {
	if (Data.Type == DataType::VALUE) {
		return true;
	}
	else {
		return StrIsNum(Data.Str);
	}	
}

inline void UpdateCore(LPRDATA rdPtr, std::wstring& Param, LPPARAMVEC Tar) {
	if (Param == Empty_Str) {
		return;
	}

	//Ref
	//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c

	size_t start = Param.find_first_not_of(Delimiter), end = start;

	while (start != std::wstring::npos) {
		// Find next occurence of delimiter
		end = Param.find(Delimiter, start);
		// Push back the token found into vector
		Tar->emplace_back(Data_Str(Param.substr(start, end - start)));
		// Skip all occurences of the delimiter to find new start
		start = Param.find_first_not_of(Delimiter, end);
	}
	
	return;
}

inline void UpdateParam(LPRDATA rdPtr, std::wstring& Param) {
	UpdateCore(rdPtr, Param, &rdPtr->FuncParamStack->back());
}

inline void UpdateReturn(LPRDATA rdPtr, std::wstring& Param) {
	UpdateCore(rdPtr, Param, rdPtr->FuncReturn);	
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

	//Call Func;
	if (rdPtr->CompatibleMode) {
		//Note: if your MMF version is below R293.9, you need to enable compatible mode to avoid crash
		LPRH pRh = rdPtr->rHo.hoAdRunHeader;
		expression* saveExpToken = pRh->rh4.rh4ExpToken;
		CallEvent(ONFUNC);
		pRh->rh4.rh4ExpToken = saveExpToken;
	}
	else {
		CallEvent(ONFUNC);
	}	

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncParamStack->pop_back();	

	(*rdPtr->RecursiveIndex)[FuncName] -= 1;

	if ((*rdPtr->RecursiveIndex)[FuncName] == 0) {
		rdPtr->RecursiveIndex->erase(FuncName);
		rdPtr->FuncTempParamStack->erase(FuncName);
	}
}

#define CallFuncCore(FuncName, Param) CallFuncCore(rdPtr, FuncName, Param)

#endif // !_FUNC_

