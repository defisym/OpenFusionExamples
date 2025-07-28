// ============================================================================
//
// This file are where the Conditions/Actions/Expressions are defined.
// You can manually enter these, or use CICK (recommended)
// See the Extension FAQ in this SDK for more info and where to download it
//
// ============================================================================

// Common Include
#include	"common.h"

// Quick memo: content of the eventInformations arrays
// ---------------------------------------------------
// Menu ID
// String ID
// Code
// Flags
// Number_of_parameters
// Parameter_type [Number_of_parameters]
// Parameter_TitleString [Number_of_parameters]

// Definitions of parameters for each condition
short conditionsInfos[]=
		{
		IDMN_CONDITION_ITS, M_CONDITION_ITS, CND_CONDITION_ITS, 0, 1, PARAM_EXPSTRING,M_CND_ITN,
		IDMN_CONDITION_ITI, M_CONDITION_ITI, CND_CONDITION_ITI, 0, 1, PARAM_EXPSTRING,M_CND_ITN,

		IDMN_CONDITION_SIHV, M_CONDITION_SIHV, CND_CONDITION_SIHV, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_NF, M_ACTION_NF,	ACT_ACTION_NF,	0, 0,
		IDMN_ACTION_DF, M_ACTION_DF,	ACT_ACTION_DF,	0, 0,
		IDMN_ACTION_LF, M_ACTION_LF,	ACT_ACTION_LF,	0, 2,PARAM_FILENAME2,PARAM_EXPSTRING,ACT_ACTION_F,ACT_ACTION_K,
		IDMN_ACTION_SF, M_ACTION_SF,	ACT_ACTION_SF,	0, 2,PARAM_FILENAME2,PARAM_EXPSTRING,ACT_ACTION_F,ACT_ACTION_K,
		IDMN_ACTION_SSIV, M_ACTION_SSIV,	ACT_ACTION_SSIV,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPRESSION,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,ACT_ACTION_SSI_VAL,
		IDMN_ACTION_SSIS, M_ACTION_SSIS,	ACT_ACTION_SSIS,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,ACT_ACTION_SSI_STR,
		IDMN_ACTION_DSI, M_ACTION_DSI,	ACT_ACTION_DSI,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_SSI_S,ACT_ACTION_SSI_I,
		IDMN_ACTION_ITS, M_ACTION_ITS,	ACT_ACTION_ITS,	0, 1,PARAM_EXPSTRING,ACT_ACTION_IT,
		IDMN_ACTION_ITI, M_ACTION_ITI,	ACT_ACTION_ITI,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_ITSN,ACT_ACTION_IT,
		IDMN_ACTION_LS, M_ACTION_LS,	ACT_ACTION_LS,	0, 1,PARAM_EXPSTRING,ACT_ACTION_S,

		IDMN_ACTION_SAON, M_ACTION_SAON, ACT_ACTION_SAON,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_F,ACT_ACTION_K,
		IDMN_ACTION_SAOFF, M_ACTION_SAOFF, ACT_ACTION_SAOFF,	0, 0,

		IDMN_ACTION_CS, M_ACTION_CS, ACT_ACTION_CS,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPRESSION,ACT_ACTION_CS_SRC,ACT_ACTION_CS_DES,ACT_ACTION_CS_DEL,

		IDMN_ACTION_LAV, M_ACTION_LAV,ACT_ACTION_LAV, 0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_FIXED, M_BASE64,
		IDMN_ACTION_LP, M_ACTION_LP,ACT_ACTION_LP, 0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_FIXED, M_BASE64,

		IDMN_ACTION_LCB64, M_ACTION_LCB64, ACT_ACTION_LCB64, 0, 1, PARAM_EXPSTRING, M_BASE64,

		IDMN_ACTION_LL, M_ACTION_LL, ACT_ACTION_LL,	0, 2, PARAM_FILENAME2, PARAM_EXPSTRING, ACT_ACTION_F,ACT_ACTION_K,
		IDMN_ACTION_SLC, M_ACTION_SLC, ACT_ACTION_SLC,	0, 1, PARAM_EXPSTRING, ACT_ACTION_LC,
		IDMN_ACTION_LLO, M_ACTION_LLO, ACT_ACTION_LLO,	0, 1, PARAM_EXPRESSION, ACT_ACTION_OBJ,

		IDMN_ACTION_LB64, M_ACTION_LB64, ACT_ACTION_LB64, 0, 1, PARAM_EXPSTRING, M_BASE64,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GSIV, M_EXPRESSION_GSIV, EXP_EXPRESSION_GSIV, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
		IDMN_EXPRESSION_GSIS, M_EXPRESSION_GSIS, EXP_EXPRESSION_GSIS, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
		IDMN_EXPRESSION_GCS, M_EXPRESSION_GCS, EXP_EXPRESSION_GCS, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GCI, M_EXPRESSION_GCI, EXP_EXPRESSION_GCI, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_SS, M_EXPRESSION_SS, EXP_EXPRESSION_SS, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_SAV, M_EXPRESSION_SAV, EXP_EXPRESSION_SAV, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_FIXED,
		IDMN_EXPRESSION_SP, M_EXPRESSION_SP, EXP_EXPRESSION_SP, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_FIXED,
		IDMN_EXPRESSION_SCB64, M_EXPRESSION_SCB64, EXP_EXPRESSION_SCB64, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_SB64, M_EXPRESSION_SB64, EXP_EXPRESSION_SB64, EXPFLAG_STRING, 0,
		
		IDMN_EXPRESSION_SSB64, M_EXPRESSION_SSB64, EXP_EXPRESSION_SSB64, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_LONG, ACT_ACTION_SSI_STR, M_COMPRESSED,
		IDMN_EXPRESSION_LSB64, M_EXPRESSION_LSB64, EXP_EXPRESSION_LSB64, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_LONG, M_BASE64, M_COMPRESSED,
		};

// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnIterate_Section(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->SecLoopName->c_str());
}

long WINAPI DLLExport Condition_OnIterate_Item(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->ItemLoopName->c_str());
}

long WINAPI DLLExport Condition_SecItemHasValue(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidSecItem(FALSE);

	return rdPtr->ini->SectionExists(Section) && rdPtr->ini->KeyExists(Section, Item);
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_SetAutoSaveOn(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	rdPtr->AutoSave = true;

	*rdPtr->AutoSaveFilePath = FilePath;
	*rdPtr->AutoSaveKey = Key;

	return 0;
}
short WINAPI DLLExport Action_SetAutoSaveOff(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->AutoSave = false;

	return 0;
}

short WINAPI DLLExport Action_New(LPRDATA rdPtr, long param1, long param2) {
	InitIni(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_Release(LPRDATA rdPtr, long param1, long param2) {
	ReleaseIni(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_LoadFromString(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR String = (LPCTSTR)param1;

	AutoSave(rdPtr);
	InitIni(rdPtr);
	
	rdPtr->ini->LoadData(to_byte_string(String));

	return 0;
}

short WINAPI DLLExport Action_LoadFromBase64(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR base64 = (LPCTSTR)param1;

	AutoSave(rdPtr);
	InitIni(rdPtr);

	rdPtr->pB64->base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		rdPtr->ini->LoadData((const char*)buf, sz);
		});

	return 0;
}

short WINAPI DLLExport Action_LoadFromCompressedBase64(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR base64 = (LPCTSTR)param1;

	AutoSave(rdPtr);
	InitIni(rdPtr);

	rdPtr->pB64->base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		rdPtr->ini->LoadData(ZLIBI_UnCompress((const char*)buf, sz));
		});

	return 0;
}

short WINAPI DLLExport Action_LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	AutoSave(rdPtr);
	InitIni(rdPtr);

	if (rdPtr->AutoSave) {
		*rdPtr->AutoSaveFilePath = FilePath;
		*rdPtr->AutoSaveKey = Key;
	}	

	LoadFile(rdPtr->ini, FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_LinkLocalization(LPRDATA rdPtr, long param1, long param2) {
	const auto pObject = (LPRDATA)LproFromFixed(rdPtr, param1);

	rdPtr->pLocalization->LinkObject(pObject);

	return 0;
}

short WINAPI DLLExport Action_LoadLocalization(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	rdPtr->pLocalization->LoadFile(FilePath, Key);	

	return 0;
}

short WINAPI DLLExport Action_SetLanguageCode(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR pLanguageCode = (LPCTSTR)param1;

	rdPtr->pLocalization->SetLanguageCode(pLanguageCode);

	return 0;
}

short WINAPI DLLExport Action_SaveToFile(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	InvalidIni(0);
	
	SaveFile(rdPtr->ini, FilePath, Key);

	return 0;
}

short WINAPI DLLExport Action_SetSecItem_Value(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->ini == nullptr) {
		InitIni(rdPtr);
	}

	InvalidSecItem(0);

	long p = CNC_GetFloatParameter(rdPtr);
	float Value = *(float*)&p;

	rdPtr->Modified = Modified(rdPtr->ini->SetValue(Section, Item,
		std::format(L"{}", Value).c_str()))
		|| rdPtr->Modified;

	return 0;
}

short WINAPI DLLExport Action_SetSecItem_String(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	if (rdPtr->ini == nullptr) {
		InitIni(rdPtr);
	}

	InvalidSecItem(0);

	LPCTSTR String = (LPCTSTR)CNC_GetStringParameter(rdPtr);	

	rdPtr->Modified = Modified(rdPtr->ini->SetValue(Section, Item, String))
		|| rdPtr->Modified;

	return 0;
}

short WINAPI DLLExport Action_CopySection(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Des = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	bool DeleteSrc = (bool)CNC_GetIntParameter(rdPtr);

	InvalidIni(0);

	if (StrEmpty(Src) || StrEmpty(Des)) {
		return 0;
	}

	INILIST items;
	rdPtr->ini->GetAllKeys(Src, items);

	rdPtr->Modified = !items.empty() || rdPtr->Modified;

	for (const auto & it: items) {
		//need not to check if the value is string or number to set default value, cause src is valid
		rdPtr->ini->SetValue(Des, it.pItem, rdPtr->ini->GetValue(Src, it.pItem));
	}

	if (DeleteSrc) {
		rdPtr->ini->Delete(Src, nullptr, true);
	}

	return 0;
}

short WINAPI DLLExport Action_DeleteSecItem(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidIni(0);
	InvalidSec(0);

	if (StrEqu(Item, Empty_Str)) {
		Item = nullptr;
	}

	rdPtr->Modified = rdPtr->ini->Delete(Section, Item, true)
		|| rdPtr->Modified;

	return 0;
}

short WINAPI DLLExport Action_Iterate_Section(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidIni(0);

	*rdPtr->SecLoopName = LoopName;

	INILIST sections;
	rdPtr->ini->GetAllSections(sections);

	for (const auto& it : sections) {
		*rdPtr->CurrentSec = it.pItem;
		CallEvent(ONIT_SEC);
	}

	return 0;
}

short WINAPI DLLExport Action_Iterate_Item(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR SectionName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidIni(0);
	
	*rdPtr->ItemLoopName = LoopName;

	if (!StrEqu(SectionName, Empty_Str)) {
		*rdPtr->CurrentSec = SectionName;
	}

	INILIST items;
	rdPtr->ini->GetAllKeys(rdPtr->CurrentSec->c_str(), items);

	for (const auto& it: items) {
		*rdPtr->CurrentItem = it.pItem;
		CallEvent(ONIT_ITEM);
	}

	return 0;
}

short WINAPI DLLExport Action_LoadAlterValue(LPRDATA rdPtr, long param1, long param2) {	
	size_t fixed = (size_t)CNC_GetParameter(rdPtr);
	LPCTSTR base64 = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LPRO object = LproFromFixed(rdPtr, fixed);

	size_t offset = GetRVOffset(rdPtr, object);

	if (offset == static_cast<size_t>(-1)) {
		return 0;
	}

	rdPtr->pB64->base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		auto buffer = buf;

		if (rdPtr->cf25p) {
			rVal* pRV = (rVal*)(((BYTE*)object) + offset);

			pRV->rvValueFlags = ((long*)buffer)[0];
			buffer += sizeof(long);

			for (int i = 0; i < pRV->rvNumberOfValues; i++) {
				pRV->rvpValues[i] = ((CValue*)buffer)[0];
				buffer += sizeof(CValue);
			}

			for (int i = 0; i < pRV->rvNumberOfStrings; i++) {
				if (((wchar_t*)buffer)[0] == L'\0') {
					pRV->rvpStrings[i] = nullptr;

					buffer += sizeof(wchar_t);
				}
				else {
					size_t strlen = (wcslen((wchar_t*)buffer) + 1);

					pRV->rvpStrings[i] = (wchar_t*)mvCalloc(rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv, sizeof(wchar_t) * strlen);
					wcscpy_s(pRV->rvpStrings[i], strlen, (wchar_t*)buffer);

					buffer += sizeof(wchar_t) * strlen;
				}
			}
		}
		else {
			auto pRV = (tagRV20U*)(((BYTE*)object) + offset);

			pRV->rvValueFlags = ((long*)buffer)[0];
			buffer += sizeof(long);

			for (int i = 0; i < VALUES_NUMBEROF_ALTERABLE; i++) {
				pRV->rvpValues[i] = ((CValue*)buffer)[0];
				buffer += sizeof(CValue);
			}

			for (int i = 0; i < STRINGS_NUMBEROF_ALTERABLE; i++) {
				if (((wchar_t*)buffer)[0] == L'\0') {
					pRV->rvStrings[i] = nullptr;

					buffer += sizeof(wchar_t);
				}
				else {
					size_t strlen = (wcslen((wchar_t*)buffer) + 1);

					pRV->rvStrings[i] = (wchar_t*)mvCalloc(rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv, sizeof(wchar_t) * strlen);
					wcscpy_s(pRV->rvStrings[i], strlen, (wchar_t*)buffer);

					buffer += sizeof(wchar_t) * strlen;
				}
			}

		}
	});	

	return 0;
}

