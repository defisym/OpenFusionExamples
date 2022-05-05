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
		IDMN_CONDITION_OISSV, M_CONDITION_OISSV, CND_CONDITION_OISSV, 0, 1, PARAM_EXPSTRING, M_CND_LOOPNAME,
		IDMN_CONDITION_OIKWPV, M_CONDITION_OIKWPV, CND_CONDITION_OIKWPV, 0, 1, PARAM_EXPSTRING, M_CND_LOOPNAME,
		IDMN_CONDITION_OIMSS, M_CONDITION_OIMSS, CND_CONDITION_OIMSS, 0, 1, PARAM_EXPSTRING, M_CND_LOOPNAME,

		IDMN_CONDITION_SMR, M_CONDITION_SMR, CND_CONDITION_SMR, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACT_STR, M_ACT_REGEX,
		IDMN_CONDITION_SHR, M_CONDITION_SHR, CND_CONDITION_SHR, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_ACT_STR, M_ACT_REGEX,

		IDMN_CONDITION_OIRPE, M_CONDITION_OIRPE, CND_CONDITION_OIRPE, 0, 1, PARAM_EXPSTRING, M_CND_LOOPNAME,
		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_RP, M_ACTION_RP, ACT_ACTION_RP,	0, 0,

		IDMN_ACTION_LFF, M_ACTION_LFF, ACT_ACTION_LFF,	0, 3,PARAM_FILENAME2,PARAM_EXPSTRING,PARAM_EXPRESSION,M_ACT_FILENAME,M_ACT_KEY,M_ACT_UNICODE,
		IDMN_ACTION_LFS, M_ACTION_LFS, ACT_ACTION_LFS,	0, 1,PARAM_EXPSTRING,M_ACT_STR,
		
		IDMN_ACTION_RRF, M_ACTION_RRF, ACT_ACTION_RRF,	0, 0,
		IDMN_ACTION_SRF, M_ACTION_SRF, ACT_ACTION_SRF,	0, 1,PARAM_EXPRESSION,M_ACT_SRF,
		IDMN_ACTION_SCI, M_ACTION_SCI, ACT_ACTION_SCI,	0, 1,PARAM_EXPRESSION,M_ACT_SCI,

		IDMN_ACTION_SR, M_ACTION_SR, ACT_ACTION_SR,	0, 5,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACT_SR_S,M_ACT_SR_EL,M_ACT_SR_C,M_ACT_SR_ID,M_ACT_SR_K,
		IDMN_ACTION_SR_S, M_ACTION_SR_S, ACT_ACTION_SR_S,	0, 1,PARAM_EXPSTRING,M_ACT_SR_S,
		IDMN_ACTION_SR_EL, M_ACTION_SR_EL, ACT_ACTION_SR_EL,	0, 1,PARAM_EXPSTRING,M_ACT_SR_EL,
		IDMN_ACTION_SR_C, M_ACTION_SR_C, ACT_ACTION_SR_C,	0, 1,PARAM_EXPSTRING,M_ACT_SR_C,
		IDMN_ACTION_SR_ID, M_ACTION_SR_ID, ACT_ACTION_SR_ID,	0, 1,PARAM_EXPSTRING,M_ACT_SR_ID,
		IDMN_ACTION_SR_K, M_ACTION_SR_K, ACT_ACTION_SR_K,	0, 1,PARAM_EXPSTRING,M_ACT_SR_K,

		IDMN_ACTION_SD, M_ACTION_SD, ACT_ACTION_SD,	0, 0,

		IDMN_ACTION_ITSSV, M_ACTION_ITSSV, ACT_ACTION_ITSSV,	0, 1,PARAM_EXPSTRING,M_CND_LOOPNAME,
		IDMN_ACTION_ITKWPV, M_ACTION_ITKWPV, ACT_ACTION_ITKWPV,	0, 1,PARAM_EXPSTRING,M_CND_LOOPNAME,

		IDMN_ACTION_GASSIS, M_ACTION_GASSIS, ACT_ACTION_GASSIS,	0, 2,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACT_STR,M_ACT_REGEX,
		IDMN_ACTION_ITMSS, M_ACTION_ITMSS, ACT_ACTION_ITMSS,	0, 1,PARAM_EXPSTRING,M_CND_LOOPNAME,

		IDMN_ACTION_AS, M_ACTION_AS, ACT_ACTION_AS,	0, 1,PARAM_EXPRESSION,M_ACT_AUTO,

		IDMN_ACTION_ITRE, M_ACTION_ITRE, ACT_ACTION_ITRE,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACT_STR,M_ACT_REGEX,M_CND_LOOPNAME,
		IDMN_ACTION_ITRE_SRS, M_ACTION_ITRE_SRS, ACT_ACTION_ITRE_SRS,	0, 1,PARAM_EXPSTRING,M_ACT_STR,
		IDMN_ACTION_ITREA, M_ACTION_ITREA, ACT_ACTION_ITREA,	0, 3,PARAM_EXPSTRING,PARAM_EXPSTRING,PARAM_EXPSTRING,M_ACT_STR,M_ACT_REGEX,M_CND_LOOPNAME,
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_RS, M_EXPRESSION_RS, EXP_EXPRESSION_RS, EXPFLAG_STRING, 3, EXPPARAM_STRING, EXPPARAM_STRING, EXPPARAM_STRING, M_ACT_STR,M_ACT_REGEX,M_EXP_REPREGEX,
		
		IDMN_EXPRESSION_GMSSP, M_EXPRESSION_GMSSP, EXP_EXPRESSION_GMSSP, 0, 3, EXPPARAM_STRING,EXPPARAM_STRING,EXPPARAM_LONG,M_ACT_STR,M_ACT_REGEX,M_EXP_NTH,

		IDMN_EXPRESSION_GLMSS, M_EXPRESSION_GLMSS, EXP_EXPRESSION_GLMSS, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GMSSPIS, M_EXPRESSION_GMSSPIS, EXP_EXPRESSION_GMSSPIS, EXPFLAG_STRING, 3, EXPPARAM_STRING, EXPPARAM_STRING,EXPPARAM_LONG, M_ACT_STR,M_ACT_REGEX,M_EXP_NTH,

		IDMN_EXPRESSION_GMSSS, M_EXPRESSION_GMSSS, EXP_EXPRESSION_GMSSS, 0, 0,
		IDMN_EXPRESSION_GMSSIMSSV, M_EXPRESSION_GMSSIMSSV, EXP_EXPRESSION_GMSSIMSSV, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXP_POS,

		IDMN_EXPRESSION_ITGCMSS, M_EXPRESSION_ITGCMSS, EXP_EXPRESSION_ITGCMSS, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GSSVS, M_EXPRESSION_GSSVS, EXP_EXPRESSION_GSSVS, 0, 0,
		IDMN_EXPRESSION_GSSISSV, M_EXPRESSION_GSSISSV, EXP_EXPRESSION_GSSISSV, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXP_POS,

		IDMN_EXPRESSION_ITGCSS, M_EXPRESSION_ITGCSS, EXP_EXPRESSION_ITGCSS, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GNKW, M_EXPRESSION_GNKW, EXP_EXPRESSION_GNKW, EXPFLAG_STRING, 2, EXPPARAM_LONG, EXPPARAM_STRING, M_EXP_STARTPOS,M_ACT_REGEX,
		IDMN_EXPRESSION_GNKWP, M_EXPRESSION_GNKWP, EXP_EXPRESSION_GNKWP, 0, 2, EXPPARAM_LONG, EXPPARAM_STRING, M_EXP_STARTPOS,M_ACT_REGEX,

		IDMN_EXPRESSION_GKWPVS, M_EXPRESSION_GKWPVS, EXP_EXPRESSION_GKWPVS, 0, 0,
		IDMN_EXPRESSION_GKWIKWPV, M_EXPRESSION_GKWIKWPV, EXP_EXPRESSION_GKWIKWPV, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_EXP_POS,
		IDMN_EXPRESSION_GKWPIKWPV, M_EXPRESSION_GKWPIKWPV, EXP_EXPRESSION_GKWPIKWPV, 0, 1,EXPPARAM_LONG, M_EXP_POS,

		IDMN_EXPRESSION_ITGCKW, M_EXPRESSION_ITGCKW, EXP_EXPRESSION_ITGCKW, EXPFLAG_STRING, 0,		
		IDMN_EXPRESSION_ITGCKWP, M_EXPRESSION_ITGCKWP, EXP_EXPRESSION_ITGCKWP, 0, 0,

		IDMN_EXPRESSION_GSD, M_EXPRESSION_GSD, EXP_EXPRESSION_GSD, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GRECM, M_EXPRESSION_GRECM, EXP_EXPRESSION_GRECM, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GRES, M_EXPRESSION_GRES, EXP_EXPRESSION_GRES, EXPFLAG_STRING, 0,
		
		IDMN_EXPRESSION_GSSVP, M_EXPRESSION_GSSVP, EXP_EXPRESSION_GSSVP, 0, 0,
		};


// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport OnIterateSplitStrVec(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->SplitStrVecLoopName) ? TRUE : FALSE;
}
long WINAPI DLLExport OnIterateKeyWordPairVec(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->KeyWordPairVecLoopName) ? TRUE : FALSE;
}

long WINAPI DLLExport OnIterateSubStringVec(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->SubStringVecLoopName) ? TRUE : FALSE;
}


long WINAPI DLLExport StringMatchRegex(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));

	if (!StrEmpty(Src)) {
		return Spliter->StringMatchRegex(Src, Regex.c_str(), false);
	}
	else {
		return Spliter->StringMatchRegex(Regex.c_str(), false);
	}
}
long WINAPI DLLExport StringHasRegex(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));

	if (!StrEmpty(Src)) {
		return Spliter->StringMatchRegex(Src, Regex.c_str(), true);
	}
	else {
		return Spliter->StringMatchRegex(Regex.c_str(), true);
	}
}

long WINAPI DLLExport OnIterateReplaceEach(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(LoopName, rdPtr->ReplaceEachLoopName) ? TRUE : FALSE;
}

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport ResetSplit(LPRDATA rdPtr, long param1, long param2) {
	Spliter->ResetSplit();

	ResertPtr(rdPtr->CurrentSplitStr);
	ResertPtr(rdPtr->CurrentKeyWord);
	ResertPtr(rdPtr->CurrentSubString);

	return 0;
}

