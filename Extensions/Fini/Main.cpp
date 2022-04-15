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
		//IDMN_ACTION_LS, M_ACTION_LS,	ACT_ACTION_LS,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_S,ACT_ACTION_K,
		IDMN_ACTION_LS, M_ACTION_LS,	ACT_ACTION_LS,	0, 1,PARAM_EXPSTRING,ACT_ACTION_S,

		IDMN_ACTION_SAON, M_ACTION_SAON, ACT_ACTION_SAON,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,ACT_ACTION_F,ACT_ACTION_K,
		IDMN_ACTION_SAOFF, M_ACTION_SAOFF, ACT_ACTION_SAOFF,	0, 0,

		IDMN_ACTION_CS, M_ACTION_CS, ACT_ACTION_CS,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPRESSION,ACT_ACTION_CS_SRC,ACT_ACTION_CS_DES,ACT_ACTION_CS_DEL,

		IDMN_ACTION_LAV, M_ACTION_LAV,ACT_ACTION_LAV, 0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_FIXED, M_BASE64,
		IDMN_ACTION_LP, M_ACTION_LP,ACT_ACTION_LP, 0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_FIXED, M_BASE64,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GSIV, M_EXPRESSION_GSIV, EXP_EXPRESSION_GSIV, 0, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
		IDMN_EXPRESSION_GSIS, M_EXPRESSION_GSIS, EXP_EXPRESSION_GSIS, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_STRING, ACT_ACTION_SSI_S, ACT_ACTION_SSI_I,
		IDMN_EXPRESSION_GCS, M_EXPRESSION_GCS, EXP_EXPRESSION_GCS, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GCI, M_EXPRESSION_GCI, EXP_EXPRESSION_GCI, EXPFLAG_STRING, 0,
		//IDMN_EXPRESSION_SS, M_EXPRESSION_SS, EXP_EXPRESSION_SS, EXPFLAG_STRING, 1,EXPPARAM_STRING,ACT_ACTION_K,
		IDMN_EXPRESSION_SS, M_EXPRESSION_SS, EXP_EXPRESSION_SS, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_SAV, M_EXPRESSION_SAV, EXP_EXPRESSION_SAV, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_FIXED,
		IDMN_EXPRESSION_SP, M_EXPRESSION_SP, EXP_EXPRESSION_SP, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_FIXED,
		};

// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport OnIterate_Section(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->SecLoopName) ? TRUE : FALSE;
}

long WINAPI DLLExport OnIterate_Item(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->ItemLoopName) ? TRUE : FALSE;
}

long WINAPI DLLExport SecItemHasValue(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidSecItem(FALSE);

	LPCTSTR DefaultStr = _T("");

	return !StrEqu(Default_Str, Fini->GetValue(Section, Item, Default_Str));
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport SetAutoSaveOn(LPRDATA rdPtr, long param1, long param2) {	
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	rdPtr->AutoSave = true;

	NewStr(rdPtr->AutoSaveFilePath, FilePath);
	NewStr(rdPtr->AutoSaveKey, Key);

	return 0;
}
short WINAPI DLLExport SetAutoSaveOff(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->AutoSave = false;

	return 0;
}

short WINAPI DLLExport New(LPRDATA rdPtr, long param1, long param2) {
	init_ini();

	return 0;
}

short WINAPI DLLExport Release(LPRDATA rdPtr, long param1, long param2) {
	release_ini();

	return 0;
}

short WINAPI DLLExport LoadFromString(LPRDATA rdPtr, long param1, long param2) {
	//LPCTSTR String = (LPCTSTR)param1;
	//LPCTSTR Key = (LPCTSTR)param2;

	//init_ini();

	////Key has value, try to Decry
	//if (!StrEmpty(Key)) {
	//	Encryption Decrypt;
	//	Decrypt.GenerateKey(Key);

	//	Decrypt.SetEncryptStr(String, wcslen(String));
	//	//Decrypt.SetEncryptStr(to_byte_string(String));
	//	Decrypt.Decrypt();

	//	Fini->LoadData(Decrypt.GetOutputStr(), Decrypt.GetDecryptStrLength());
	//	Decrypt.ReleaseOutputStr();
	//}
	//else {		
	//	Fini->LoadData(to_byte_string(String));
	//}

	LPCTSTR String = (LPCTSTR)param1;

	AutoSave(rdPtr);

	init_ini();
	
	Fini->LoadData(to_byte_string(String));

	return 0;
}

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;

	AutoSave(rdPtr);

	init_ini();

	if (rdPtr->AutoSave) {
		NewStr(rdPtr->AutoSaveFilePath, FilePath);
		NewStr(rdPtr->AutoSaveKey, Key);
	}	

	//Key has value, try to Decry
	if (!StrEmpty(Key)) {
		Encryption Decrypt;
		Decrypt.GenerateKey(Key);

		Decrypt.OpenFile(FilePath);
		Decrypt.Decrypt();

		Fini->LoadData(Decrypt.GetOutputStr(), Decrypt.GetOutputStrLength());
		Decrypt.ReleaseOutputStr();
	}
	else {
		Fini->LoadFile(FilePath);
	}

	return 0;
}

