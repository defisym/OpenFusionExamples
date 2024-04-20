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
		IDMN_CONDITION_OGOIC, M_CONDITION_OGOIC, CND_CONDITION_OGOIC, 0, 1, PARAM_EXPSTRING, M_ICONITNAME,
		IDMN_CONDITION_OTAGCB, M_CONDITION_OTAGCB, CND_CONDITION_OTAGCB, 0, 1, PARAM_EXPSTRING, M_TAGCBNAME,
		IDMN_CONDITION_OTAGCBF, M_CONDITION_OTAGCBF, CND_CONDITION_OTAGCBF, 0, 0,
		IDMN_CONDITION_POTR, M_CONDITION_POTR, CND_CONDITION_POTR, EVFLAGS_ALWAYS | EVFLAGS_NOTABLE, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPSTRING, M_TRIGGERX, M_TRIGGERY, M_TRIGGERNAME,

		};

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION_CS, M_ACTION_CS, ACT_ACTION_CS,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,
		IDMN_ACTION_CSTR, M_ACTION_CSTR, ACT_ACTION_CSTR,	0, 1, PARAM_EXPSTRING, M_STR,
		IDMN_ACTION_CRS, M_ACTION_CRS, ACT_ACTION_CRS,	0, 1, PARAM_EXPRESSION, M_ROWSPACE,
		IDMN_ACTION_CCS, M_ACTION_CCS, ACT_ACTION_CCS,	0, 1, PARAM_EXPRESSION, M_COLSPACE,

		IDMN_ACTION_CHA, M_ACTION_CHA, ACT_ACTION_CHA,	0, 1, PARAM_EXPRESSION, M_HA,
		IDMN_ACTION_CVA, M_ACTION_CVA, ACT_ACTION_CVA,	0, 1, PARAM_EXPRESSION, M_VA,

		IDMN_ACTION_ASTR, M_ACTION_ASTR, ACT_ACTION_ASTR,	0, 1, PARAM_EXPSTRING, M_STR,
		IDMN_ACTION_ASTRNL, M_ACTION_ASTRNL, ACT_ACTION_ASTRNL,	0, 1, PARAM_EXPSTRING, M_STR,
		
		IDMN_ACTION_SH, M_ACTION_SH, ACT_ACTION_SH,	0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, IDS_PROP_HOTSPOT_DEFAULT_INFO, IDS_PROP_HOTSPOT_X, IDS_PROP_HOTSPOT_Y,
		IDMN_ACTION_Z, M_ACTION_Z, ACT_ACTION_Z,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_XSCALE, M_YSCALE,
		IDMN_ACTION_S, M_ACTION_S, ACT_ACTION_S,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_WIDTH, M_HEIGHT,
		IDMN_ACTION_R, M_ACTION_R, ACT_ACTION_R,	0, 1, PARAM_EXPRESSION, M_ANGLE,

		IDMN_ACTION_EF, M_ACTION_EF, ACT_ACTION_EF,	0, 2, PARAM_EXPSTRING, PARAM_EXPSTRING, M_FONTNAME, M_KEY,

		IDMN_ACTION_LA, M_ACTION_LA, ACT_ACTION_LA,	0, 1, PARAM_OBJECT, M_ACTIVE,
		
		IDMN_ACTION_SIO, M_ACTION_SIO, ACT_ACTION_SIO,	0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_ICONXOFFSET, M_ICONYOFFSET,
		IDMN_ACTION_SIS, M_ACTION_SIS, ACT_ACTION_SIS,	0, 1, PARAM_EXPRESSION, M_ICONSCALE,
		IDMN_ACTION_SIR, M_ACTION_SIR, ACT_ACTION_SIR,	0, 1, PARAM_EXPRESSION, M_ICONRESAMPLE,

		IDMN_ACTION_SVO, M_ACTION_SVO, ACT_ACTION_SVO,	0, 1, PARAM_EXPRESSION, M_VERTICALOFFSET,
		
		IDMN_ACTION_LO, M_ACTION_LO, ACT_ACTION_LO,	0, 2, PARAM_OBJECT, PARAM_EXPSTRING, M_OBJECT, M_ICONITNAME,
		IDMN_ACTION_SOKV, M_ACTION_SOKV, ACT_ACTION_SOKV, 0, 3, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_EXPRESSION, M_HASH, M_PSF, M_OVERWRITE,
		
		IDMN_ACTION_SFF, M_ACTION_SFF, ACT_ACTION_SFF, 0, 1, PARAM_EXPRESSION, M_FILTERFLAG,

		IDMN_ACTION_FRD, M_ACTION_FRD, ACT_ACTION_FRD, 0, 0,
		IDMN_ACTION_FRDGI, M_ACTION_FRDGI, ACT_ACTION_FRDGI, 0, 0,
		
		IDMN_ACTION_RENDER, M_ACTION_RENDER, ACT_ACTION_RENDER, 0, 0,

		IDMN_ACTION_SRO_VR, M_ACTION_SRO_VR, ACT_ACTION_SRO_VR, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_VISIBLERATIO, M_INCLUDEALPHA,

		IDMN_ACTION_STS, M_ACTION_STS, ACT_ACTION_STS, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_TABSIZE, M_TABEM,

		IDMN_ACTION_SROFFSET, M_ACTION_SROFFSET, ACT_ACTION_SROFFSET, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_REMARKXOFFSET, M_REMARKYOFFSET,

		IDMN_ACTION_FNF, M_ACTION_FNF, ACT_ACTION_FNF,	0, 1, PARAM_EXPSTRING, M_FMT,
		IDMN_ACTION_APS, M_ACTION_APS, ACT_ACTION_APS,	0, 1, PARAM_EXPSTRING, M_PARAM,
		IDMN_ACTION_APV, M_ACTION_APV, ACT_ACTION_APV,	0, 2, PARAM_EXPRESSION, PARAM_EXPSTRING, M_PARAM, M_FMT,

		IDMN_ACTION_SRO_STAGCBIDX, M_ACTION_SRO_STAGCBIDX, ACT_ACTION_SRO_STAGCBIDX, 0, 1, PARAM_EXPRESSION, M_STAGCBIDX,
		IDMN_ACTION_SRO_STAGCBIDXM, M_ACTION_SRO_STAGCBIDXM, ACT_ACTION_SRO_STAGCBIDXM, 0, 1, PARAM_EXPRESSION, M_STAGCBIDXM,

		IDMN_ACTION_SCROLL_D, M_ACTION_SCROLL_D, ACT_ACTION_SCROLL_D, 0, 0,
		IDMN_ACTION_SCROLL_SSC, M_ACTION_SCROLL_SSC, ACT_ACTION_SCROLL_SSC, 0, 2, PARAM_EXPRESSION, PARAM_EXPRESSION, M_SCROLLCOEFX, M_SCROLLCOEFY,

		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{
		IDMN_EXPRESSION_GSTR, M_EXPRESSION_GSTR, EXP_EXPRESSION_GSTR, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GRS, M_EXPRESSION_GRS, EXP_EXPRESSION_GRS, 0, 0,
		IDMN_EXPRESSION_GCS, M_EXPRESSION_GCS, EXP_EXPRESSION_GCS, 0, 0,
		
		IDMN_EXPRESSION_GLCX, M_EXPRESSION_GLCX, EXP_EXPRESSION_GLCX, 0, 0,
		IDMN_EXPRESSION_GLCY, M_EXPRESSION_GLCY, EXP_EXPRESSION_GLCY, 0, 0,
		IDMN_EXPRESSION_GMW, M_EXPRESSION_GMW, EXP_EXPRESSION_GMW, 0, 0,

		IDMN_EXPRESSION_GHA, M_EXPRESSION_GHA, EXP_EXPRESSION_GHA, 0, 0,
		IDMN_EXPRESSION_GVA, M_EXPRESSION_GVA, EXP_EXPRESSION_GVA, 0, 0,

		IDMN_EXPRESSION_GTH, M_EXPRESSION_GTH, EXP_EXPRESSION_GTH, 0, 0,
		
		IDMN_EXPRESSION_GRW, M_EXPRESSION_GRW, EXP_EXPRESSION_GRW, 0, 0,
		IDMN_EXPRESSION_GRH, M_EXPRESSION_GRH, EXP_EXPRESSION_GRH, 0, 0,
		IDMN_EXPRESSION_GHX, M_EXPRESSION_GHX, EXP_EXPRESSION_GHX, 0, 0,
		IDMN_EXPRESSION_GHY, M_EXPRESSION_GHY, EXP_EXPRESSION_GHY, 0, 0,
		IDMN_EXPRESSION_GXS, M_EXPRESSION_GXS, EXP_EXPRESSION_GXS, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GYS, M_EXPRESSION_GYS, EXP_EXPRESSION_GYS, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GA, M_EXPRESSION_GA, EXP_EXPRESSION_GA, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GFN, M_EXPRESSION_GFN, EXP_EXPRESSION_GFN, EXPFLAG_STRING, 3, EXPPARAM_STRING, EXPPARAM_STRING, EXPPARAM_LONG, M_FONTNAME, M_KEY, M_POS,

		IDMN_EXPRESSION_GCX, M_EXPRESSION_GCX, EXP_EXPRESSION_GCX, 0, 1, EXPPARAM_LONG, M_POS,
		IDMN_EXPRESSION_GCY, M_EXPRESSION_GCY, EXP_EXPRESSION_GCY, 0, 1, EXPPARAM_LONG, M_POS,

		IDMN_EXPRESSION_GIOX, M_EXPRESSION_GIOX, EXP_EXPRESSION_GIOX, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GIOY, M_EXPRESSION_GIOY, EXP_EXPRESSION_GIOY, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GIS, M_EXPRESSION_GIS, EXP_EXPRESSION_GIS, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GIR, M_EXPRESSION_GIR, EXP_EXPRESSION_GIR, EXPFLAG_DOUBLE, 0,
		
		IDMN_EXPRESSION_GVO, M_EXPRESSION_GVO, EXP_EXPRESSION_GVO, 0, 0,
		
		IDMN_EXPRESSION_GFS, M_EXPRESSION_GFS, EXP_EXPRESSION_GFS, EXPFLAG_STRING, 2, EXPPARAM_STRING, EXPPARAM_LONG, M_STRING, M_FILTERFLAGS,
		IDMN_EXPRESSION_GPS, M_EXPRESSION_GPS, EXP_EXPRESSION_GPS, EXPFLAG_STRING, 1, EXPPARAM_STRING, M_STRING,

		IDMN_EXPRESSION_GNCO, M_EXPRESSION_GNCO, EXP_EXPRESSION_GNCO, 0, 2, EXPPARAM_STRING, EXPPARAM_LONG, M_STRING, M_STRINGSTART,
		
		IDMN_EXPRESSION_GHS, M_EXPRESSION_GHS, EXP_EXPRESSION_GHS, 0, 1, EXPPARAM_STRING, M_STRING,
		IDMN_EXPRESSION_GIPN, M_EXPRESSION_GIPN, EXP_EXPRESSION_GIPN, 0, 0,
		IDMN_EXPRESSION_GIPS, M_EXPRESSION_GIPS, EXP_EXPRESSION_GIPS, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_POS,
		
		IDMN_EXPRESSION_GFF, M_EXPRESSION_GFF, EXP_EXPRESSION_GFF, 0, 0,
		
		IDMN_EXPRESSION_GRSBFSL, M_EXPRESSION_GRSBFSL, EXP_EXPRESSION_GRSBFSL, EXPFLAG_STRING, 3, EXPPARAM_STRING, EXPPARAM_LONG, EXPPARAM_LONG, M_STRING, M_POS, M_FILTERFLAGS,

		IDMN_EXPRESSION_GRO_VR, M_EXPRESSION_GRO_VR, EXP_EXPRESSION_GRO_VR, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GTS_TS, M_EXPRESSION_GTS_TS, EXP_EXPRESSION_GTS_TS, 0, 0,
		IDMN_EXPRESSION_GTS_EM, M_EXPRESSION_GTS_EM, EXP_EXPRESSION_GTS_EM, 0, 0,

		IDMN_EXPRESSION_GROX, M_EXPRESSION_GROX, EXP_EXPRESSION_GROX, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_GROY, M_EXPRESSION_GROY, EXP_EXPRESSION_GROY, EXPFLAG_DOUBLE, 0,

		IDMN_EXPRESSION_GFMTS, M_EXPRESSION_GFMTS, EXP_EXPRESSION_GFMTS, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_GTPN, M_EXPRESSION_GTPN, EXP_EXPRESSION_GTPN, 0, 0,
		IDMN_EXPRESSION_GTPS, M_EXPRESSION_GTPS, EXP_EXPRESSION_GTPS, EXPFLAG_STRING, 1, EXPPARAM_LONG, M_POS,
		IDMN_EXPRESSION_GTCBN, M_EXPRESSION_GTCBN, EXP_EXPRESSION_GTCBN, EXPFLAG_STRING, 0,

		IDMN_EXPRESSION_S_GCX, M_EXPRESSION_S_GCX, EXP_EXPRESSION_S_GCX, EXPFLAG_DOUBLE, 0,
		IDMN_EXPRESSION_S_GCY, M_EXPRESSION_S_GCY, EXP_EXPRESSION_S_GCY, EXPFLAG_DOUBLE, 0,
		
		IDMN_EXPRESSION_GCOTN, M_EXPRESSION_GCOTN, EXP_EXPRESSION_GCOTN, EXPFLAG_STRING, 0,
		IDMN_EXPRESSION_GCOTC, M_EXPRESSION_GCOTC, EXP_EXPRESSION_GCOTC, EXPFLAG_STRING, 0,
		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Condition_OnGetObjectICon(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pLoopName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(pLoopName, rdPtr->pIConItName->c_str());
}

long WINAPI DLLExport Condition_OnTagCallback(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pLoopName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	return StrEqu(pLoopName, rdPtr->pTagCallbackName->c_str());
}
long WINAPI DLLExport Condition_OnTagCallbackForward(LPRDATA rdPtr, long param1, long param2) {
	return true;
}
long WINAPI DLLExport Condition_PosOverlapTriggerRect(LPRDATA rdPtr, long param1, long param2) {
	auto x = CNC_GetIntParameter(rdPtr);
	auto y = CNC_GetIntParameter(rdPtr);
	const auto pTriggerName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	// render
	UpdateLastCharPos(rdPtr);

	// get hotspot
	int hotSpotX = rdPtr->hotSpotX;
	int hotSpotY = rdPtr->hotSpotY;

	UpdateHotSpot(rdPtr->hotSpotPos
		, rdPtr->rHo.hoImgWidth, rdPtr->rHo.hoImgHeight
		, hotSpotX, hotSpotY);

	// corret to left top of object
	x = x - rdPtr->rHo.hoX - hotSpotX;
	y = y - rdPtr->rHo.hoY - hotSpotY;

	// check
	rdPtr->currentTriggerIndex = rdPtr->pNeoStr->OverlapTrigger(x, y,
		!StrEmpty(pTriggerName)
		? pTriggerName
		: nullptr);

	return rdPtr->currentTriggerIndex != static_cast<size_t>(-1);
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport Action_ChangeRenderSize(LPRDATA rdPtr, long param1, long param2) {
	short width = TURNCATE_SHORT(CNC_GetIntParameter(rdPtr));
	short height = TURNCATE_SHORT(CNC_GetIntParameter(rdPtr));

	rdPtr->swidth = width;
	rdPtr->sheight = height;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeString(LPRDATA rdPtr, long param1, long param2) {
	const LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	struct IndexManagedHelper {
		LPRDATA rdPtr = nullptr;
		bool bAppend = false;

		IndexManagedHelper(LPRDATA rdPtr, const LPCWSTR pStr) {
			if (!rdPtr->bTagCallbackIndexManaged) { return; }

			this->rdPtr = rdPtr;
			this->bAppend = StringAppend(rdPtr->pStr->c_str(), pStr);
		}
		~IndexManagedHelper() {
			if (this->rdPtr == nullptr) { return; }

			const auto pOpt = static_cast<NeoStr::RenderOptions*>(this->rdPtr->pRenderOptions);

			// not append, reset
			if (!this->bAppend) {
				pOpt->tagCallbackIndex = 0;
			}
			// append, use previous render result
			else {

			}
		}
	};

	// RAII
	{
		IndexManagedHelper helper(rdPtr, pStr);
		*rdPtr->pStr = pStr;
	}

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_AppendString(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pStr->append(pStr);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_AppendStringNewLine(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	rdPtr->pStr->append(L"\r\n");
	rdPtr->pStr->append(pStr);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeRowSpace(LPRDATA rdPtr, long param1, long param2) {
	short nRowSpace = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->nRowSpace = nRowSpace;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeColSpace(LPRDATA rdPtr, long param1, long param2) {
	short nColSpace = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->nColSpace = nColSpace;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeHorizontalAlign(LPRDATA rdPtr, long param1, long param2) {
	short horizontalFlag = (short)CNC_GetIntParameter(rdPtr);
	
	rdPtr->dwAlignFlags = (rdPtr->dwAlignFlags & ~(DT_LEFT | DT_CENTER | DT_RIGHT)) | (horizontalFlag);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeVerticalAlign(LPRDATA rdPtr, long param1, long param2) {
	short verticalFlag = (short)CNC_GetIntParameter(rdPtr);

	rdPtr->dwAlignFlags = (rdPtr->dwAlignFlags & ~(DT_TOP | DT_VCENTER | DT_BOTTOM)) | (verticalFlag);

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ChangeTabSettings(LPRDATA rdPtr, long param1, long param2) {
	const auto tabSize = (unsigned char)CNC_GetIntParameter(rdPtr);
	const auto bTabEM = (bool)CNC_GetIntParameter(rdPtr);

	rdPtr->tabSize = tabSize;
	rdPtr->bTabEM = bTabEM;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetHotSpot(LPRDATA rdPtr, long param1, long param2) {
	HotSpotPos pos = (HotSpotPos)CNC_GetIntParameter(rdPtr);

	int x = (int)CNC_GetIntParameter(rdPtr);
	int y = (int)CNC_GetIntParameter(rdPtr);

	rdPtr->hotSpotPos = pos;

	UpdateHotSpot(rdPtr->hotSpotPos, rdPtr->swidth, rdPtr->sheight, x, y);

	rdPtr->hotSpotX = x;
	rdPtr->hotSpotY = y;

	ChangeScale(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_Zoom(LPRDATA rdPtr, long param1, long param2) {
	float xScale = GetFloatParam(rdPtr);
	float yScale = GetFloatParam(rdPtr);

	rdPtr->xScale = xScale;
	rdPtr->yScale = yScale;

	ChangeScale(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_Stretch(LPRDATA rdPtr, long param1, long param2) {
	int width = (int)CNC_GetIntParameter(rdPtr);
	int height = (int)CNC_GetIntParameter(rdPtr);

	float xScale = (1.0f * width / rdPtr->swidth);
	float yScale = (1.0f * height / rdPtr->sheight);

	rdPtr->xScale = xScale;
	rdPtr->yScale = yScale;

	ChangeScale(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_Rotate(LPRDATA rdPtr, long param1, long param2) {
	const float angle = fmod(GetFloatParam(rdPtr), 360.0f);
	rdPtr->angle = angle;

	ChangeScale(rdPtr);

	return 0;
}

// https://docs.microsoft.com/zh-cn/windows/win32/gdi/font-installation-and-deletion
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-addfontresourceexa
short WINAPI DLLExport Action_EmbedFont(LPRDATA rdPtr, long param1, long param2) {
	std::wstring FilePath = GetFullPathNameStr((LPCTSTR)CNC_GetStringParameter(rdPtr));
	LPCTSTR Key = (LPCTSTR)CNC_GetStringParameter(rdPtr);

	bool bFromMem = !StrEmpty(Key);
	Encryption* E = nullptr;

	if (bFromMem) {
		E = new Encryption;
		E->GenerateKey(Key);

		E->OpenFile(FilePath.c_str());
		E->Decrypt();
	}

	int ret = 0;
	DWORD fontNum = 0;

	auto flags = FR_PRIVATE;

	//ret = RemoveFontResourceEx(FilePath.c_str(), flags, 0);
	ret = bFromMem
		? (int)AddFontMemResourceEx(E->GetOutputData(), E->GetOutputDataLength(), 0, &fontNum)
		: AddFontResourceEx(FilePath.c_str(), flags, 0);

	if (ret == 0) {
		return 0;
	}

	auto gdipRet = bFromMem
		? rdPtr->pData->pFontCollection->AddMemoryFont(E->GetOutputData(), E->GetOutputDataLength())
		: rdPtr->pData->pFontCollection->AddFontFile(FilePath.c_str());

	if (gdipRet != Gdiplus::Status::Ok) {
		return 0;
	}

	auto fontNames = bFromMem
		? GetFontNameFromFile((LPCWSTR)E->GetOutputData(), E->GetOutputDataLength())
		: GetFontNameFromFile(FilePath.c_str());

	delete E;

#ifdef _FONTEMBEDDEBUG
	MSGBOX((std::wstring)L"Embed " + FilePath +
		(std::wstring)(gdipRet != Gdiplus::Status::Ok
			? L" Not OK"
			: L" OK"));
 
	auto font = rdPtr->pData->pFontCollection->GetFamilyCount();
 
	for (auto& i : fontNames) {
		wprintf(L"%s\n", i.c_str());
		//std::wcout << i << std::endl;
	}
#endif // _FONTEMBEDDEBUG

	//refresh objects
	ObjectSelection Oc(rdPtr->rHo.hoAdRunHeader);
	Oc.IterateObjectWithIdentifier(rdPtr, rdPtr->rHo.hoIdentifier, [fontNames](LPRO pObject) {
		LPRDATA pObj = (LPRDATA)pObject;

		std::wstring withRegular = (std::wstring)pObj->logFont.lfFaceName + (std::wstring)L" Regular";
		//std::wstring withNormal = (std::wstring)pObj->logFont.lfFaceName + (std::wstring)L" Normal";

		for (auto& name : fontNames) {
			if (StrIEqu(pObj->logFont.lfFaceName, name.c_str())
				|| StrIEqu(withRegular.c_str(), name.c_str())
				/*|| StrIEqu(withNormal.c_str(), name.c_str())*/) {
				SetRunObjectFont(pObj, &pObj->logFont, nullptr);
				
				return;
			}
		}

		});

	return 0;
}

short WINAPI DLLExport Action_LinkActive(LPRDATA rdPtr, long param1, long param2) {
	LPRO pObject = (LPRO)CNC_GetParameter(rdPtr);

	auto bDestroyCaller = IsDestroyed((LPRO)rdPtr);
	auto bDestroyObject = IsDestroyed(pObject);
	
	if (!bDestroyCaller
		&& pObject != nullptr && LPROValid(pObject, IDENTIFIER_ACTIVE) && !bDestroyObject) {
		rdPtr->pIConObject = pObject;

		GIPP(rdPtr->pIConParamParser) = [=](NeoStr::ControlParams& controlParams, NeoStr::IConLib& iConLib) {
			// check caller validity
			if (IsDestroyed((LPRO)rdPtr)) {
				return FORMAT_INVALID_ICON;
			}

			// parse param
			int frame = 0;
			int direction = 0;
			int animation = 0;

			do {
				std::reverse(controlParams.begin(), controlParams.end());

				if (!controlParams.empty()) {
					frame = _stoi(controlParams.back());
					controlParams.pop_back();
				}

				if (!controlParams.empty()) {
					direction = _stoi(controlParams.back());
					controlParams.pop_back();
				}

				if (!controlParams.empty()) {
					animation = _stoi(controlParams.back());
					controlParams.pop_back();
				}
			} while (0);
			
			// find image
			DWORD hImage = FORMAT_INVALID_ICON;
			auto appli = rdPtr->rHo.hoAdRunHeader->rhIdAppli;

			do {
				if (pObject == nullptr || appli == nullptr) {
					break;
				}

				if (animation < 0 || direction < 0 || frame < 0) {
					break;
				}

				auto pRoa = &pObject->roa;

				if (pRoa->raAnimOffset->anOffsetToDir[direction] < 0) {
					break;
				}

				auto pDir = &pRoa->raAnimDirOffset[direction];

				if (frame >= pDir->adNumberOfFrame) {
					break;
				}

				hImage = pDir->adFrame[frame];
			} while (0);

			if (hImage == FORMAT_INVALID_ICON) {
				return hImage;
			}

			// blit to lib
			auto IConLibIt = iConLib.find(hImage);

			if (IConLibIt == iConLib.end()) {
				cSurface imageSurface;
				LockImageSurface(appli, hImage, imageSurface);

				auto pIConSf = CreateSurface(32, imageSurface.GetWidth(), imageSurface.GetHeight());
				//pIConSf->SetTransparentColor(imageSurface.GetTransparentColor());
				//_AddAlpha(pIConSf);

				imageSurface.Blit(*pIConSf);
				//imageSurface.Blit(*pIConSf, 0, 0, BMODE_OPAQUE, BOP_COPY, 0, BLTF_ANTIA | BLTF_COPYALPHA);

				UnlockImageSurface(imageSurface);

				iConLib[hImage] = pIConSf;
			}

			return hImage;
		};

		GlobalIConUpdater(rdPtr);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_LinkObject(LPRDATA rdPtr, long param1, long param2) {
	LPRO pObject = (LPRO)CNC_GetParameter(rdPtr);
	LPCWSTR pLoopName = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	auto bDestroyCaller = IsDestroyed((LPRO)rdPtr);
	auto bDestroyObject = IsDestroyed(pObject);

	if (!bDestroyCaller
		&& pObject != nullptr && !bDestroyObject) {
		// pObject is not used, it just for human readibility, actual identifier is hash of pLoopName
		// for the case when switching frame, fixed/LPRO changes but using the same global data
		// to reduce unnecessary refresh

		//rdPtr->pIConObject = pObject;
		rdPtr->pIConObject = (LPRO)std::hash<std::wstring>{}(pLoopName);		

		GIPP(rdPtr->pIConParamParser) = 
			[=, loopName = std::wstring(pLoopName)]
			(NeoStr::ControlParams& controlParams, NeoStr::IConLib& iConLib) {
			// check caller validity
			if (IsDestroyed((LPRO)rdPtr)) {
				return FORMAT_INVALID_ICON;
			}
			
			// call callback
			rdPtr->bOverWrite = false;
			rdPtr->iconLibKey = FORMAT_INVALID_ICON;
			rdPtr->pIConLibValue = nullptr;

			*rdPtr->pIConItName = loopName;
			rdPtr->pIConParams = &controlParams;

			CallEvent(ONITOIC);

			rdPtr->pIConParams = nullptr;

			//// if overwrite, then all global icon objects will trigger update
			//if (rdPtr->bOverWrite) {
			//	SetIConUpdate(rdPtr);
			//}

			if (rdPtr->iconLibKey == FORMAT_INVALID_ICON) {
				return rdPtr->iconLibKey;
			}

			if (rdPtr->pIConLibValue == nullptr || !rdPtr->pIConLibValue->IsValid()) {
				return rdPtr->iconLibKey;
			}

			// blit to lib
			auto BlitToLib=[=](NeoStr::IConLib& iConLib){
				auto pIConSf = CreateSurface(32, rdPtr->pIConLibValue->GetWidth(), rdPtr->pIConLibValue->GetHeight());
				pIConSf->CreateAlpha();

				auto ret = rdPtr->pIConLibValue->Blit(*pIConSf);

				iConLib[rdPtr->iconLibKey] = pIConSf;

#ifdef _DEBUG
				//_SavetoClipBoard(rdPtr->pIConLibValue);
				//_SavetoClipBoard(pIConSf);
#endif // _DEBUG
			};

			auto IConLibIt = iConLib.find(rdPtr->iconLibKey);
			auto bExist = IConLibIt != iConLib.end();

			if (!bExist) {
				BlitToLib(iConLib);
			}
			else if (rdPtr->bOverWrite) {
				delete IConLibIt->second;
				IConLibIt->second = nullptr;

				BlitToLib(iConLib);
			}

			return rdPtr->iconLibKey;
		};

		GlobalIConUpdater(rdPtr);
		ReDisplay(rdPtr);
	}

	return 0;
}

short WINAPI DLLExport Action_SetObjectKeyValue(LPRDATA rdPtr, long param1, long param2) {
	size_t hash = (size_t)CNC_GetParameter(rdPtr);
	LPSURFACE pSf = (LPSURFACE)CNC_GetParameter(rdPtr);
	bool bOverWrite = (bool)CNC_GetParameter(rdPtr);

	rdPtr->iconLibKey = hash;
	rdPtr->pIConLibValue = pSf;
	rdPtr->bOverWrite = bOverWrite;

	return 0;
}

short WINAPI DLLExport Action_SetIConOffset(LPRDATA rdPtr, long param1, long param2) {
	const auto offsetX = GetFloatParam(rdPtr);
	const auto offsetY = GetFloatParam(rdPtr);

	rdPtr->iConOffsetX = offsetX;
	rdPtr->iConOffsetY = offsetY;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetIConScale(LPRDATA rdPtr, long param1, long param2) {
	auto scale = GetFloatParam(rdPtr);

	rdPtr->iConScale = scale;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetIConResample(LPRDATA rdPtr, long param1, long param2) {
	bool bResample = (bool)CNC_GetParameter(rdPtr);

	rdPtr->bIConResample = bResample;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetRemarkOffset(LPRDATA rdPtr, long param1, long param2) {
	const auto offsetX = GetFloatParam(rdPtr);
	const auto offsetY = GetFloatParam(rdPtr);

	rdPtr->remarkOffsetX = offsetX;
	rdPtr->remarkOffsetY = offsetY;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetVerticalOffset(LPRDATA rdPtr, long param1, long param2) {
	bool bVerticalAlignOffset = (bool)CNC_GetParameter(rdPtr);

	rdPtr->bVerticalAlignOffset = bVerticalAlignOffset;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetFilterFlag(LPRDATA rdPtr, long param1, long param2) {
	size_t newFlag = (size_t)CNC_GetIntParameter(rdPtr);

	rdPtr->filterFlags = newFlag;

	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ForceRedraw(LPRDATA rdPtr, long param1, long param2) {
	rdPtr->bIConNeedUpdate = true;
	ReDisplay(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_ForceRedrawGlobalICon(LPRDATA rdPtr, long param1, long param2) {
	SetIConUpdate(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_Render(LPRDATA rdPtr, long param1, long param2) {
	UpdateLastCharPos(rdPtr);

	return 0;
}

short WINAPI DLLExport Action_SetRenderOption_VisibleRatio(LPRDATA rdPtr, long param1, long param2) {
	const float visibleRatio = Range(GetFloatParam(rdPtr), 0.0f, 1.0f);
	const bool bIncludeAlpha = (bool)CNC_GetParameter(rdPtr);

	const auto pOpt = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);

	if(pOpt->visibleRatio != visibleRatio || pOpt->bIncludeAlpha != bIncludeAlpha) {
		pOpt->visibleRatio = visibleRatio;
		pOpt->bIncludeAlpha = bIncludeAlpha;

		rdPtr->reRender = true;
	}
	
	return 0;
}

short WINAPI DLLExport Action_SetRenderOption_TagCallbackIndex(LPRDATA rdPtr, long param1, long param2) {
	const auto idx = (size_t)CNC_GetParameter(rdPtr) - 1;

	rdPtr->bTagCallbackIndexManaged = false;
	const auto pOpt = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);

	if (pOpt->tagCallbackIndex != idx) {
		pOpt->tagCallbackIndex = idx;

		rdPtr->reRender = true;
	}

	return 0;
}

short WINAPI DLLExport Action_SetRenderOption_TagCallbackIndexManaged(LPRDATA rdPtr, long param1, long param2) {
	const auto bManaged = (bool)CNC_GetParameter(rdPtr);

	// no change
	if (rdPtr->bTagCallbackIndexManaged == bManaged) { return 0; }

	// changed
	rdPtr->bTagCallbackIndexManaged = bManaged;

	const auto pOpt = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);
	if (pOpt->UpdateTagCallbackIndex(rdPtr->pNeoStr)) {
		rdPtr->reRender = true;
	}

	return 0;
}


short WINAPI DLLExport Action_Format_NewFormat(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	delete rdPtr->pFormatByVector;
	rdPtr->pFormatByVector = new FormatByVector<std::wstring>(pStr);

	return 0;
}

short WINAPI DLLExport Action_Format_AddParamString(LPRDATA rdPtr, long param1, long param2) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->pFormatByVector == nullptr) { return 0; }
	rdPtr->pFormatByVector->AddParam(pStr);

	return 0;
}

short WINAPI DLLExport Action_Format_AddParamValue(LPRDATA rdPtr, long param1, long param2) {
	const auto value = GetFloatParam(rdPtr);
	const auto pFMT = (LPCWSTR)CNC_GetStringParameter(rdPtr);

	if (rdPtr->pFormatByVector == nullptr) { return 0; }
	rdPtr->pFormatByVector->AddParam(
		FormatByVector<std::wstring>::GetFormatString(
		wcslen(pFMT) == 0
		? L"{}"
		: pFMT,
		value));

	return 0;
}

short WINAPI DLLExport Action_Scroll_Disable(LPRDATA rdPtr, long param1, long param2) {
	if (rdPtr->bScroll) { rdPtr->bUpdateScroll = true; }
	rdPtr->bScroll = false;

	return 0;
}

short WINAPI DLLExport Action_Scroll_SetScrollCoef(LPRDATA rdPtr, long param1, long param2) {
	const auto scrollCoefX = GetFloatParam(rdPtr);
	const auto scrollCoefY = GetFloatParam(rdPtr);

	if (!rdPtr->bScroll) { rdPtr->bUpdateScroll = true; }

	rdPtr->bScroll = true;

	rdPtr->scrollCoefX = scrollCoefX;
	rdPtr->scrollCoefY = scrollCoefY;

	return 0;
}

// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport Expression_GetString(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->pStr->c_str());
}

long WINAPI DLLExport Expression_GetRowSpace(LPRDATA rdPtr,long param1) {
	return rdPtr->nRowSpace;
}

long WINAPI DLLExport Expression_GetColSpace(LPRDATA rdPtr, long param1) {
	return rdPtr->nColSpace;
}

long WINAPI DLLExport Expression_GetLastCharX(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).x;
}

long WINAPI DLLExport Expression_GetLastCharY(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).y;
}

long WINAPI DLLExport Expression_GetMaxWidth(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).maxWidth;
}

long WINAPI DLLExport Expression_GetTotalHeight(LPRDATA rdPtr, long param1) {
	return UpdateLastCharPos(rdPtr).totalHeight;
}

long WINAPI DLLExport Expression_GetHorizontalAlign(LPRDATA rdPtr, long param1) {
	if (rdPtr->dwAlignFlags & DT_CENTER) {
		return DT_CENTER;
	}
	if (rdPtr->dwAlignFlags & DT_RIGHT) {
		return DT_RIGHT;
	}

	return DT_LEFT;
}

long WINAPI DLLExport Expression_GetVerticalAlign(LPRDATA rdPtr, long param1) {
	if (rdPtr->dwAlignFlags & DT_VCENTER) {
		return DT_VCENTER;
	}
	if (rdPtr->dwAlignFlags & DT_BOTTOM) {
		return DT_BOTTOM;
	}

	return DT_TOP;
}

long WINAPI DLLExport Expression_GetRenderWidth(LPRDATA rdPtr, long param1) {
	return rdPtr->swidth;
}
long WINAPI DLLExport Expression_GetRenderHeight(LPRDATA rdPtr, long param1) {
	return rdPtr->sheight;
}

long WINAPI DLLExport Expression_GetHotSpotX(LPRDATA rdPtr, long param1) {
	int hotSpotX = rdPtr->hotSpotX;
	int hotSpotY = rdPtr->hotSpotY;

	UpdateHotSpot(rdPtr->hotSpotPos
		, rdPtr->rHo.hoImgWidth, rdPtr->rHo.hoImgHeight
		, hotSpotX, hotSpotY);

	return hotSpotX;
}
long WINAPI DLLExport Expression_GetHotSpotY(LPRDATA rdPtr, long param1) {
	int hotSpotX = rdPtr->hotSpotX;
	int hotSpotY = rdPtr->hotSpotY;

	UpdateHotSpot(rdPtr->hotSpotPos
		, rdPtr->rHo.hoImgWidth, rdPtr->rHo.hoImgHeight
		, hotSpotX, hotSpotY);

	return hotSpotY;
}

long WINAPI DLLExport Expression_GetXScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->xScale);
}
long WINAPI DLLExport Expression_GetYScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->yScale);
}

long WINAPI DLLExport Expression_GetAngle(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->angle);
}

long WINAPI DLLExport Expression_GetFontFamilyName(LPRDATA rdPtr, long param1) {
	std::wstring filePath = (LPCTSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	LPCTSTR Key = (LPCTSTR)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_STRING);

	size_t pos = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);

	bool bFromMem = !StrEmpty(Key);
	Encryption* E = nullptr;

	if (bFromMem) {
		E = new Encryption;
		E->GenerateKey(Key);

		E->OpenFile(filePath.c_str());
		E->Decrypt();
	}

	auto fontNames = bFromMem
		? GetFontNameFromFile((LPCWSTR)E->GetOutputData(), E->GetOutputDataLength())
		: GetFontNameFromFile(filePath.c_str());

	delete E;

	rdPtr->rHo.hoFlags |= HOF_STRING;

	try{
		auto& name = fontNames.at(pos);
		NewStr(*rdPtr->pExpRet, name);

		return (long)rdPtr->pExpRet->c_str();

	}catch(...) {
		return (long)Empty_Str;
	}
}

long WINAPI DLLExport Expression_GetCharX(LPRDATA rdPtr, long param1) {
	const size_t pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	UpdateLastCharPos(rdPtr);
	auto charPosX = rdPtr->pNeoStr->GetCharPos(pos).x;

	if (rdPtr->bScroll) {
		charPosX -= static_cast<NeoStr::BlitOptions*>(rdPtr->pBlitOptions)->scrollOffsetX;
	}
	
	return charPosX;
}

long WINAPI DLLExport Expression_GetCharY(LPRDATA rdPtr, long param1) {
	const size_t pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	UpdateLastCharPos(rdPtr);
	auto charPosY = rdPtr->pNeoStr->GetCharPos(pos).y;

	if (rdPtr->bScroll) {
		charPosY -= static_cast<NeoStr::BlitOptions*>(rdPtr->pBlitOptions)->scrollOffsetY;
	}

	return charPosY;
}

long WINAPI DLLExport Expression_GetIConOffsetX(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->iConOffsetX);
}

long WINAPI DLLExport Expression_GetIConOffsetY(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->iConOffsetY);
}

long WINAPI DLLExport Expression_GetIConScale(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->iConScale);
}

long WINAPI DLLExport Expression_GetIConResample(LPRDATA rdPtr, long param1) {
	return rdPtr->bIConResample;
}

long WINAPI DLLExport Expression_GetRemarkOffsetX(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->remarkOffsetX);
}

long WINAPI DLLExport Expression_GetRemarkOffsetY(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->remarkOffsetY);
}

long WINAPI DLLExport Expression_GetVerticalOffset(LPRDATA rdPtr, long param1) {
	return rdPtr->bVerticalAlignOffset;
}

long WINAPI DLLExport Expression_GetFilteredString(LPRDATA rdPtr, long param1) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	size_t flags = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG);

	NeoStr pFilter(0, 0, rdPtr->hFont
			, rdPtr->pData->pFontCache
			, rdPtr->pData->pCharSzCacheWithFont
			, rdPtr->pData->pRegexHandler
			, rdPtr->pData->pIConData
			, rdPtr->pData->pFontCollection
			, false);
	pFilter.GetFormat(pStr
		, flags == -1
		? rdPtr->filterFlags
		: flags);

	*rdPtr->pExpRet = pFilter.GetText() != nullptr ? pFilter.GetText() : Default_Str;

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->pExpRet->c_str());
}

long WINAPI DLLExport Expression_GetPaddingString(LPRDATA rdPtr, long param1) {
	const LPCWSTR pStr = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	const auto len = wcslen(pStr);

	if (len == 0) {
		//Setting the HOF_STRING flag lets MMF know that you are a string.
		rdPtr->rHo.hoFlags |= HOF_STRING;

		//This returns a pointer to the string for MMF.
		return (long)(Empty_Str);
	}

	*rdPtr->pExpRet = pStr;
	const auto pStart = rdPtr->pExpRet->data();
	
	bool bInCommand = false;
	size_t nestCount = 0;

	//https://zh.wikipedia.org/wiki/%E8%99%9A%E7%BC%BA%E5%8F%B7
	//const auto replaceChar = L'�';
	const auto replaceChar = L'□';

	for (size_t i = 0; i < len; i++) {
		const auto pCur = pStart + i;

		const auto curChar = pCur[0];
		const auto nextChar = pCur[1];

		// Escape
		if (curChar == L'\\' && nextChar == L'[') {
			i++;

			continue;
		}

		// Parse Format
		if (curChar == L'[') {
			if (bInCommand) { nestCount++; }
			else { bInCommand = true; }
		}

		if (bInCommand) {
			pCur[0] = replaceChar;
		}

		if (bInCommand && curChar == L']') {
			if (nestCount != 0) { nestCount--; }
			else { bInCommand = false; }
		}
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->pExpRet->c_str());
}

long WINAPI DLLExport Expression_GetNonCommandOffset(LPRDATA rdPtr, long param1) {
	const auto pStrSrc = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	size_t start = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG);

	const auto srcLen = wcslen(pStrSrc);

	auto GetNonCommandOffset = [] (LPCWSTR pSrc, size_t pSrcLen, size_t start)->size_t {
		if (start >= pSrcLen) { return 0; }

		const auto pStr = pSrc + start;
		const auto len = wcslen(pStr);

		if (len == 0) { return 0; }

		const auto curChar = pStr[0];
		const auto nextChar = pStr[1];

		if (curChar == L'\\' && nextChar == L'[') {
			return 1;
		}

		if (curChar == L'[') {
			size_t end = 0;
			size_t nestCount = 0;

			while ((pStr + end)[0] != L']' || nestCount != 0) {
				// protect for end
				if (end == len) {
					return len;
				}

				const auto pTest = pStr + end;

				// handle nest format contorl for params
				if ((pTest)[0] == L'\\' && (pTest + 1)[0] == L'[') {
					end += 2;
					continue;
				}

				if (end != 0 && (pTest)[0] == L'[') { nestCount++; }
				if (nestCount != 0 && (pTest)[0] == L']') { nestCount--; }

				end++;
			}			

			return end + 1;
		}

		// normal
		return 0;
	};

	size_t totalOffset = 0;

	for (;;) {
		const auto curOffset = GetNonCommandOffset(pStrSrc, srcLen, start);
		totalOffset += curOffset;

		if (curOffset != 0) {
			start += curOffset;
		}
		else {
			break;
		}
	}
	
	return (long)totalOffset;
}

long WINAPI DLLExport Expression_GetHashedString(LPRDATA rdPtr, long param1) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);

	auto hash = std::hash<std::wstring>{}(pStr);

	return (long)(hash);
}

long WINAPI DLLExport Expression_GetIConParamNum(LPRDATA rdPtr, long param1) {
	const auto pParams = static_cast<NeoStr::ControlParams*>(rdPtr->pIConParams);

	if (pParams == nullptr) {
		return -1;
	}

	return (long)(pParams->size());
}

long WINAPI DLLExport Expression_GetIConParamString(LPRDATA rdPtr, long param1) {
	const size_t pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	const auto pParams = static_cast<NeoStr::ControlParams*>(rdPtr->pIConParams);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(pParams != nullptr && pParams->size() > pos
		? [&]() {
			*rdPtr->pExpRet = (*pParams)[pos];
			return rdPtr->pExpRet->c_str();
		}()
		: Empty_Str);
}

long WINAPI DLLExport Expression_GetFilterFlag(LPRDATA rdPtr, long param1) {
	return (long)(rdPtr->filterFlags);
}

long WINAPI DLLExport Expression_GetRawStringByFilteredStringLength(LPRDATA rdPtr, long param1) {
	LPCWSTR pStr = (LPCWSTR)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_STRING);
	size_t filteredLength = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG);
	size_t flags = (size_t)CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_LONG);

	NeoStr pFilter(0, 0, rdPtr->hFont
			, rdPtr->pData->pFontCache
			, rdPtr->pData->pCharSzCacheWithFont
			, rdPtr->pData->pRegexHandler
			, rdPtr->pData->pIConData
			, rdPtr->pData->pFontCollection
			, false);

	try {		
		pFilter.GetFormat(pStr
			, flags == static_cast<size_t>(-1) ? rdPtr->filterFlags : flags
			, true
			, FORMAT_OPERATION_GetRawStringByFilteredStringLength
			, filteredLength);		
	}
	catch ([[maybe_unused]] NeoStrFormatException& e) {
		// not the flag we want, may hide other errors
		if (e.GetFlag() != FORMAT_OPERATION_GetRawStringByFilteredStringLength) {
			throw;
		}

		*rdPtr->pExpRet = pFilter.GetRawText() != nullptr
			? pFilter.GetRawText()
			: Default_Str;
	}

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->pExpRet->c_str());
}