short WINAPI DLLExport LoadFromFile(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	bool Enable = (bool)CNC_GetIntParameter(rdPtr);

	Spliter->LoadFile(FilePath, Key, Enable);	

	if (rdPtr->AutoSplit) {
		Spliter->SplitData();
	}

	return 0;
}
short WINAPI DLLExport LoadFromString(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR String = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	Spliter->LoadData(String);

	if (rdPtr->AutoSplit) {
		Spliter->SplitData();
	}

	return 0;
}

short WINAPI DLLExport ResetRegexFlag(LPRDATA rdPtr, long param1, long param2) {
	Spliter->ReSetRegexFlag();
	return 0;
}
short WINAPI DLLExport SetRegexFlag(LPRDATA rdPtr, long param1, long param2) {
	RegexFlag Flag = (RegexFlag)CNC_GetIntParameter(rdPtr);
	Spliter->SetRegexFlag(Flag);
	return 0;
}
short WINAPI DLLExport SetCaseInsensitive(LPRDATA rdPtr, long param1, long param2) {
	bool Enable = (bool)CNC_GetIntParameter(rdPtr);
	Spliter->SetCaseInsensitive(Enable);
	return 0;
}

short WINAPI DLLExport SetRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Split = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	const std::wstring EmptyLine = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	const std::wstring Comment = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	const std::wstring Indent = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	const std::wstring Keyword = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));

	Spliter->InitRegex(Split.c_str(), EmptyLine.c_str(), Comment.c_str(), Indent.c_str(), Keyword.c_str());

	return 0;
}