short WINAPI DLLExport Action_LoadPosition(LPRDATA rdPtr, long param1, long param2) {
	size_t fixed = (size_t)CNC_GetParameter(rdPtr);
	LPCTSTR base64 = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LPRO object = LproFromFixed(rdPtr, fixed);

	if (!LPROValid(object)) {
		return 0;
	}

	rdPtr->pB64->base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		const auto buffer = reinterpret_cast<const int*>(buf);

		object->roHo.hoX = buffer[0];
		object->roHo.hoY = buffer[1];
	});	

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetSecItem_Value(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	
	InvalidIni(0);
	InvalidSecItem(0);
		
	LPCTSTR String = rdPtr->ini->GetValue(Section, Item, Default_Val);

	return ReturnFloat(_stof(String));
}

long WINAPI DLLExport Expression_GetSecItem_String(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	InvalidIni((long)Default_Str);	
	InvalidSecItem((long)Default_Str);

	const auto pToFind = rdPtr->ini->GetValue(Section, Item, Default_Str);
	*rdPtr->Str =  rdPtr->pLocalization->GetLocalization(pToFind);
	
	//This returns a pointer to the string for MMF.
	return (long)rdPtr->Str->c_str();
}

long WINAPI DLLExport Expression_GetCurrentSection(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->CurrentSec->c_str();
}

