#pragma once

inline void Assert(long value, const std::wstring& msg) {
#ifndef RUN_ONLY
	if (!value) {
		const auto ret = MessageBox(nullptr, StrEqu(msg.c_str(), Empty_Str)
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

template<typename Type>
inline long NotInFuncError() {
#ifndef RUN_ONLY
	Assert(false, L"Not In Func");
#endif

	if (std::is_same_v<Type, std::wstring()>) {
		return (long)Default_Str;
	}
	else {
		return -1;
	}
}

//convert data
inline void Data_StoV(Data& Data) {
	if (!Data.Converted && Data.Type == DataType::STRING) {
		Data.Val = _stof(Data.Str);
		Data.Converted = true;
	}
}

inline void Data_VtoS(Data& Data) {
	if (!Data.Converted && Data.Type == DataType::VALUE) {
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

inline void UpdateCore(LPRDATA rdPtr, const std::wstring& Param, LPPARAMVEC Tar) {
	if (Param.empty()) {
		return;
	}

	SplitStringCore(Param, Delimiter, [&] (const std::wstring& item) {
		Tar->emplace_back(Data(item));
	});
}

inline void UpdateParam(LPRDATA rdPtr, const std::wstring& Param) {
	rdPtr->FuncParamStack->back().reserve(DefaultVecSize);
	UpdateCore(rdPtr, Param, &rdPtr->FuncParamStack->back());
}

inline void UpdateReturn(LPRDATA rdPtr, const std::wstring& Param) {
	rdPtr->FuncReturn->reserve(DefaultVecSize);
	UpdateCore(rdPtr, Param, rdPtr->FuncReturn);
}

inline const std::wstring GetRecursiveSuffix(size_t ID) {
	switch (ID) {
	case 1:
		return L"_1";
	case 2:
		return L"_2";
	case 3:
		return L"_3";
	case 4:
		return L"_4";
	case 5:
		return L"_5";
	case 6:
		return L"_6";
	case 7:
		return L"_7";
	case 8:
		return L"_8";
	case 9:
		return L"_9";
	default:
		return std::wstring(L"_") + _itos(ID);
	}
}

inline std::wstring GetFuncNameWithRecursiveID(LPRDATA rdPtr, const std::wstring& funcName) {
	const auto it = rdPtr->RecursiveIndex->find(funcName);
	const std::wstring suffix = it == rdPtr->RecursiveIndex->end() ? L"_0" : GetRecursiveSuffix(it->second);

	return funcName + suffix;
}

inline void CallFuncCore(LPRDATA rdPtr, std::wstring& FuncName, std::wstring& Param) {
	// ------------
	// Prepare
	// ------------

	rdPtr->FuncNameStack->emplace_back(FuncName);
	rdPtr->FuncRawParamStack->emplace_back(Param);

	rdPtr->FuncParamStack->emplace_back();
	UpdateParam(rdPtr, Param);

	rdPtr->FuncReturn->clear();

	(*rdPtr->RecursiveIndex)[FuncName] += 1;

	// ------------
	// Call Func;
	// ------------

	*rdPtr->pPreviousFuncName = rdPtr->FuncNameStack->back();

	if (rdPtr->CompatibleMode) {
		//Note: if your MMF version is below R293.9, you need to enable compatible mode to avoid crash
		const LPRH pRh = rdPtr->rHo.hoAdRunHeader;
		expression* saveExpToken = pRh->rh4.rh4ExpToken;

		CallEventFunc(rdPtr, ONFUNC, rdPtr->bKeepScope, rdPtr->pSelect);

		pRh->rh4.rh4ExpToken = saveExpToken;
	}
	else {
		CallEventFunc(rdPtr, ONFUNC, rdPtr->bKeepScope, rdPtr->pSelect);
	}

	// ------------
	// Clean up
	// ------------

	rdPtr->FuncNameStack->pop_back();
	rdPtr->FuncRawParamStack->pop_back();
	
	rdPtr->FuncParamStack->pop_back();	

	rdPtr->FuncTempParam->erase(GetFuncNameWithRecursiveID(rdPtr, FuncName));

	(*rdPtr->RecursiveIndex)[FuncName] -= 1;

	if ((*rdPtr->RecursiveIndex)[FuncName] == 0) {
		rdPtr->RecursiveIndex->erase(FuncName);
	}
}

inline bool HasParam(LPRDATA rdPtr, size_t Pos) {
	return !rdPtr->FuncParamStack->empty()
		&& !rdPtr->FuncParamStack->back().empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncParamStack->back().size() - 1)));
}

inline Data& GetParam(LPRDATA rdPtr, size_t Pos) {
	return rdPtr->FuncParamStack->back().at(Pos);
}

inline bool HasReturn(LPRDATA rdPtr, size_t Pos) {
	return !rdPtr->FuncReturn->empty()
		&& (Pos == max(Pos, min(Pos, rdPtr->FuncReturn->size() - 1)));
}

inline Data& GetReturn(LPRDATA rdPtr, size_t Pos) {
#ifndef RUN_ONLY
	try {
		return rdPtr->FuncReturn->at(Pos);
	} catch ([[maybe_unused]] std::out_of_range& e) {
		std::wstring msg = std::format(L"{}", *rdPtr->pPreviousFuncName);

		if (rdPtr->FuncReturn->empty()) {
			msg += L" Func Has No Return";
		}
		else {
			msg += L" Func Has No Return At " + _itos(Pos);
		}

		Assert(false, msg);

		return rdPtr->defaultData;
	}
#else
	if (Pos >= rdPtr->FuncReturn->size()) {
		return rdPtr->defaultData;
	}
	else {
		return rdPtr->FuncReturn->at(Pos);
	}
#endif // !RUN_ONLY
}

inline Data& TempParam(LPRDATA rdPtr, const std::wstring& FuncName, const std::wstring& ParamName) {
	return (*rdPtr->FuncTempParam)[GetFuncNameWithRecursiveID(rdPtr, FuncName)][ParamName];
}

inline bool HasTempParam(LPRDATA rdPtr, const std::wstring& FuncName, const std::wstring& ParamName) {
	const auto name = GetFuncNameWithRecursiveID(rdPtr, FuncName);

	return  (*rdPtr->FuncTempParam).contains(name) && (*rdPtr->FuncTempParam)[name].contains(ParamName);
}