short WINAPI DLLExport SetSplitRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	Spliter->InitSplit(Regex.c_str());
	return 0;
}
short WINAPI DLLExport SetEmptyLineRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	Spliter->InitEmptyLine(Regex.c_str());
	return 0;
}
short WINAPI DLLExport SetCommentRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	Spliter->InitComment(Regex.c_str());
	return 0;
}
short WINAPI DLLExport SetIndentRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	Spliter->InitIndent(Regex.c_str());
	return 0;
}short WINAPI DLLExport SetKeyWordRegex(LPRDATA rdPtr, long param1, long param2) {
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	Spliter->InitKeyWord(Regex.c_str());
	return 0;
}

short WINAPI DLLExport SetAutoSplit(LPRDATA rdPtr, long param1, long param2) {
	bool Enable = (bool)CNC_GetIntParameter(rdPtr);
	rdPtr->AutoSplit = Enable;

	return 0;
}

short WINAPI DLLExport SplitData(LPRDATA rdPtr, long param1, long param2) {
	Spliter->SplitData();
	return 0;
}

short WINAPI DLLExport IterateSplitStrVec(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->SplitStrVecLoopName, LoopName);
	for (size_t i = 0; i < Spliter->GetSplitSize(); i++) {
		rdPtr->CurrentSplitStr = (*Spliter)[i];
		CallEvent(ONIT_SSV);
	}
	return 0;
}
short WINAPI DLLExport IterateKeyWordPairVec(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->KeyWordPairVecLoopName, LoopName);
	for (size_t i = 0; i < Spliter->GetKeyWordPairVecSize(); i++) {		
		rdPtr->CurrentKeyWord = Spliter->GetKeyWord(i);
		rdPtr->CurrentKeyWordPos = Spliter->GetKeyWordPos(i);
		CallEvent(ONIT_KWPV);
	}
	return 0;
}

short WINAPI DLLExport GetAllSubStringInString(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));

	if (!StrEmpty(Src)) {
		Spliter->GetAllSubString(Src, Regex.c_str());
	}
	else {
		Spliter->GetAllSubString(Regex.c_str());
	}

	return 0;
}
short WINAPI DLLExport IterateMatchedSubString(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->SubStringVecLoopName, LoopName);
	for (size_t i = 0; i < Spliter->GetSubStringSize(); i++) {
		rdPtr->CurrentSubString = Spliter->GetSubString(i);
		CallEvent(ONIT_MSS);
	}
	return 0;
}

short WINAPI DLLExport IterateReplaceEach(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Src= (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->ReplaceEachLoopName, LoopName);

	wregex SubString(Regex, Spliter->GetRegexFlag());
	wsmatch MatchedStr;

	std::wstring::const_iterator StrBegin = Src.begin();
	std::wstring::const_iterator StrEnd = Src.end();

	wregex Replace;
	std::wstring Result = Src;	

	for (; regex_search(StrBegin, StrEnd, MatchedStr, SubString); StrBegin = MatchedStr[0].second) {
		//Get match result
		std::wstring MatchResult= MatchedStr[0].str().c_str();
		rdPtr->CurrentMatchString = MatchResult.c_str();
		
		//Call event, update replace string
		CallEvent(ONIT_RPE);

		//DO replace
		if (rdPtr->CurrentReplaceString != nullptr) {
			Replace.assign(MatchedStr[0].str());
			Result = regex_replace(Result, Replace, rdPtr->CurrentReplaceString);
		}

		delete rdPtr->CurrentReplaceString;
		rdPtr->CurrentReplaceString = nullptr;
	}

	NewStr(rdPtr->ReplacEachResult, Result.c_str());

	return 0;
}

