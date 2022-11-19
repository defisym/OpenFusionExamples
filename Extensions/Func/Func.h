#ifndef _FUNC_
#define _FUNC_

inline void Assert(long value, const std::wstring& msg) {
#ifndef RUN_ONLY
	if (!value) {
		auto ret = MessageBox(NULL, StrEqu(msg.c_str(), Empty_Str)
			? L"Assert Failed"
			: msg.c_str()
			, L"Assert Failed"
			, MB_ABORTRETRYIGNORE);

		if (ret == IDABORT) {
			exit(0);
		}
	}
#endif // !RUN_ONLY
}

inline Data& GetReturn(LPRDATA rdPtr, size_t Pos) {
#ifndef RUN_ONLY
	try {
#endif // !RUN_ONLY
		return rdPtr->FuncReturn->at(Pos);
#ifndef RUN_ONLY
	}
	catch (std::out_of_range) {
		std::wstring msg = *rdPtr->pPreviousFuncName;

		if (rdPtr->FuncReturn->empty()) {
			msg += L" Func Has No Return";
		}
		else {
			msg += L" Func Has No Return At " + _itos(Pos);
		}

		Assert(false, msg);

		return Data{};
	}
#endif // !RUN_ONLY
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

//check if data can be convert to number
inline bool DataIsNum(Data& Data) {
	if (!Data.IsNumberChecked) {
		Data.IsNumberChecked = true;
		Data.IsNumber = StrIsNum(Data.Str);
	}

	return Data.IsNumber;
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
	rdPtr->FuncParamStack->back().reserve(DefaultVecSize);
	UpdateCore(rdPtr, Param, &rdPtr->FuncParamStack->back());
}

inline void UpdateReturn(LPRDATA rdPtr, std::wstring& Param) {
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	UpdateCore(rdPtr, Param, rdPtr->FuncReturn);
}

inline std::wstring GetFuncNameWithRecursiveID(LPRDATA rdPtr, std::wstring& funcName) {
	auto& it = rdPtr->RecursiveIndex->find(funcName);

	std::wstring suffix = it == rdPtr->RecursiveIndex->end() ? _itos(0) : _itos(it->second);

	return std::wstring(L"_") + suffix;
}

#define GetFuncNameWithRecursiveID(funcName) GetFuncNameWithRecursiveID(rdPtr, funcName)

inline void CallFuncCore(LPRDATA rdPtr, std::wstring& FuncName, std::wstring& Param) {
	rdPtr->FuncNameStack->emplace_back(FuncName);

	rdPtr->FuncParamStack->emplace_back();
	UpdateParam(rdPtr, Param);

	rdPtr->FuncReturn->clear();

	(*rdPtr->RecursiveIndex)[FuncName] += 1;

	//Call Func;
	*rdPtr->pPreviousFuncName = rdPtr->FuncNameStack->back();

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

	rdPtr->FuncTempParam->erase(GetFuncNameWithRecursiveID(FuncName));

	(*rdPtr->RecursiveIndex)[FuncName] -= 1;

	if ((*rdPtr->RecursiveIndex)[FuncName] == 0) {
		rdPtr->RecursiveIndex->erase(FuncName);
	}
}

#define CallFuncCore(FuncName, Param) CallFuncCore(rdPtr, FuncName, Param)

#endif // !_FUNC_