long WINAPI DLLExport Expression_GetRenderOption_VisableRatio(LPRDATA rdPtr, long param1) {
	const auto pOpt = static_cast<NeoStr::RenderOptions*>(rdPtr->pRenderOptions);

	return ReturnFloat(pOpt->visibleRatio);
}

long WINAPI DLLExport Expression_GetTabSettings_TabSize(LPRDATA rdPtr, long param1) {
	return (long)(rdPtr->tabSize);
}

long WINAPI DLLExport Expression_GetTabSettings_TabEM(LPRDATA rdPtr, long param1) {
	return (long)(rdPtr->bTabEM);
}

long WINAPI DLLExport Expression_Format_GetFormatString(LPRDATA rdPtr, long param1) {	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return rdPtr->pFormatByVector != nullptr
		? [&] () {
			*rdPtr->pExpRet = rdPtr->pFormatByVector->GetFormatString();
			return reinterpret_cast<long>(rdPtr->pExpRet->c_str());
		}()
		: reinterpret_cast<long>(Empty_Str);
}

long WINAPI DLLExport Expression_GetTagParamNum(LPRDATA rdPtr, long param1) {
	const auto pParams = rdPtr->pTagCallbackParams;

	if (pParams == nullptr) {
		return -1;
	}

	return (long)(pParams->size());
}