short WINAPI DLLExport IterateReplaceEachAll(LPRDATA rdPtr, long param1, long param2) {
	std::wstring Src = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	const std::wstring Regex = NewLineEscape((LPCTSTR)CNC_GetStringParameter(rdPtr));
	LPCTSTR LoopName = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->ReplaceEachLoopName, LoopName);

	wregex SubString(Regex, Spliter->GetRegexFlag());
	wsmatch MatchedStr;

	do {
		std::wstring::const_iterator StrBegin = Src.begin();
		std::wstring::const_iterator StrEnd = Src.end();

		wregex Replace;
		std::wstring Result = Src;

		for (; regex_search(StrBegin, StrEnd, MatchedStr, SubString); StrBegin = MatchedStr[0].second) {
			//Get match result
			std::wstring MatchResult = MatchedStr[0].str().c_str();
			rdPtr->CurrentMatchString = MatchResult.c_str();

			//Call event, update replace string
			CallEvent(ONIT_RPE);

			//DO replace
			if (rdPtr->CurrentReplaceString != nullptr) {
				Replace.assign(MatchedStr[0].str());
				Result = regex_replace(Result, Replace, rdPtr->CurrentReplaceString);
			}

			delete rdPtr->CurrentReplaceString;
			rdPtr->CurrentReplaceString = nullptr;
		}

		//No replace -> all needed substring has been replaced
		if (Src == Result) {
			break;
		}

		Src = Result;		
	} while (regex_search(Src, SubString));

	NewStr(rdPtr->ReplacEachResult, Src.c_str());

	return 0;
}

short WINAPI DLLExport IterateReplaceEachSetReplaceString(LPRDATA rdPtr, long param1, long param2) {
	LPCTSTR Replace = (LPCTSTR)CNC_GetStringParameter(rdPtr);
	NewStr(rdPtr->CurrentReplaceString, Replace);

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport ReplaceString(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Src = (LPCTSTR)p1;
	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Regex = NewLineEscape((LPCTSTR)p2);
	long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Replace = (LPCTSTR)p3;

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	LPCTSTR Result = nullptr;

	if (!StrEmpty(Src)) {
		Result = Spliter->ReplaceStr(Src, Regex.c_str(), Replace);
	}
	else {
		Result = Spliter->ReplaceStr(Regex.c_str(), Replace);
	}

	return (long)(Result != nullptr ? Result : Default_Str);
}

long WINAPI DLLExport GetSubStringPosInString(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Src = (LPCTSTR)p1;
	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Regex = NewLineEscape((LPCTSTR)p2);

	//Keep this to int cause we need to use -1 as max size_t
	int Pos = (int)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	if (!StrEmpty(Src)) {
		return Spliter->GetSubStringPos(Src, Regex.c_str(), Pos);
	}
	else {
		return Spliter->GetSubStringPos(Regex.c_str(), Pos);
	}
}

long WINAPI DLLExport GetLastMatchedSubString(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;
	//This returns a pointer to the string for MMF.
	return (long)(Spliter->GetMatchResult() != nullptr ? Spliter->GetMatchResult() : Default_Str);
}
long WINAPI DLLExport GetMatchedSubStringInString(LPRDATA rdPtr, long param1) {
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Src = (LPCTSTR)p1;
	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Regex = NewLineEscape((LPCTSTR)p2);

	int Pos = (int)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	LPCTSTR Result = nullptr;

	if (!StrEmpty(Src)) {
		Result = Spliter->GetMatchResult(Src, Regex.c_str(), Pos);
	}
	else {
		Result = Spliter->GetMatchResult(Regex.c_str(), Pos);
	}

	return (long)(Result != nullptr ? Result : Default_Str);
}

long WINAPI DLLExport GetSubStringVecSize(LPRDATA rdPtr, long param1) {
	return Spliter->GetSubStringSize();
}
long WINAPI DLLExport GetMatchedSubStringInSubStringVec(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(Spliter->GetSubString(Pos) != nullptr ? Spliter->GetSubString(Pos) : Default_Str);
}

long WINAPI DLLExport GetCurrentSubString(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->CurrentSubString != nullptr ? rdPtr->CurrentSubString : Default_Str);
}

long WINAPI DLLExport GetSplitStrVecSize(LPRDATA rdPtr, long param1) {
	return Spliter->GetSplitSize();
}
long WINAPI DLLExport GetSplitStrInSplitStrVec(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)((*Spliter)[Pos] != nullptr ? (*Spliter)[Pos] : Default_Str);
}

long WINAPI DLLExport GetCurrentSplitStr(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->CurrentSplitStr != nullptr ? rdPtr->CurrentSplitStr : Default_Str);
}