short WINAPI DLLExport SaveToFile(LPRDATA rdPtr, long param1, long param2) {
	invalid(0);
	
	LPCTSTR FilePath = (LPCTSTR)param1;
	LPCTSTR Key = (LPCTSTR)param2;
	
	SaveFile(Fini, FilePath, Key);

	////Key has value, try to Encry
	//if (!StrEmpty(Key)) {
	//	std::string Output;
	//	Fini->Save(Output);

	//	Encryption Encrypt;
	//	Encrypt.GenerateKey(Key);
	//	
	//	Encrypt.SetEncryptStr(Output);
	//	Encrypt.Encrypt();

	//	Encrypt.SaveFile(FilePath);
	//}
	//else {
	//	Fini->SaveFile(FilePath, false);
	//}	

	return 0;
}

short WINAPI DLLExport SetSecItem_Value(LPRDATA rdPtr, long param1, long param2) {
	//invalid(0);
	if (!valid(Fini)) {
		init_ini();
	}
	
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	
	InvalidSecItem(0);

	long p = CNC_GetFloatParameter(rdPtr);
	float Value = *(float*)&p;

	LPTSTR String = new WCHAR[FLOAT_MAX];

	//Save int
	if (Value == (int)Value) {
		swprintf(String, FLOAT_MAX, _T("%d"), (int)Value);
	}
	//Save float
	else {
		swprintf(String, FLOAT_MAX, _T("%f"), Value);
	}
		
	rdPtr->Modified = Modified(Fini->SetValue(Section, Item, String)) || rdPtr->Modified;

	delete[] String;

	return 0;
}

short WINAPI DLLExport SetSecItem_String(LPRDATA rdPtr, long param1, long param2) {
	//invalid(0);
	if (!valid(Fini)) {
		init_ini();
	}
	
	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidSecItem(0);

	LPCTSTR String = (LPCTSTR)CNC_GetStringParameter(rdPtr);	

	rdPtr->Modified = Modified(Fini->SetValue(Section, Item, String)) || rdPtr->Modified;

	return 0;
}

short WINAPI DLLExport CopySection(LPRDATA rdPtr, long param1, long param2) {
	invalid(0);

	LPCTSTR Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Des = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	bool DeleteSrc = (bool)CNC_GetIntParameter(rdPtr);

	if (StrEmpty(Src) || StrEmpty(Des)) {
		return 0;
	}

	INILIST Temp;
	Fini->GetAllKeys(Src, Temp);
	
	rdPtr->Modified = (Temp.size() != 0) || rdPtr->Modified;

	INIIT it;
	for (it = Temp.begin(); it != Temp.end(); ++it) {
		//need not to check if the value is string or number to set default value, cause src is valid
		Fini->SetValue(Des, it->pItem, Fini->GetValue(Src, it->pItem));
	}

	if (DeleteSrc) {
		Fini->Delete(Src, nullptr, true);
	}

	return 0;
}

short WINAPI DLLExport DeleteSecItem(LPRDATA rdPtr, long param1, long param2) {
	invalid(0);

	LPCTSTR Section = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Item = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	InvalidSecItem(0);

	if (StrEqu(Item, Empty_Str)) {
		Item = nullptr;
	}

	rdPtr->Modified = Fini->Delete(Section, Item, true) || rdPtr->Modified;

	return 0;
}

short WINAPI DLLExport Iterate_Section(LPRDATA rdPtr, long param1, long param2) {
	invalid(0);

	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->SecLoopName, LoopName);

	INILIST Temp;
	Fini->GetAllSections(Temp);

	INIIT it;
	for (it = Temp.begin(); it != Temp.end(); ++it) {
		NewStr(rdPtr->CurrentSec, it->pItem);
		CallEvent(ONIT_SEC);
	}

	return 0;
}

short WINAPI DLLExport Iterate_Item(LPRDATA rdPtr, long param1, long param2) {
	invalid(0);

	LPCTSTR SectionName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);	
	
	NewStr(rdPtr->ItemLoopName, LoopName);	

	if (!StrEqu(SectionName, Empty_Str)) {
		if (!valid(rdPtr->CurrentSec) || !StrEqu(SectionName, rdPtr->CurrentSec)) {
			NewStr(rdPtr->CurrentSec, SectionName);
		}
	}

	INILIST Temp;
	Fini->GetAllKeys(rdPtr->CurrentSec, Temp);

	INIIT it;
	for (it = Temp.begin(); it != Temp.end(); ++it) {
		NewStr(rdPtr->CurrentItem, it->pItem);
		CallEvent(ONIT_ITEM);
	}

	return 0;
}