long WINAPI DLLExport Expression_GetTagParamString(LPRDATA rdPtr, long param1) {
	const size_t pos = (size_t)CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);

	const auto pParams = rdPtr->pTagCallbackParams;

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(pParams != nullptr && pParams->size() > pos
		? [&] () {
			*rdPtr->pExpRet = (*pParams)[pos];
			return rdPtr->pExpRet->c_str();
		}()
			: Empty_Str);
}

long WINAPI DLLExport Expression_GetTagCallbackName(LPRDATA rdPtr, long param1) {	
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)rdPtr->pTagCallbackName->c_str();
}

long WINAPI DLLExport Expression_Scroll_GetCoefX(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->scrollCoefX);
}

long WINAPI DLLExport Expression_Scroll_GetCoefY(LPRDATA rdPtr, long param1) {
	return ReturnFloat(rdPtr->scrollCoefY);
}

long WINAPI DLLExport Expression_GetCurrentOverlapTriggerName(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->currentTriggerIndex != static_cast<size_t>(-1)
	? rdPtr->pNeoStr->GetCurrentTrigger(rdPtr->currentTriggerIndex).triggerName.c_str()
	: Empty_Str);
}

long WINAPI DLLExport Expression_GetCurrentOverlapTriggerContent(LPRDATA rdPtr, long param1) {
	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)(rdPtr->currentTriggerIndex != static_cast<size_t>(-1)
	? rdPtr->pNeoStr->GetCurrentTrigger(rdPtr->currentTriggerIndex).content.c_str()
	: Empty_Str);
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
			Condition_OnGetObjectICon,
			Condition_OnTagCallback,
			Condition_OnTagCallbackForward,
			Condition_PosOverlapTriggerRect,
			
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action_ChangeRenderSize,
			Action_ChangeString,
			Action_ChangeRowSpace,
			Action_ChangeColSpace,
			
			Action_ChangeHorizontalAlign,
			Action_ChangeVerticalAlign,
			
			Action_AppendString,
			Action_AppendStringNewLine,

			Action_SetHotSpot,
			Action_Zoom,
			Action_Stretch,
			Action_Rotate,

			Action_EmbedFont,

			Action_LinkActive,

			Action_SetIConOffset,
			Action_SetIConScale,
			Action_SetIConResample,

			Action_SetVerticalOffset,

			Action_LinkObject,
			Action_SetObjectKeyValue,

			Action_SetFilterFlag,

			Action_ForceRedraw,
			Action_ForceRedrawGlobalICon,
			Action_Render,

			Action_SetRenderOption_VisibleRatio,

			Action_ChangeTabSettings,

			Action_SetRemarkOffset,

			Action_Format_NewFormat,
			Action_Format_AddParamString,
			Action_Format_AddParamValue,

			Action_SetRenderOption_TagCallbackIndex,
			Action_SetRenderOption_TagCallbackIndexManaged,

			Action_Scroll_Disable,
			Action_Scroll_SetScrollCoef,

			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			Expression_GetString,
			Expression_GetRowSpace,
			Expression_GetColSpace,
			
			Expression_GetLastCharX,
			Expression_GetLastCharY,
			Expression_GetMaxWidth,

			Expression_GetHorizontalAlign,
			Expression_GetVerticalAlign,

			Expression_GetTotalHeight,
			
			Expression_GetRenderWidth,
			Expression_GetRenderHeight,
			Expression_GetHotSpotX,
			Expression_GetHotSpotY,
			Expression_GetXScale,
			Expression_GetYScale,
			Expression_GetAngle,

			Expression_GetFontFamilyName,

			Expression_GetCharX,
			Expression_GetCharY,
			
			Expression_GetIConOffsetX,
			Expression_GetIConOffsetY,
			Expression_GetIConScale,
			Expression_GetIConResample,

			Expression_GetVerticalOffset,

			Expression_GetFilteredString,
			Expression_GetPaddingString,
			Expression_GetNonCommandOffset,

			Expression_GetHashedString,
			Expression_GetIConParamNum,
			Expression_GetIConParamString,
			
			Expression_GetFilterFlag,

			Expression_GetRawStringByFilteredStringLength,

			Expression_GetRenderOption_VisableRatio,

			Expression_GetTabSettings_TabSize,
			Expression_GetTabSettings_TabEM,

			Expression_GetRemarkOffsetX,
			Expression_GetRemarkOffsetY,

			Expression_Format_GetFormatString,

			Expression_GetTagParamNum,
			Expression_GetTagParamString,
			Expression_GetTagCallbackName,

			Expression_Scroll_GetCoefX,
			Expression_Scroll_GetCoefY,

			Expression_GetCurrentOverlapTriggerName,
			Expression_GetCurrentOverlapTriggerContent,
			0
			};