long WINAPI DLLExport Expression_GetCurrentItem(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->CurrentItem->c_str();
}

long WINAPI DLLExport Expression_SaveToString(LPRDATA rdPtr, long param1) {
	std::string Output;
	rdPtr->ini->Save(Output);

	*rdPtr->Str = to_wide_string(Output);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->Str->c_str();
}

long WINAPI DLLExport Expression_SaveToBase64(LPRDATA rdPtr, long param1) {
	std::string Output;
	rdPtr->ini->Save(Output);

	*rdPtr->b64Str = rdPtr->pB64->base64_encode((BYTE*)(Output.data()), Output.length());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

long WINAPI DLLExport Expression_SaveToCompressedBase64(LPRDATA rdPtr, long param1) {	
	std::string iniOutput;
	rdPtr->ini->Save(iniOutput);

    const auto compressed = ZLIBI_Compress(iniOutput);
	*rdPtr->b64Str = rdPtr->pB64->base64_encode((const BYTE*)compressed.data(), 
        compressed.size());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

long WINAPI DLLExport Expression_SaveStringToBase64(LPRDATA rdPtr, long param1) {
	std::string str = to_byte_string((LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING));
	const auto bCompress = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	const auto pData = reinterpret_cast<BYTE*>(str.data());
	const auto dataSz = str.length();

	if(!bCompress) {
		*rdPtr->b64Str = rdPtr->pB64->base64_encode(pData, dataSz);
	}else {
        const auto compressed = ZLIBI_Compress(str);
        *rdPtr->b64Str = rdPtr->pB64->base64_encode((const BYTE*)compressed.data(),
            compressed.size());
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

long WINAPI DLLExport Expression_LoadStringFromBase64(LPRDATA rdPtr, long param1) {
	const std::wstring base64 = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	const auto bCompress = (bool)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	rdPtr->pB64->base64_decode_callback(base64, [&] (const BYTE* buf, const size_t sz) {
		if (!bCompress) {			
			*rdPtr->b64Str = to_wide_string(std::string((const char*)buf, sz));
		}
		else {           
			*rdPtr->b64Str = to_wide_string(ZLIBI_UnCompress((const char*)buf, sz));
		}
	});

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

long WINAPI DLLExport Expression_SaveAlterValue(LPRDATA rdPtr, long param1) {
	size_t fixed = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	LPRO object = LproFromFixed(rdPtr, fixed);

	size_t offset = GetRVOffset(rdPtr, object);

	if (offset == static_cast<size_t>(-1)) {
		rdPtr->rHo.hoFlags |= HOF_STRING;
		return (long)Empty_Str;
	}

	if (rdPtr->cf25p) {
		rVal* pRV = (rVal*)(((BYTE*)object) + offset);

		size_t sz = sizeof(long) + sizeof(CValue) * pRV->rvNumberOfValues;
		
		for (int i = 0; i < pRV->rvNumberOfStrings; i++) {
			auto ptr = pRV->rvpStrings[i];

			if (ptr == nullptr) {
				sz += sizeof(wchar_t);
			}
			else {
				sz += sizeof(wchar_t) * (wcslen(ptr) + 1);
			}
		}

		BYTE* buf = new BYTE[sz];
		memset(buf, 0, sz);

		BYTE* buffer = buf;

		((long*)buffer)[0] = pRV->rvValueFlags;
		buffer += sizeof(long);

		for (int i = 0; i < pRV->rvNumberOfValues; i++) {
			((CValue*)buffer)[0] = pRV->rvpValues[i];

			auto view = ((CValue*)buffer)[0];

			buffer += sizeof(CValue);
		}

		for (int i = 0; i < pRV->rvNumberOfStrings; i++) {
			auto ptr = pRV->rvpStrings[i];

			if (ptr == nullptr) {
				((wchar_t*)buffer)[0] = L'\0';

				buffer += sizeof(wchar_t);
			}
			else {
				size_t strlen = (wcslen(ptr) + 1);
				wcscpy_s((wchar_t*)buffer, strlen, ptr);

				buffer += sizeof(wchar_t) * strlen;
			}
		}

		*rdPtr->b64Str = rdPtr->pB64->base64_encode(buf, sz);

		delete[] buf;
	}
	else {
		auto pRV = (tagRV20U*)(((BYTE*)object) + offset);

		size_t sz = sizeof(long) + sizeof(CValue) * VALUES_NUMBEROF_ALTERABLE;

		for (int i = 0; i < STRINGS_NUMBEROF_ALTERABLE; i++) {
			auto ptr = pRV->rvStrings[i];

			if (ptr == nullptr) {
				sz += sizeof(wchar_t);
			}
			else {
				sz += sizeof(wchar_t) * (wcslen(ptr) + 1);
			}
		}

		BYTE* buf = new BYTE[sz];
		memset(buf, 0, sz);

		BYTE* buffer = buf;

		((long*)buffer)[0] = pRV->rvValueFlags;
		buffer += sizeof(long);

		for (int i = 0; i < VALUES_NUMBEROF_ALTERABLE; i++) {
			((CValue*)buffer)[0] = pRV->rvpValues[i];

			auto view = ((CValue*)buffer)[0];

			buffer += sizeof(CValue);
		}

		for (int i = 0; i < STRINGS_NUMBEROF_ALTERABLE; i++) {
			auto ptr = pRV->rvStrings[i];

			if (ptr == nullptr) {
				((wchar_t*)buffer)[0] = L'\0';

				buffer += sizeof(wchar_t);
			}
			else {
				size_t strlen = (wcslen(ptr) + 1);
				wcscpy_s((wchar_t*)buffer, strlen, ptr);

				buffer += sizeof(wchar_t) * strlen;
			}
		}

		*rdPtr->b64Str = rdPtr->pB64->base64_encode(buf, sz);

		delete[] buf;
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

long WINAPI DLLExport Expression_SavePosition(LPRDATA rdPtr, long param1) {
	size_t fixed = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	LPRO object = LproFromFixed(rdPtr, fixed);

	if (!LPROValid(object)) {
		rdPtr->rHo.hoFlags |= HOF_STRING;
		return (long)nullptr;
	}

	size_t sz = 2 * sizeof(int);
	BYTE* buf = new BYTE[sz];
	memset(buf, 0, sz);

	int* buffer = (int*)buf;

	buffer[0] = object->roHo.hoX;
	buffer[1] = object->roHo.hoY;

	*rdPtr->b64Str = rdPtr->pB64->base64_encode(buf, sz);

	delete[] buf;

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->b64Str->c_str();
}

// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) = 
			{ 
			Condition_OnIterate_Section,
			Condition_OnIterate_Item,

			Condition_SecItemHasValue,
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_New,
			Action_Release,
			Action_LoadFromFile,
			Action_SaveToFile,
			Action_SetSecItem_Value,
			Action_SetSecItem_String,
			Action_DeleteSecItem,
			Action_Iterate_Section,
			Action_Iterate_Item,
			Action_LoadFromString,
			Action_SetAutoSaveOn,
			Action_SetAutoSaveOff,
			Action_CopySection,
			Action_LoadAlterValue,
			Action_LoadPosition,
			Action_LoadFromCompressedBase64,
			Action_LoadLocalization,
			Action_SetLanguageCode,
			Action_LinkLocalization,
			Action_LoadFromBase64,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetSecItem_Value,
			Expression_GetSecItem_String,
			Expression_GetCurrentSection,
			Expression_GetCurrentItem,
			Expression_SaveToString,
			Expression_SaveAlterValue,
			Expression_SavePosition,
			Expression_SaveToCompressedBase64,
			Expression_SaveToBase64,

			Expression_SaveStringToBase64,
			Expression_LoadStringFromBase64,

			0
			};