short WINAPI DLLExport LoadAlterValue(LPRDATA rdPtr, long param1, long param2) {	
	size_t fixed = (size_t)CNC_GetParameter(rdPtr);
	LPCTSTR base64 = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LPRO object = LproFromFixed(rdPtr, fixed);

	size_t offset = GetRVOffset(rdPtr, object);

	if (offset == -1) {
		return 0;
	}

	try {
		rdPtr->pB64->base64_decode(base64);
	}
	catch (decltype(BASE64_DECODEERROR)) {
		return 0;
	}

	auto sz = rdPtr->pB64->base64_decode_size();

	BYTE* buf = new BYTE [sz];
	memset(buf, 0, sz);

	rdPtr->pB64->base64_decode_to_pointer(buf, sz);

	BYTE* buffer = buf;

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

		pRV->rvValueFlags= ((long*)buffer)[0];
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

	delete[] buf;

	return 0;
}

short WINAPI DLLExport LoadPosition(LPRDATA rdPtr, long param1, long param2) {
	size_t fixed = (size_t)CNC_GetParameter(rdPtr);
	LPCTSTR base64 = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	LPRO object = LproFromFixed(rdPtr, fixed);

	if (!LPROValid(object)) {
		return 0;
	}

	try {
		rdPtr->pB64->base64_decode(base64);
	}
	catch (decltype(BASE64_DECODEERROR)) {
		return 0;
	}

	auto sz = rdPtr->pB64->base64_decode_size();

	BYTE* buf = new BYTE[sz];
	memset(buf, 0, sz);

	rdPtr->pB64->base64_decode_to_pointer(buf, sz);

	int* buffer = (int*)buf;

	object->roHo.hoX = buffer[0];
	object->roHo.hoY = buffer[1];

	delete[] buf;

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport GetSecItem_Value(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	
	invalid(0);
	InvalidSecItem(0);
		
	LPCTSTR String = Fini->GetValue(Section, Item, Default_Val);

	return ReturnFloat(_stof(String));
}

long WINAPI DLLExport GetSecItem_String(LPRDATA rdPtr, long param1) {
	LPCTSTR Section = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Item = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	invalid((long)Default_Str);	
	InvalidSecItem((long)Default_Str);

	NewStr(OStr, Fini->GetValue(Section, Item, Default_Str));

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	
	//This returns a pointer to the string for MMF.
	return (long)OStr;
}

long WINAPI DLLExport GetCurrentSection(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return valid(rdPtr->CurrentSec) ? (long)rdPtr->CurrentSec : (long)Empty_Str;
}

long WINAPI DLLExport GetCurrentItem(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return valid(rdPtr->CurrentItem) ? (long)rdPtr->CurrentItem : (long)Empty_Str;
}

long WINAPI DLLExport SaveToString(LPRDATA rdPtr, long param1) {
	//LPCTSTR Key = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	//std::string Output;
	//
	////Key has value, try to Encry
	//if (!StrEmpty(Key)) {		
	//	std::string Temp;
	//	Fini->Save(Temp);

	//	Encryption Encrypt;
	//	Encrypt.GenerateKey(Key);

	//	Encrypt.SetEncryptStr(Temp);
	//	Encrypt.Encrypt();
	//			
	//	Output = Encrypt.GetOutputStr();

	//	wchar_t* Result = new wchar_t[(int)ceil(Output.length() / 2)];
	//	memcpy(Result, Output.c_str(), Output.length());

	//	NewStr(OStr, Result);
	//	
	//	delete[] Result;
	//}
	//else {
	//	Fini->Save(Output);
	//	NewStr(OStr, to_wide_string(Output).c_str());
	//}

	std::string Output;
	Fini->Save(Output);
	
	NewStr(OStr, to_wide_string(Output).c_str());

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)OStr;
}

long WINAPI DLLExport SaveAlterValue(LPRDATA rdPtr, long param1) {
	size_t fixed = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	LPRO object = LproFromFixed(rdPtr, fixed);

	size_t offset = GetRVOffset(rdPtr, object);

	if (offset == -1) {
		rdPtr->rHo.hoFlags |= HOF_STRING;
		return (long)nullptr;
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

long WINAPI DLLExport SavePosition(LPRDATA rdPtr, long param1) {
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
			OnIterate_Section,
			OnIterate_Item,

			SecItemHasValue,
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			New,
			Release,
			LoadFromFile,
			SaveToFile,
			SetSecItem_Value,
			SetSecItem_String,
			DeleteSecItem,
			Iterate_Section,
			Iterate_Item,
			LoadFromString,
			SetAutoSaveOn,
			SetAutoSaveOff,
			CopySection,
			LoadAlterValue,
			LoadPosition,
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			GetSecItem_Value,
			GetSecItem_String,
			GetCurrentSection,
			GetCurrentItem,
			SaveToString,
			SaveAlterValue,
			SavePosition,
			0
			};