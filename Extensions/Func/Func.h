#pragma once

inline bool CheckCompatibility(LPRDATA rdPtr) {
	const auto ver = rdPtr->rHo.hoAdRunHeader->rhApp->m_miniHdr.gaPrdBuild;

	return ver < 295;
}

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

	if constexpr (std::is_same_v<Type, std::wstring()>) {
		return reinterpret_cast<long>(Default_Str);
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

	SplitStringCore<wchar_t>(Param, Delimiter, [&] (const std::wstring_view& item) {
		Tar->emplace_back(item);
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

inline std::wstring GetRecursiveSuffix(size_t ID) {
	switch (ID) {
	case 0:
		return L"_0";
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

inline const std::wstring& GetFuncNameWithRecursiveID(LPRDATA rdPtr, const std::wstring& funcName) {
	const auto it = rdPtr->FuncManglingName->find(funcName);

	if (it == rdPtr->FuncManglingName->end()) {
		(*rdPtr->FuncManglingName)[funcName] = funcName + GetRecursiveSuffix(0);
	}

	return (*rdPtr->FuncManglingName)[funcName];
}

class FuncInfoObject {
	LPRDATA rdPtr = nullptr;
	std::wstring funcName;
	std::wstring manglingName;
	std::wstring oldManglingName;

	inline std::wstring GetFuncNameWithRecursiveID() const {
		const auto it = rdPtr->RecursiveIndex->find(funcName);
		const std::wstring suffix = GetRecursiveSuffix(it != rdPtr->RecursiveIndex->end()
			? it->second
			: 0);

		return funcName + suffix;
	}

public:
	FuncInfoObject(LPRDATA rdPtr, std::wstring& funcName, std::wstring& param) {
		this->rdPtr = rdPtr;
		this->funcName = funcName;

		rdPtr->FuncNameStack->emplace_back(funcName);
		rdPtr->FuncRawParamStack->emplace_back(param);

		rdPtr->FuncParamStack->emplace_back();
		UpdateParam(rdPtr, param);

		rdPtr->FuncReturn->clear();

		(*rdPtr->RecursiveIndex)[funcName] += 1;

		this->manglingName = GetFuncNameWithRecursiveID();
		(*rdPtr->FuncManglingName)[funcName] = this->manglingName;

		*rdPtr->pPreviousFuncName = rdPtr->FuncNameStack->back();
	}
	~FuncInfoObject() {
		rdPtr->FuncNameStack->pop_back();
		rdPtr->FuncRawParamStack->pop_back();

		rdPtr->FuncParamStack->pop_back();

		rdPtr->FuncTempParam->erase(manglingName);

		(*rdPtr->RecursiveIndex)[funcName] -= 1;

		if ((*rdPtr->RecursiveIndex)[funcName] == 0) {
			rdPtr->RecursiveIndex->erase(funcName);
		}

		(*rdPtr->FuncManglingName)[funcName] = GetFuncNameWithRecursiveID();
	}

	inline const auto& GetManglingName() { return manglingName; }

	// handle loop
	inline void CallFunc(const std::function<void()>& callback,
		size_t loopIndex = 1) const {
		if (loopIndex == 1) {
			callback();

			return;
		}

		(*rdPtr->FuncLoopIndex)[manglingName] = loopIndex;

		for ((*rdPtr->FuncCurLoopIndex)[manglingName] = 0;
			(*rdPtr->FuncCurLoopIndex)[manglingName] < loopIndex;
			(*rdPtr->FuncCurLoopIndex)[manglingName]++) {
			callback();
		}

		rdPtr->FuncLoopIndex->erase(manglingName);
		rdPtr->FuncCurLoopIndex->erase(manglingName);
	}
};

inline void CallFuncCore(LPRDATA rdPtr, std::wstring& funcName, std::wstring& param, size_t loopIndex = 1) {
	// RAII
	const FuncInfoObject funcObj(rdPtr, funcName, param);

	rdPtr->pSelect->KeepScopeCall(rdPtr->bKeepScope, [&] () {
		funcObj.CallFunc([&] () {
			if (rdPtr->CompatibleMode) {
				//Note: if your MMF version is below R293.9, you need to enable compatible mode to avoid crash
				const LPRH pRh = rdPtr->rHo.hoAdRunHeader;
				expression* saveExpToken = pRh->rh4.rh4ExpToken;

				CallEvent(ONFUNC);

				pRh->rh4.rh4ExpToken = saveExpToken;
			}
			else {
				CallEvent(ONFUNC);
			}
		}, loopIndex);

	});
}

inline void IterateObjectCore(LPRDATA rdPtr, short oil,
	const std::function<void(const ObjectSelection::SelObj&)>& callback) {
	ObjectSelection::SelObj toIterate;
	const auto flag = rdPtr->pSelect->ObjectIsSelected(oil)
		? ForEachFlag_SelectedOnly
		: ForEachFlag_Default;

	rdPtr->pSelect->ForEach(rdPtr, oil, [&] (LPRO object) {
		toIterate.emplace_back(object);
		}, flag);

	if (toIterate.empty()) {
		return;
	}

	callback(toIterate);
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
	const auto& name = GetFuncNameWithRecursiveID(rdPtr, FuncName);

	return  rdPtr->FuncTempParam->contains(name) && (*rdPtr->FuncTempParam)[name].contains(ParamName);
}