long WINAPI DLLExport GetNextKeyWord(LPRDATA rdPtr, long param1) {
	size_t StartPos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	long p1 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Regex = NewLineEscape((LPCTSTR)p1);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	LPCTSTR Result = nullptr;

	if (!StrEmpty(Regex.c_str())) {
		Result = Spliter->GetNextKeyWord(StartPos, Regex.c_str());
	}
	else {
		Result = Spliter->GetNextKeyWord(StartPos);
	}
	return (long)(Result != nullptr ? Result : Default_Str);
}
long WINAPI DLLExport GetNextKeyWordPos(LPRDATA rdPtr, long param1) {
	size_t StartPos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	long p1 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);
	const std::wstring Regex = NewLineEscape((LPCTSTR)p1);

	if (!StrEmpty(Regex.c_str())) {
		return Spliter->GetNextKeyWordPos(StartPos, Regex.c_str());
	}
	else {
		return Spliter->GetNextKeyWordPos(StartPos);
	}
}

long WINAPI DLLExport GetKeyWordPairVecSize(LPRDATA rdPtr, long param1) {
	return Spliter->GetKeyWordPairVecSize();
}
long WINAPI DLLExport GetKeyWordInKeyWordPairVec(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(Spliter->GetKeyWord(Pos) != nullptr ? Spliter->GetKeyWord(Pos) : Default_Str);
}
long WINAPI DLLExport GetKeyWordPosInKeyWordPairVec(LPRDATA rdPtr, long param1) {
	size_t Pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	return Spliter->GetKeyWordPos(Pos);
}

long WINAPI DLLExport GetCurrentKeyWord(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->CurrentKeyWord != nullptr ? rdPtr->CurrentKeyWord : Default_Str);
}
long WINAPI DLLExport GetCurrentKeyWordPos(LPRDATA rdPtr, long param1) {
	return rdPtr->CurrentKeyWordPos;
}

long WINAPI DLLExport GetSplitData(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(Spliter->GetSplitData() != nullptr ? Spliter->GetSplitData() : Default_Str);
}

long WINAPI DLLExport GetReplaceEachCurrentMatch(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->CurrentMatchString != nullptr ? rdPtr->CurrentMatchString : Default_Str);
}

long WINAPI DLLExport GetReplaceEachResult(LPRDATA rdPtr, long param1) {	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->ReplacEachResult != nullptr ? rdPtr->ReplacEachResult : Default_Str);
}

long WINAPI DLLExport GetSubStringVecPointer(LPRDATA rdPtr, long param1) {
	const std::vector<std::wstring>* pSubVec = Spliter->GetSubStringVec();
	
	return ConvertToLong(pSubVec);
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
			OnIterateSplitStrVec,
			OnIterateKeyWordPairVec,

			OnIterateSubStringVec,

			StringMatchRegex,
			StringHasRegex,

			OnIterateReplaceEach,
			
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			ResetSplit,

			LoadFromFile,
			LoadFromString,

			ResetRegexFlag,
			SetRegexFlag,
			SetCaseInsensitive,

			SetRegex,
			SetSplitRegex,
			SetEmptyLineRegex,
			SetCommentRegex,
			SetIndentRegex,
			SetKeyWordRegex,

			SplitData,

			IterateSplitStrVec,
			IterateKeyWordPairVec,

			GetAllSubStringInString,
			IterateMatchedSubString,

			SetAutoSplit,
			
			IterateReplaceEach,
			IterateReplaceEachSetReplaceString,
			IterateReplaceEachAll,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			ReplaceString,

			GetSubStringPosInString,

			GetLastMatchedSubString,
			GetMatchedSubStringInString,

			GetSubStringVecSize,
			GetMatchedSubStringInSubStringVec,

			GetCurrentSubString,

			GetSplitStrVecSize,
			GetSplitStrInSplitStrVec,

			GetCurrentSplitStr,

			GetNextKeyWord,
			GetNextKeyWordPos,

			GetKeyWordPairVecSize,
			GetKeyWordInKeyWordPairVec,
			GetKeyWordPosInKeyWordPairVec,

			GetCurrentKeyWord,
			GetCurrentKeyWordPos,

			GetSplitData,

			GetReplaceEachCurrentMatch,
			GetReplaceEachResult,

			GetSubStringVecPointer,

			0
			};