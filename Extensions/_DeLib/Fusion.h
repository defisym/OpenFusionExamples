#pragma once

// ------------------------
// include
// ------------------------

#include	"ccxhdr.h"
#include	"Surface.h"

#include	"ImageFlt.h"
#include	"ImgFlt.h"
#include	"CfcFile.h"

#include	"d3d11surfinfo.h"
#include	"D3dSurfInfo.h"

#include	<map>
#include	<vector>
#include	<thread>
#include	<functional>

#ifdef _OPENMP
#include <omp.h>
#endif

// ------------------------
// forward declaration
// ------------------------

inline bool IsHWA(LPSURFACE Src);
inline void ConvertToHWATexture(LPRDATA rdPtr, LPSURFACE& Src);

inline void _SavetoClipBoard(LPSURFACE Src, bool release = false, HWND Handle = NULL);
inline void __SavetoClipBoard(LPSURFACE Src, HWND Handle = NULL, bool release = false);
inline void __SavetoFile(LPRDATA rdPtr, LPSURFACE Src, LPCWSTR FilePath, LPCWSTR DefaultFilterName = nullptr, bool release = false);

inline void ProcessBitmap(LPRDATA rdPtr, LPSURFACE pSf, const std::function<void(const LPSURFACE pBitmap)>& processor);

struct SfCoef {
	BYTE* pData = nullptr;
	int pitch = 0;
	int byte = 0;
	int sz = 0;

	BYTE* pAlphaData = nullptr;
	int alphaPitch = 0;
	int alphaByte = 0;
	int alphaSz = 0;
};

inline SfCoef GetSfCoef(LPSURFACE pSf);
inline void ReleaseSfCoef(LPSURFACE pSf, const SfCoef& coef);
inline BYTE* GetPixelAddress(int width, int height, int x, int y, const SfCoef& coef);
inline BYTE* GetAlphaPixelAddress(int width, int height, int x, int y, const SfCoef& coef);

inline void ProcessBitmap(LPSURFACE pSf, const std::function<void(const LPSURFACE pBitmap)>& processor);
inline void ProcessBitmap(LPRDATA rdPtr, LPSURFACE pSf, const std::function<void(const LPSURFACE pBitmap)>& processor);
inline void ProcessPixel(LPSURFACE pSf, const std::function<void(const SfCoef& coef)>& processor);
inline void IteratePixel(LPSURFACE pSf, const std::function<void(int, int, const SfCoef&, BYTE*, BYTE*)>& process);

// ------------------------
// definition
// ------------------------

// -------------
// return
// -------------

inline auto GetCurrentParamPointer(LPRDATA rdPtr) {
	const auto rhPtr = rdPtr->rHo.hoAdRunHeader;
	const auto pResults = rhPtr->rh4.rh4Results;
	const auto pPos = rhPtr->rh4.rh4PosPile;

	return pResults[pPos + 1];
}

template<typename T>
concept FusionReturn = std::is_same_v<T, long>
|| std::is_same_v<T, LPCWSTR>
|| std::is_same_v<T, float>;

// ReSharper disable once CppNotAllPathsReturnValue
template<FusionReturn T>
inline T GetCurrentParam(LPRDATA rdPtr) {
	const auto pCValue = GetCurrentParamPointer(rdPtr);

	if constexpr (std::is_same_v<T, long>) {
		if (pCValue->m_type == TYPE_LONG) {
			return pCValue->m_long;
		}

		if (pCValue->m_type == TYPE_FLOAT) {
			return static_cast<long>(pCValue->m_double);
		}

		return 0;
	}

	if constexpr (std::is_same_v<T, float>) {
		if (pCValue->m_type == TYPE_LONG) {
			return static_cast<float>(pCValue->m_long);
		}

		if (pCValue->m_type == TYPE_FLOAT) {
			return static_cast<float>(pCValue->m_double);
		}

		return 0;
	}

	if constexpr (std::is_same_v<T, LPCWSTR>) {
		if(pCValue->m_type== TYPE_STRING) {
			return pCValue->m_pString;
		}

		return Default_Str;
	}
}


//Return float
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

inline long ReturnFloat(LPRDATA rdPtr, double Val) {
	return ReturnFloat(rdPtr, (float)Val);
}

#define ReturnFloat(Val) ReturnFloat(rdPtr, Val)

inline long ReturnString(LPRDATA rdPtr, const std::wstring& str) {
	auto sz = (str.size() + 1) * sizeof(wchar_t);
	//auto pStr = (wchar_t*)_mvCalloc(sz * sizeof(wchar_t));
	auto pStr = (wchar_t*)callRunTimeFunction(rdPtr, RFUNCTION_GETSTRINGSPACE_EX, 0, sz);
	
	memset(pStr, 0, sz);
	memcpy(pStr, str.c_str(), sz);

	//Setting the HOF_STRING flag lets MMF know that you are a string.
	rdPtr->rHo.hoFlags |= HOF_STRING;

	//This returns a pointer to the string for MMF.
	return (long)pStr;
}

#define ReturnString(Str) ReturnString(rdPtr, Str)

// -------------
// object
// -------------

inline auto GetObjectName(LPRDATA rdPtr) {
	return rdPtr->rHo.hoOiList->oilName;
}

// -------------
// animation
// -------------

// Iterator
template<typename Child>
inline void IterateValidItem(void* pParent, short* pArr,unsigned short sz, const std::function<void(Child*)>& cb) {
	for (decltype(sz) i = 0; i < sz; i++) {
		if (pArr[i] > 0) {
			const auto pChild = reinterpret_cast<Child*>(static_cast<byte*>(pParent) + pArr[i]);
			cb(pChild);
		}
	}
}

//Check if object has animation
inline bool ObjectHasAnimation(LPRO object) {
	if (object == nullptr) {
		return false;
	}

	return object->roHo.hoOEFlags & OEFLAG_ANIMATIONS;
}

inline bool ObjectHasAnimation(LPRDATA rdPtr, int Fixed) {
	return ObjectHasAnimation(LproFromFixed(rdPtr, Fixed));
}

//Check if object has animation ID
inline const AnimHeader* GetObjectAnimationHeader(LPRO object) {
	if (!ObjectHasAnimation(object)) {
		return nullptr;
	}

	const auto pOC = object->roHo.hoCommon;
	const auto offset = pOC->ocAnimations;
	const auto pAH = reinterpret_cast<AnimHeader*>(reinterpret_cast<byte*>(pOC) + offset);

	return pAH;
}

inline const AnimHeader* GetObjectAnimationHeader(LPRDATA rdPtr, int Fixed) {
	return GetObjectAnimationHeader(LproFromFixed(rdPtr, Fixed));
}

//IterateAnimation(const_cast<AnimHeader*>(pAH), [] (Anim* pA) {
//});
inline void IterateAnimation(AnimHeader* pAH, const std::function<void(Anim*)>& cb) {
	//for (decltype(pAH->ahAnimMax) i = 0; i < pAH->ahAnimMax; i++) {
	//	if (pAH->ahOffsetToAnim[i] > 0) {
	//		const auto pA = reinterpret_cast<Anim*>(reinterpret_cast<byte*>(pAH) + pAH->ahOffsetToAnim[i]);
	//		cb(pA);
	//	}
	//}

	IterateValidItem(pAH, pAH->ahOffsetToAnim, pAH->ahAnimMax, cb);
}

inline bool ObjectHasAnimationID(const AnimHeader* pAH, size_t id) {
	if (pAH == nullptr) {
		return false;
	}

	if (id > pAH->ahAnimMax - 1u) {
		return false;
	}

	return pAH->ahOffsetToAnim[id] > 0;
}

inline bool ObjectHasAnimationID(LPRO object, size_t id) {
	const auto pAH = GetObjectAnimationHeader(object);

	return ObjectHasAnimationID(pAH, id);
}

inline bool ObjectHasAnimationID(LPRDATA rdPtr, int Fixed, size_t id) {
	return ObjectHasAnimationID(LproFromFixed(rdPtr, Fixed), id);
}

//Get object's display animation id
inline size_t DisplayAnimationID(LPRO object) {
	const auto pAH = GetObjectAnimationHeader(object);

	if (pAH == nullptr) {
		return -1;
	}

	const auto pA = &object->roa;
	const auto givenID = pA->raAnimOn;

	if (ObjectHasAnimationID(object, givenID)) {
		return givenID;
	}

	for (auto i = 0; i < pAH->ahAnimMax; i++) {
		if(ObjectHasAnimationID(object, i)) {
			return i;
		}
	}

	// shouldn't run to here as fusion guaranteed that object should have animation
	return -1;
}

inline size_t DisplayAnimationID(LPRDATA rdPtr, int Fixed) {
	return DisplayAnimationID(LproFromFixed(rdPtr, Fixed));
}

// -------------
// direction
// -------------

inline void IterateDirection(Anim* pA, const std::function<void(AnimDirection*)>& cb) {
	//for (int j = 0; j < DIRID_MAX; j++) {
	//	if (pA->anOffsetToDir[j] > 0) {
	//		const auto pD = reinterpret_cast<AnimDirection*>(reinterpret_cast<byte*>(pA) + pA->anOffsetToDir[j]);
	//	}
	//}

	IterateValidItem(pA, pA->anOffsetToDir, DIRID_MAX, cb);
}

//Check if a dir has frame
inline bool DirHasFrame(LPRO object, size_t Dir) {
	if (!ObjectHasAnimation(object)) {
		return false;
	}

	Dir = (std::max)(0u, (std::min)(DIRID_MAX - 1u, Dir));

	return object->roa.raAnimOffset->anOffsetToDir[Dir] > 0;
}

inline bool DirHasFrame(LPRDATA rdPtr, int Fixed, size_t Dir) {
	return DirHasFrame(LproFromFixed(rdPtr, Fixed), Dir);
}

//Get object's display animation direction
inline size_t DisplayAnimationDirection(LPRO object) {
	if (!ObjectHasAnimation(object)) {
		return -1;
	}

	//Mechanism
	//fusion decides to display a replaced animation when current direction doesn't have one by following rules below:
	//	- Display the closest direction(0 is treated as 32)
	//	- If both former and later have the same distance, display the one backward to the direction of direction change(if only 0 and 8 have animation, switch from 3->4, display 0, switch from 5->4, display 8)

	size_t former = 0;
	size_t later = 0;

	const size_t curDir = object->roa.raAnimDir;
	const size_t prevDir = object->roa.raAnimPreviousDir;

	bool clockwise;

	if (prevDir == 0) {
		if (curDir <= 16) {
			clockwise = false;
		}
		else {
			clockwise = true;
		}
	}
	else {
		clockwise = prevDir >= curDir;
	}

	//former
	for (size_t pos = curDir; pos != (size_t)(-1); pos--) {
		if (DirHasFrame(object, pos)) {
			former = pos;
			break;
		}
	}

	//later
	for (size_t pos = curDir; pos <= DIRID_MAX; pos++) {
		if (DirHasFrame(object, pos % 32)) {
			later = pos;
			break;
		}
	}

	if ((curDir - former) == (later - curDir)) {
		return !clockwise ? former : later % 32;
	}
	else {
		return (curDir - former) <= (later - curDir) ? former : later % 32;
	}
}

inline size_t DisplayAnimationDirection(LPRDATA rdPtr, int Fixed) {
	return DisplayAnimationDirection(LproFromFixed(rdPtr, Fixed));
}

// -------------
// frame
// -------------

inline int GetCurrentFrameCount(LPRO object) {
	if(!ObjectHasAnimation(object)) {
		return -1;
	}

	return object->roa.raAnimDirOffset->adNumberOfFrame;
}

inline int GetCurrentFrameCount(LPRDATA rdPtr, int Fixed) {
	return GetCurrentFrameCount(LproFromFixed(rdPtr, Fixed));
}

inline int GetAnimDirFrameCount(LPRO object, size_t id, size_t Dir) {
	const auto pAH = GetObjectAnimationHeader(object);

	if (!ObjectHasAnimationID(pAH, id)) {
		return -1;
	}

	const auto pA = reinterpret_cast<const Anim*>(reinterpret_cast<const byte*>(pAH) + pAH->ahOffsetToAnim[id]);

	if (Dir >= DIRID_MAX) {
		return -1;
	}

	if (pA->anOffsetToDir[Dir] < 0) {
		return -1;
	}

	const auto pD = reinterpret_cast<const AnimDirection*>(reinterpret_cast<const byte*>(pA) + pA->anOffsetToDir[Dir]);

	return pD->adNumberOfFrame;
}

inline int GetAnimDirFrameCount(LPRDATA rdPtr, int Fixed, size_t id, size_t Dir) {
	return GetAnimDirFrameCount(LproFromFixed(rdPtr, Fixed), id, Dir);
}

// -------------
// surface
// -------------

//Surface
constexpr auto Dir_X = false;
constexpr auto Dir_Y = true;

constexpr auto Do_Normal = false;
constexpr auto Do_Alpha = true;

constexpr auto Fast = false;
constexpr auto HighQuality = true;

//Pixel
struct RGBA {
	double r;
	double g;
	double b;
	double a;
};

inline RGBA Range(RGBA A) {
	A.r = Range(A.r, 0.0, 255.0);
	A.g = Range(A.g, 0.0, 255.0);
	A.b = Range(A.b, 0.0, 255.0);
	A.a = Range(A.a, 0.0, 255.0);

	return A;
}

inline RGBA operator +(RGBA A, const RGBA& B) {
	A.r += B.r;
	A.g += B.g;
	A.b += B.b;
	A.a += B.a;

	return A;
}

inline RGBA operator -(RGBA A, const RGBA& B) {
	A.r -= B.r;
	A.g -= B.g;
	A.b -= B.b;
	A.a -= B.a;

	return A;
}

inline RGBA operator +=(const RGBA& A, const RGBA& B) {
	return A + B;
}

inline RGBA operator -=(const RGBA& A, const RGBA& B) {
	return A - B;
}

inline RGBA operator *(RGBA A, double B) {
	A.r = A.r * B;
	A.g = A.g * B;
	A.b = A.b * B;
	A.a = A.a * B;

	return A;
}

inline RGBA operator *(double B, const RGBA& A) {
	return A * B;
}

inline RGBA operator *(const RGBA& A, int B) {
	return A * static_cast<double>(B);
}

inline RGBA operator *(int B, const RGBA& A) {
	return A * B;
}

inline RGBA operator /(RGBA A, double B) {
	A.r = A.r / B;
	A.g = A.g / B;
	A.b = A.b / B;
	A.a = A.a / B;

	return A;
}

inline RGBA operator /(double B, const RGBA& A) {
	return A / B;
}

inline RGBA operator /(const RGBA& A, int B) {
	return A / static_cast<double>(B);
}

inline RGBA operator /(int B, const RGBA& A) {
	return A / B;
}

inline RGBA operator >>(RGBA A, int B) {
	A.r = static_cast<double>(static_cast<int>(A.r) >> B);
	A.g = static_cast<double>(static_cast<int>(A.g) >> B);
	A.b = static_cast<double>(static_cast<int>(A.b) >> B);
	A.a = static_cast<double>(static_cast<int>(A.a) >> B);

	return A;
}

//Free collision mask
inline void FreeColMask(LPSMASK& pColMask) {
	if (pColMask != nullptr) {
		free(pColMask);
		pColMask = nullptr;
	}
}

//Create surface
inline LPSURFACE CreateHWASurface(int depth, int width, int height
	, int type = ST_HWA_ROMTEXTURE, int driver = SD_D3D11) {
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, depth, type, driver);

	const auto hwa = new cSurface;
	hwa->Create(width, height, proto);

	return hwa;
}

inline void CreateHWASurface(LPSURFACE pSf, int depth, int width, int height
	, int type = ST_HWA_ROMTEXTURE, int driver = SD_D3D11) {
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, depth, type, driver);

	pSf->Create(width, height, proto);
}

inline LPSURFACE CreateHWASurface(LPRDATA rdPtr, int depth, int width, int height
	, int type = ST_HWA_ROMTEXTURE) {
	LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
	int sfDrv = wSurf->GetDriver();

	return CreateHWASurface(depth, width, height, type, sfDrv);
}

inline void CreateHWASurface(LPRDATA rdPtr, LPSURFACE pSf, int depth, int width, int height
	, int type = ST_HWA_ROMTEXTURE) {
	LPSURFACE wSurf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
	int sfDrv = wSurf->GetDriver();

	CreateHWASurface(pSf, depth, width, height, type, sfDrv);
}

inline LPSURFACE CreateSurface(int depth, int width, int height) {
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, depth, ST_MEMORYWITHDC, SD_DIB);

	const auto sur = new cSurface;
	sur->Create(width, height, proto);

	return sur;
}

inline void CreateSurface(LPSURFACE pSf, int depth, int width, int height) {
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, depth, ST_MEMORYWITHDC, SD_DIB);

	pSf->Create(width, height, proto);
}

// failed to load, display blank
inline LPSURFACE CreateBlankSurface() {
	auto pSf = CreateSurface(24, 4, 4);
	pSf->CreateAlpha();

	return pSf;
}

// failed to load, display blank
inline void CreateBlankSurface(LPSURFACE Src) {
	CreateSurface(Src, 24, 4, 4);
	Src->CreateAlpha();
}

[[deprecated ("Clone won't copy alpha of HWA surface")]]
inline LPSURFACE CreateCloneSurface(LPSURFACE Src) {
	const auto sur = new cSurface;
	sur->Clone(*Src);

	return sur;
}

inline LPSURFACE CreateCloneSurface(LPRDATA rdPtr, LPSURFACE pSrc) {
	// clone doesn't handle hwa alpha & rtt flip properly
	// so do it by hand
	if (IsHWA(pSrc) && pSrc->HasAlpha()) {
		auto pSf = CreateSurface(pSrc->GetDepth(), pSrc->GetWidth(), pSrc->GetHeight());
		pSf->CreateAlpha();
		pSrc->Blit(*pSf);
		ConvertToHWATexture(rdPtr, pSf);

		return pSf;
	}
	else {
		const auto pSf = new cSurface;
		pSf->Clone(*pSrc);

		return pSf;
	}
}

//Get info
inline D3D11SURFINFO GetSurfaceInfo(LPSURFACE pSf) {
	D3D11SURFINFO info = { };
	info.m_lSize= sizeof(D3D11SURFINFO);

	auto ret = pSf->GetDriverInfo(&info);

	return info;
}

inline auto GetD3DInfo(LPRDATA rdPtr) {
	auto pSf = WinGetSurface((int)rdPtr->rHo.hoAdRunHeader->rhIdEditWin);
	auto info = GetSurfaceInfo(pSf);

	return info;
}

inline auto GetD3DDevice(LPRDATA rdPtr) {
	return GetD3DInfo(rdPtr).m_pD3D11Device;
}

//Convert to HWA
inline bool IsHWA(LPSURFACE Src) {
	return Src->GetType() >= ST_HWA_SCREEN;
}

inline bool IsOpaque(LPSURFACE pSf) {
	if(!pSf->HasAlpha()) { return true; }

	bool bRet = true;

	ProcessBitmap(pSf, [&](const LPSURFACE pBitmap) {
		auto coef = GetSfCoef(pSf);
		if (coef.pData == nullptr || coef.pAlphaData == nullptr) {		
			return;
		}

		for (int i = 0; i < coef.alphaSz; i++) {
			if (coef.pAlphaData[i] != 255) {
				bRet = false;

				break;
			}
		}

		ReleaseSfCoef(pSf, coef);
		});

	return bRet;
}

inline bool IsTransparent(LPSURFACE pSf) {
	bool result = false;

	if (!IsHWA(pSf)) {
		result = pSf->IsTransparent();
	}
	else {
		auto pTempMemSf = CreateSurface(pSf->GetDepth()
			, pSf->GetWidth(), pSf->GetHeight());

		pSf->Blit(*pTempMemSf);

		result = pTempMemSf->IsTransparent();

		delete pTempMemSf;
	}

	return result;
}

// not updated yet
constexpr BOOL transpTBD = -1;

inline BOOL GetTransparent(LPSURFACE pSf) {
	return IsHWA(pSf) ? transpTBD : pSf->IsTransparent();
}

inline LPSURFACE ConvertHWATarget(LPRDATA rdPtr, LPSURFACE Src) {
	return IsHWA(Src) 
	? Src 
	: CreateHWASurface(rdPtr, Src->GetDepth(), Src->GetWidth(), Src->GetHeight(), ST_HWA_RTTEXTURE);
}

inline void ConvertToHWATarget(LPRDATA rdPtr, LPSURFACE& Src) {
	if (IsHWA(Src)) {
		return;
	}

	auto pBitmap = Src;
	Src = ConvertHWATarget(rdPtr, Src);
	delete pBitmap;
}

#define _NO_REF

#ifdef _NO_REF
#else
#endif // _NO_REF

inline LPSURFACE ConvertBitmap(LPSURFACE pOldSf) {
	if (!IsHWA(pOldSf)) {
		return pOldSf;
	}

	LPSURFACE pMemSf = CreateSurface(pOldSf->GetDepth(), pOldSf->GetWidth(), pOldSf->GetHeight());

	if (pOldSf->HasAlpha()) {
		pMemSf->CreateAlpha();
	}

	pOldSf->Blit(*pMemSf);

	return pMemSf;
}

inline LPSURFACE ConvertBitmap(LPRDATA rdPtr, LPSURFACE pOldSf) {
	return ConvertBitmap(pOldSf);
}

inline LPSURFACE ConvertHWATexture(LPRDATA rdPtr, LPSURFACE Src) {
	if (IsHWA(Src)) {
		return Src;
	}

	cSurface* hwa = CreateHWASurface(rdPtr, Src->GetDepth(), Src->GetWidth(), Src->GetHeight(), ST_HWA_ROMTEXTURE);

	if (Src->HasAlpha()) {
		hwa->CreateAlpha();
	}

	Src->Blit(*hwa);

	return hwa;
}

inline void ConvertToBitmap(LPRDATA rdPtr, LPSURFACE& pOldSf) {
	if (!IsHWA(pOldSf)) {
		return;
	}

	auto pHWA = pOldSf;
	pOldSf = ConvertBitmap(rdPtr, pOldSf);	
	delete pHWA;
}

inline void ConvertToHWATexture(LPRDATA rdPtr, LPSURFACE& Src) {
	if (IsHWA(Src)) {
		return;
	}

	auto pBitmap = Src;
	Src = ConvertHWATexture(rdPtr, Src);
	delete pBitmap;
}

inline void ConvertToBitmap_NRef(LPRDATA rdPtr, const LPSURFACE pOldSf) {
	if (!IsHWA(pOldSf)) {
		return;
	}

	auto pBitmap = ConvertBitmap(rdPtr, pOldSf);
	CreateSurface(pOldSf, pOldSf->GetDepth(), pOldSf->GetWidth(), pOldSf->GetHeight());

	pBitmap->Blit(*pOldSf);

	delete pBitmap;
}

inline void ConvertToHWATexture_NRef(LPRDATA rdPtr,const LPSURFACE pOldSf) {
	if (IsHWA(pOldSf)) {
		return;
	}

	auto pHWA = ConvertHWATexture(rdPtr, pOldSf);
	CreateHWASurface(rdPtr, pOldSf, pOldSf->GetDepth(), pOldSf->GetWidth(), pOldSf->GetHeight(), ST_HWA_ROMTEXTURE);

	pHWA->Blit(*pOldSf);

	delete pHWA;
}

inline DWORD GetFlag(LPSURFACE Src, bool HighQuality) {
	DWORD flag = 0;

	if (HighQuality) {
		flag = flag | STRF_RESAMPLE;
	}
	if (Src->HasAlpha()) {
		flag = flag | STRF_COPYALPHA;
	}

	return flag;
}

//Stretch Surface
inline void Stretch(LPSURFACE Src, LPSURFACE Des, bool HighQuality) {	
	auto ret = Src->Stretch(*Des, 0, 0, Des->GetWidth(), Des->GetHeight(), BMODE_OPAQUE, BOP_COPY, 0, GetFlag(Src, HighQuality));

	return;
}

inline bool Offset(LPSURFACE Src, LPSURFACE Des,
	int X, int Y, bool Wrap = true) {
	if (X == 0 && Y == 0) {
		return false;
	}

	const auto bHWA = IsHWA(Src);

	if (bHWA) {
		Des->BeginRendering(TRUE, 0);
	}

	POINT hotSpot = { 0,0 };
	
	const auto width = Src->GetWidth();
	const auto height = Src->GetHeight();

	auto BlitBitmap = [&] (int x, int y) {
		//Src->Blit(*Des, x, y, BMODE_OPAQUE, BOP_COPY, 0, GetFlag(Src, HighQuality));
		Src->Blit(*Des, x, y, BMODE_TRANSP, BOP_COPY);
	};

	auto BlitHWA = [&] (int x, int y) {
		Src->BlitEx(*Des, (float)x, (float)y,
		1.0, 1.0, 0, 0,
		width, height, &hotSpot, (float)0,
		BMODE_TRANSP,
		BOP_COPY,0, STRF_RESAMPLE | STRF_COPYALPHA);
	};

	std::function<void(int, int)> Blit = nullptr;

	if (bHWA) {
		Blit = BlitHWA;
	}
	else {
		Blit = BlitBitmap;
	}

	Blit(X, Y);

	if (Wrap) {
		const int XWrap = X > 0 ? X - width : X + width;
		const int YWrap = Y > 0 ? Y - height : Y + height;

		Blit(X, YWrap);
		Blit(XWrap, Y);
		Blit(XWrap, YWrap);
	}

	if (bHWA) {
		Des->EndRendering();
	}
	
	return true;
}

//Get Ext's FilterName

//指针Map需要使用自定义比较
struct LPWSTR_Compare
{
	bool operator()(LPCWSTR l, LPCWSTR r)  const noexcept { return (wcscmp(l, r) < 0); };
};

inline LPCWSTR GetFilterName(LPCWSTR Name, LPCWSTR DefaultFilterName = nullptr) {
	//To Lower
	auto LowerWStr = [](LPWSTR Str)->LPWSTR {
		int i = 0;
		while (Str[i]) {
			Str[i] = towlower(Str[i]);
			i++;
		}
		return Str;
	};

	//常量会被放入全局变量区
	const std::map<LPCWSTR, LPCWSTR, LPWSTR_Compare> ExtList{
		{_T(".png"),_T("Portable Network Graphics")},
		{_T(".tga"),_T("Targa Bitmap")},
		{_T(".bmp"),_T("Windows Bitmap")},
		{_T(".jpg"),_T("JPEG")},
		{_T(".flc"),_T("Autodesk FLIC")},
		{_T(".gif"),_T("Compuserve Bitmap")},
		{_T(".avi"),_T("Video For Windows")},
		{_T(".pcx"),_T("PaintBrush")}
	};

	WCHAR* Ext = new WCHAR[wcslen(Name) + 1];
	wcscpy_s(Ext, wcslen(Name) + 1, Name);
	LowerWStr(Ext);

	auto it = ExtList.find(Ext);
	delete[] Ext;

	//default format is JPEG
	//return _T("JPEG");
	return (it != ExtList.end()) ? it->second : ((DefaultFilterName == nullptr) ? _T("JPEG") : DefaultFilterName);
};

//Get Filter ID By Ext
inline DWORD GetFilterIDByExt(LPRDATA rdPtr, LPCTSTR Ext, LPCWSTR DefaultFilterName = nullptr) {
	//Surface
	CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
	CImageFilter    pFilter(pImgMgr);

	auto GetFilterID = [pImgMgr](LPCWSTR Ext)->DWORD {
		for (int i = 0; i < pImgMgr->GetFilterCount(); i++) {
			auto Exts = pImgMgr->GetFilterExtsW(i);
			if (Exts == nullptr) {
				return 0;
			}

			for (size_t j = 0; Exts[j] != nullptr; j++) {
				if (_wcsicmp(Exts[j], Ext) == 0) {
					return pImgMgr->GetFilterID(i);
				}
			}
		}

		return 0;
	};

	auto Result = GetFilterID(Ext);

	return Result != 0 ? Result : GetFilterID(L"JPG");
}

//Get Filter ID By File Name
inline DWORD GetFilterIDByFileName(LPRDATA rdPtr, LPCTSTR FilePath, LPCWSTR DefaultFilterName = nullptr) {
	//Get Ext from FileName
	WCHAR* Ext = new WCHAR[FILENAME_MAX];
	_wsplitpath_s(FilePath, NULL, 0, NULL, 0, NULL, 0, Ext, FILENAME_MAX);

	//DWORD FilterID = GetFilterIDByExt(rdPtr, Ext);
	DWORD FilterID = GetFilterIDByExt(rdPtr, Ext + 1);

	delete[] Ext;

	return FilterID;
}

//Save && Load

//Save to Clipboard
inline void _SavetoClipBoard(LPSURFACE Src, bool release, HWND Handle) {
	if (!Src->IsValid()) {
		return;
	}

	OpenClipboard(Handle);
	EmptyClipboard();

	HGLOBAL cb = GlobalAlloc(GMEM_MOVEABLE, Src->GetDIBSize());
	
	if (cb == NULL) {
		return;
	}

	BITMAPINFO* OutPut = (BITMAPINFO*)GlobalLock(cb);

	Src->SaveImage(OutPut, (BYTE*)(OutPut + 1) - 4);
	SetClipboardData(CF_DIB, OutPut);

	GlobalUnlock(cb);
	CloseClipboard();

	if (release) {
		delete Src;
	}
}

inline void __SavetoClipBoard(LPSURFACE Src, HWND Handle, bool release) {
	_SavetoClipBoard(Src, release, Handle);
}

//Save to File
inline void _SavetoFile(LPSURFACE Src, LPCWSTR FilePath, LPRDATA rdPtr, bool release = false, LPCWSTR DefaultFilterName = nullptr) {
	if (Src == nullptr || !Src->IsValid()) {
		return;
	}

	BOOL result = FALSE;
	
	ProcessBitmap(rdPtr, Src, [&](const LPSURFACE pBitmap) {
		CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
		result = ExportImage(pImgMgr, FilePath, pBitmap, GetFilterIDByFileName(rdPtr, FilePath, DefaultFilterName));
		});	

	if (release) {
		delete Src;
	}
}

inline void __SavetoFile(LPRDATA rdPtr, LPSURFACE Src, LPCWSTR FilePath, LPCWSTR DefaultFilterName, bool release) {
	_SavetoFile(Src, FilePath, rdPtr, release, DefaultFilterName);
}

//Load From ClipBoard
inline void _LoadFromClipBoard(LPSURFACE Src, int width, int height, bool NoStretch, bool HighQuality, HWND Handle = NULL) {
	if (IsClipboardFormatAvailable(CF_DIB) && OpenClipboard(Handle)) {
		//BMP
		HANDLE handle = GetClipboardData(CF_DIB);
		BITMAPINFO* bmp = (BITMAPINFO*)GlobalLock(handle);

		if (bmp == NULL) {
			return;
		}

		if (NoStretch) {
			delete Src;
			Src = CreateSurface(24, bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight);
			Src->LoadImage(bmp, GetDIBBitmap(bmp));
		}
		else {
			LPSURFACE img;
			img = CreateSurface(24, bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight);
			img->LoadImage(bmp, GetDIBBitmap(bmp));

			delete Src;
			Src = CreateSurface(24, width, height);

			Stretch(img, Src, HighQuality);

			delete img;
		}
		
		GlobalUnlock(handle);
		CloseClipboard();
	}
}

//Load From File
inline void _ForceAddAlpha(LPSURFACE Src, BYTE coef = 255) {
	auto pitch = Src->GetWidth();
	auto size = pitch * Src->GetHeight();

	BYTE* pAlpha = new BYTE[size];
	memset(pAlpha, coef, size);

	Src->SetAlpha(pAlpha, pitch);

	delete[] pAlpha;
}

inline void _AddAlpha(LPSURFACE Src, BYTE coef = 255) {
	if (!Src->HasAlpha()) {
		_ForceAddAlpha(Src);
	}
}

// return false if created the blank surface
#ifdef _NO_REF
inline bool _LoadCore(LPRDATA rdPtr, const LPSURFACE Src
	, int width, int height, bool NoStretch, bool HighQuality
	, std::function<bool(CImageFilterMgr*, const LPSURFACE)> load) {
#else
inline bool _LoadCore(LPRDATA rdPtr, LPSURFACE& Src
	, int width, int height, bool NoStretch, bool HighQuality
	, std::function<bool(CImageFilterMgr*, LPSURFACE&)> load) {
#endif // _NO_REF
	//MGR
	CImageFilterMgr* pImgMgr = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv->mvImgFilterMgr;
	CImageFilter    pFilter(pImgMgr);

	bool ret = true;

	if (NoStretch) {
		if (!load(pImgMgr, Src)) {
			CreateBlankSurface(Src);
			ret = false;
		}
	}
	else {
		LPSURFACE pImg = new cSurface;

		if (load(pImgMgr, pImg)) {
#ifdef _NO_REF
			CreateSurface(Src, pImg->GetDepth(), width, height);
#else
			delete Src;
			Src = CreateSurface(pImg->GetDepth(), width, height);
#endif // _NO_REF
			Stretch(pImg, Src, HighQuality);
		}
		else {
			CreateBlankSurface(Src);
			ret = false;
		}

		delete pImg;
	}

	return ret;
}

#ifdef _NO_REF
inline bool _LoadFromFile(const LPSURFACE Src, LPCTSTR FilePath, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	return _LoadCore(rdPtr, Src, width, height, NoStretch, HighQuality, [FilePath](CImageFilterMgr* pImgMgr, const LPSURFACE pSf) {
		return ImportImage(pImgMgr, FilePath, pSf, 0, 0);
		});
}

inline bool _LoadFromMemFile(const LPSURFACE Src, LPBYTE pData, DWORD dataSz, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	CInputMemFile MemFile;
	MemFile.Create(pData, dataSz);

	return _LoadCore(rdPtr, Src, width, height, NoStretch, HighQuality, [&MemFile](CImageFilterMgr* pImgMgr, const LPSURFACE pSf) {
		return ImportImageFromInputFile(pImgMgr, &MemFile, pSf, 0, 0);
		});
}
#else
inline bool _LoadFromFile(LPSURFACE& Src, LPCTSTR FilePath, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	return _LoadCore(rdPtr, Src, width, height, NoStretch, HighQuality, [FilePath](CImageFilterMgr* pImgMgr, LPSURFACE& pSf) {
		return ImportImage(pImgMgr, FilePath, pSf, 0, 0);
		});
}

inline bool _LoadFromMemFile(LPSURFACE& Src, LPBYTE pData, DWORD dataSz, LPRDATA rdPtr, int width, int height, bool NoStretch, bool HighQuality) {
	CInputMemFile MemFile;
	MemFile.Create(pData, dataSz);

	return _LoadCore(rdPtr, Src, width, height, NoStretch, HighQuality, [&MemFile](CImageFilterMgr* pImgMgr, LPSURFACE& pSf) {
		return ImportImageFromInputFile(pImgMgr, &MemFile, pSf, 0, 0);
		});
}
#endif // _NO_REF

// create a temp bitmap pSf if needed
// don't use the bitmap in callback out of this function
inline void ProcessBitmap(LPSURFACE pSf, const std::function<void(const LPSURFACE pBitmap)>& processor) {
	auto bHWA = IsHWA(pSf);
	auto pBitmap = pSf;

	if (bHWA) {
		pBitmap = ConvertBitmap(pSf);
	}

	processor(pBitmap);

	if (bHWA) {
		delete pBitmap;
	}
}

inline void ProcessBitmap(LPRDATA rdPtr, LPSURFACE pSf, const std::function<void(const LPSURFACE pBitmap)>& processor) {
	ProcessBitmap(pSf, processor);
}

// create a temp HWA pSf if needed
inline void ProcessHWA(LPRDATA rdPtr, LPSURFACE pSf, const std::function<void(const LPSURFACE pHWA)>& processor) {
	auto bHWA = IsHWA(pSf);
	auto pHWA = pSf;

	if (!bHWA) {
		pHWA = ConvertHWATexture(rdPtr, pSf);
	}

	processor(pHWA);

	if (!bHWA) {
		delete pHWA;
	}
}

//Get Valid Scale
inline void GetValidScale(float* scale) {
	*scale = (std::max)(1.0f, *scale);
	return;
}

//Get Max supported thread of current CPU
inline void GetMaximumDivide(int* divide) {
	int Max = std::thread::hardware_concurrency();

	if (*divide == -1) {
		*divide = Max;
	}
	else {
		*divide = (std::max)(1, (std::min)(*divide, Max));
	}
	return;
}

//Stack Blur
#define STACK_BLUR_ALPHA

#ifdef _NO_REF
inline void StackBlur(const LPSURFACE pSrc, int radius, float scale, int divide) {
#else
inline void StackBlur(LPSURFACE& pSrc, int radius, float scale, int divide) {
#endif // _NO_REF
	//获取参数
	constexpr auto SB_MIN_RADIUS = 0;
	constexpr auto SB_MAX_RADIUS = 254;

	radius = (std::min)(SB_MAX_RADIUS, (std::max)(SB_MIN_RADIUS, radius));

	GetValidScale(&scale);
	GetMaximumDivide(&divide);

	//Dimensions
	int owidth = pSrc->GetWidth(), oheight = pSrc->GetHeight();
	int depth = pSrc->GetDepth();
	int width = (int)(owidth / scale);
	int height = (int)(oheight / scale);

	// 降采样
	LPSURFACE img = pSrc;

	const auto bScale = scale != 1.0f;

	if (bScale) {
		img = CreateSurface(24, width, height);
		img->Clone(*pSrc, width, height);
	}

	//Lock buffer, get pitch etc.
	auto coef = GetSfCoef(img);
	if (coef.pData == nullptr) {		
		if (bScale) { delete img;	}

		return;
	}

	static unsigned short const stackblur_mul[255] =
	{
			512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
			454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
			482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
			437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
			497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
			320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
			446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
			329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
			505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
			399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
			324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
			268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
			451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
			385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
			332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
			289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
	};

	static unsigned char const stackblur_shr[255] =
	{
			9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
			17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
			19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
			20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
			21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
			21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
			22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
			22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
			23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
			23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
			23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
			23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
			24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
			24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
			24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
			24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
	};

	int t_width = width / divide;
	int t_height = height / divide;

#ifdef STACK_BLUR_ALPHA
	bool bBlurAlpha = false;

	auto GetStride = [&](bool dir) {
		return dir
			? (bBlurAlpha
				? coef.alphaPitch
				: coef.pitch)
			: (bBlurAlpha
				? coef.alphaByte
				: coef.byte);
	};
#else
	auto GetStride = [&](bool dir) {
		return dir ? coef.pitch : coef.byte;
	};
#endif // STACK_BLUR_ALPHA	

	using PixelGetter = std::function<RGBA(BYTE* src, int src_offset)>;
	using PixelSetter = std::function<void(RGBA src, BYTE* des, int des_offset)>;

	PixelGetter normalGetter = [](BYTE* src, int src_offset) {
		return RGBA{ (double)src[src_offset + 2], (double)src[src_offset + 1], (double)src[src_offset + 0], 0 };
	};
	PixelSetter normalSetter = [](const RGBA& src, BYTE* des, int des_offset) {
		des[des_offset + 2] = (BYTE)src.r;
		des[des_offset + 1] = (BYTE)src.g;
		des[des_offset + 0] = (BYTE)src.b;
	};

#ifdef STACK_BLUR_ALPHA
	PixelGetter alphaGetter = [](BYTE* src, int src_offset) {
		return RGBA{ (double)src[src_offset + 0], 0, 0, 0 };
	};
	PixelSetter alphaSetter = [](const RGBA& src, BYTE* des, int des_offset) {
		des[des_offset + 0] = (BYTE)src.r;
	};
#endif // STACK_BLUR_ALPHA	

	auto StackBlur1DFilter = [=](BYTE* src, BYTE* des
		, int size, bool dir
		, PixelGetter getter, PixelSetter setter) {
		int div = radius * 2 + 1;
		int sizem = size - 1;

		int stride = GetStride(dir);
		int o_stride = GetStride(!dir);

		int src_offset = 0;
		int des_offset = 0;

		int mul_sum = stackblur_mul[radius];
		char shr_sum = stackblur_shr[radius];

		RGBA Sum = { 0,0,0,0 };
		RGBA Sum_In = { 0,0,0,0 };
		RGBA Sum_Out = { 0,0,0,0 };
		RGBA* stack = (RGBA*)malloc(sizeof(RGBA) * (div));

		for (int i = 0; i <= radius; i++) {
			int stack_offset = i;
			//RGBA src_pixel = { (double)src[2],(double)src[1],(double)src[0],0 };
			RGBA src_pixel = getter(src, 0);

			stack[stack_offset] = src_pixel;
			Sum = Sum + src_pixel * (i + 1);
			Sum_Out = Sum_Out + src_pixel;
		}

		src_offset = 0;

		for (int i = 1; i <= radius; i++) {
			if (i <= sizem) {
				src_offset += stride;
			}
			
			int stack_offset = i + radius;
			//RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
			RGBA src_pixel = getter(src, src_offset);
			
			stack[stack_offset] = src_pixel;
			Sum = Sum + src_pixel * (radius + 1 - i);
			Sum_In = Sum_In + src_pixel;
		}

		int sp = radius;
		int xp = radius;

		if (xp > sizem) {
			xp = sizem;
		}

		src_offset = xp * stride;
		des_offset = 0;

		for (int i = 0; i < size; i++) {
			RGBA des_pixel = (Sum * mul_sum) >> shr_sum;

			//des[des_offset + 2] = (BYTE)des_pixel.r;
			//des[des_offset + 1] = (BYTE)des_pixel.g;
			//des[des_offset + 0] = (BYTE)des_pixel.b;

			setter(des_pixel, des, des_offset);

			des_offset += stride;

			Sum = Sum - Sum_Out;

			int stack_offset = sp + div - radius;
			if (stack_offset >= div) {
				stack_offset -= div;
			}
			Sum_Out = Sum_Out - stack[stack_offset];

			if (xp < sizem) {
				src_offset += stride;
				xp++;
			}

			//RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
			RGBA src_pixel = getter(src, src_offset);
			stack[stack_offset] = src_pixel;

			Sum_In = Sum_In + src_pixel;
			Sum = Sum + Sum_In;

			sp++;

			if (sp >= div) {
				sp = 0;
			}

			stack_offset = sp;

			Sum_Out = Sum_Out + stack[stack_offset];
			Sum_In = Sum_In - stack[stack_offset];
		}

		free(stack);
	};

	auto Filter1D = [&](BYTE* src, int it_size, int filter_size, bool dir) {
		int stride = GetStride(dir);
		int o_stride = GetStride(!dir);

		for (int i = 0; i < it_size; i++) {
			StackBlur1DFilter(src + i * o_stride, src + i * o_stride
				, filter_size, dir
#ifdef STACK_BLUR_ALPHA
				, (bBlurAlpha
					? alphaGetter
					: normalGetter)
				, (bBlurAlpha
					? alphaSetter
					: normalSetter));
#else
				, normalGetter, normalSetter);
#endif // STACK_BLUR_ALPHA		
		}
	};

	auto multithread = [&](BYTE* buff, bool dir) {
		std::vector<std::thread> t_vec;

		int stride = GetStride(dir);
		int o_stride = GetStride(!dir);

		for (int i = 0; i < divide; i++) {
			//Edge
			int t_rsize = dir ? height : width;
			int t_risize = ((!dir) ? t_height : t_width);

			if (i == divide - 1) {
				t_risize = ((!dir) ? height : width) - i * t_risize;
			}

			int offset = i * ((!dir) ? t_height : t_width);
			//Filter1D(buff + offset * o_stride, t_risize, t_rsize, dir);
			t_vec.emplace_back(std::thread(Filter1D, buff + offset * o_stride, t_risize, t_rsize, dir));
		}

		for (auto& it : t_vec) {
			it.join();
		}
	};

	multithread(coef.pData, Dir_X);
	multithread(coef.pData, Dir_Y);

#ifdef STACK_BLUR_ALPHA
	if (img->HasAlpha() && !IsOpaque(img)) {
		bBlurAlpha = true;

		multithread(coef.pAlphaData, Dir_X);
		multithread(coef.pAlphaData, Dir_Y);
	}
#endif // STACK_BLUR_ALPHA	
	
	ReleaseSfCoef(img, coef);

	//还原大小
	if (bScale) {
		pSrc->Clone(*img, owidth, oheight);
		delete img;
	}

	return;
}

// use macro to speed up compiling
#ifdef PERSPECTIVE_TRANSFORMATION
#pragma warning(disable : 4819)

#include "Eigen/Dense"

inline void GetReversePerspectiveTransformationPoint(double in[3][3]){
	Eigen::Matrix<double, 3, 3, Eigen::RowMajor> arr(in[0]);
	arr = arr.inverse().eval();

	std::copy_n(arr.data(), 9, &in[0][0]);
}

inline constexpr auto PerspectiveTransformationPoint(const double x, const double y,
	const double matrix[3][3]) {
	const auto div = (matrix[2][0] * x + matrix[2][1] * y + matrix[2][2]);
	const auto newX = (matrix[0][0] * x + matrix[0][1] * y + matrix[0][2]) / div;
	const auto newY = (matrix[1][0] * x + matrix[1][1] * y + matrix[1][2]) / div;

	return std::make_tuple(newX, newY);
}

inline constexpr auto GetPerspectiveTransformationSize(const double width, const double height,
	const double matrix[3][3]) {
	const auto [xLT, yLT] = PerspectiveTransformationPoint(0, 0, matrix);
	const auto [xRT, yRT] = PerspectiveTransformationPoint(width, 0, matrix);
	const auto [xLB, yLB] = PerspectiveTransformationPoint(0, height, matrix);
	const auto [xRB, yRB] = PerspectiveTransformationPoint(width, height, matrix);

	const auto xList = { xLT, xRT, xLB, xRB };
	const auto yList = { yLT, yRT, yLB, yRB };

	//auto getOffset = [] (const auto& list) {
	//	auto min = (std::min)(list);
	//	auto max = (std::max)(list);

	//	return max - min;
	//};

	//const auto newWidth = getOffset(xList);
	//const auto newHeight = getOffset(yList);

	//return std::make_tuple(newWidth, newHeight);

	auto minX = (std::min)(xList);
	auto maxX = (std::max)(xList);
	auto minY = (std::min)(yList);
	auto maxY = (std::max)(yList);

	return std::make_tuple(minX, minY, maxX, maxY);
}

inline LPSURFACE PerspectiveTransformation(const LPSURFACE pSrc, const double matrix[3][3]) {
	const auto [minX, minY, maxX, maxY]
		= GetPerspectiveTransformationSize(pSrc->GetWidth(), pSrc->GetHeight(), matrix);

	const auto newWidth = static_cast<int>(maxX - minX);
	const auto newHeight = static_cast<int>(maxY - minY);

	const auto pBitmap = CreateSurface(pSrc->GetDepth(), newWidth, newHeight);
	pBitmap->CreateAlpha();	

	double rm[3][3] = { 0.0 };
	std::copy_n(&matrix[0][0], 9, &rm[0][0]);
	GetReversePerspectiveTransformationPoint(rm);

	const auto pSrcCoef = GetSfCoef(pSrc);
	if (pSrcCoef.pData == nullptr || pSrcCoef.pAlphaData == nullptr) {
		delete pBitmap;

		return nullptr;
	}

	const auto width = pSrc->GetWidth();
	const auto height = pSrc->GetHeight();

	IteratePixel(pBitmap, [&] (const int x, const int y,
		const SfCoef& sfCoef,
		BYTE* pPixel, BYTE* pAlphaPixel) {
			//const auto r = static_cast<BYTE>(pPixel[2]);
			//const auto g = static_cast<BYTE>(pPixel[1]);
			//const auto b = static_cast<BYTE>(pPixel[0]);
			//const auto a = static_cast<BYTE>(pAlphaPixel[0]);

			const auto [nx, ny] = PerspectiveTransformationPoint(x + static_cast<int>(minX),
				y + static_cast<int>(minY),
				rm);
			const auto nxi = Range(static_cast<int>(nx), 0, width - 1);
			const auto nyi = Range(static_cast<int>(ny), 0, height - 1);

			const auto pSrcPixel = GetPixelAddress(width, height, nxi, nyi, pSrcCoef);
			const auto pSrcAlphaPixel = GetAlphaPixelAddress(width, height, nxi, nyi, pSrcCoef);

			pPixel[2] = pSrcPixel[2];
			pPixel[1] = pSrcPixel[1];
			pPixel[0] = pSrcPixel[0];

			pAlphaPixel[0] = pSrcAlphaPixel[0];
	});

	ReleaseSfCoef(pSrc, pSrcCoef);

	return pBitmap;
}
#endif

//dec2rgb
#define DEC2RGB(DEC) RGB(((DEC) >> 16), ((DEC) >> 8) & 0xff, (DEC) & 0xff)

inline SfCoef GetSfCoef(LPSURFACE pSf) {
	SfCoef pSfCoef = { 0 };
	
	if (pSf == nullptr) {
		return pSfCoef;
	}

	int width = pSf->GetWidth();
	int height = pSf->GetHeight();

	// normal
	pSfCoef.pData = (BYTE*)pSf->LockBuffer();

	if (!pSfCoef.pData) { return pSfCoef; }

	pSfCoef.pitch = pSf->GetPitch();
	if (pSfCoef.pitch < 0) {
		pSfCoef.pitch *= -1;
		pSfCoef.pData -= pSfCoef.pitch * (height - 1);
	}
	pSfCoef.sz = pSfCoef.pitch * height;
	pSfCoef.byte = pSf->GetDepth() >> 3;

	// alpha
	if (pSf->HasAlpha()) {
		pSfCoef.pAlphaData = pSf->LockAlpha();
		
		if (!pSfCoef.pAlphaData) { return pSfCoef; }
		
		pSfCoef.alphaPitch = pSf->GetAlphaPitch();
		if (pSfCoef.alphaPitch < 0) {
			pSfCoef.alphaPitch *= -1;
			pSfCoef.pAlphaData -= pSfCoef.alphaPitch * (height - 1);
		}
		pSfCoef.alphaSz = pSfCoef.alphaPitch * height;
		pSfCoef.alphaByte = 1;
	}

	return pSfCoef;
}

inline void ReleaseSfCoef(LPSURFACE pSf, const SfCoef& coef) {
	pSf->UnlockBuffer(coef.pData);

	// alpha
	if (pSf->HasAlpha()) {
		pSf->UnlockAlpha();
	}
}

inline BYTE* GetPixelAddress(int width, int height, int x, int y, const SfCoef& coef) {
	const auto offset = y * coef.pitch + x * coef.byte;
	BYTE* srcPixel = coef.pData + offset;
	
	return srcPixel;
}

inline BYTE* GetAlphaPixelAddress(int width, int height, int x, int y, const SfCoef& coef) {
	const auto alphaOffset = (height - 1 - y) * coef.alphaPitch + x * coef.alphaByte;
	BYTE* alphaPixel = coef.pAlphaData + alphaOffset;

	return alphaPixel;
}

inline void ProcessPixel(LPSURFACE pSf, const std::function<void(const SfCoef& coef)>& processor) {
	ProcessBitmap(pSf, [&](const LPSURFACE Bitmap) {
		const auto coef = GetSfCoef(pSf);
		if (coef.pData == nullptr) {		
			return;
		}

		if(pSf->HasAlpha() && coef.pAlphaData == nullptr) {
			return;
		}

		processor(coef);
		ReleaseSfCoef(pSf, coef);
	});
}

inline void ProcessPixel(LPRDATA rdPtr, LPSURFACE pSf, const std::function<void(const SfCoef& coef)>& processor) {
	ProcessPixel(pSf, processor);
}

inline void IteratePixel(LPSURFACE pSf, const std::function<void(int,int,const SfCoef&,BYTE*,BYTE*)>& process) {
	ProcessPixel(pSf, [&] (const SfCoef& coef) {
		//Dimensions
		int width = pSf->GetWidth();
		int height = pSf->GetHeight();

		//Lock buffer, get pitch etc.
		BYTE* pData = coef.pData;
		int pitch = coef.pitch;
		int size = coef.sz;
		int byte = coef.byte;

		//Alpha
		BYTE* pAlphaData = coef.pAlphaData;
		int alphaPitch = coef.alphaPitch;
		int alphaSz = coef.alphaSz;
		int alphaByte = coef.alphaByte;

		//Loop through all pixels
#ifdef _OPENMP 
		omp_set_num_threads(std::thread::hardware_concurrency());

#pragma omp parallel for  
#endif
		for (int y = 0; y < height; ++y) {
#ifdef _OPENMP
//#pragma omp parallel shared(y)
			{
//#pragma omp parallel for
#endif
			const auto yOffset = y * pitch;
			const auto yAlphaOffset =  (height - 1 - y) * alphaPitch;

			for (int x = 0; x < width; ++x) {
				const auto offset = yOffset + x * byte;
				BYTE* srcPixel = pData + offset;

				const auto alphaOffset = yAlphaOffset + x * alphaByte;
				BYTE* alphaPixel = pAlphaData + alphaOffset;

				process(x, y, coef
					, srcPixel, alphaPixel);
			}
#ifdef _OPENMP
		}
#endif
		}
});
}

// dst transparent -> src alpha
// TODO: BOP_BLEND_REPLACETRANSP ?
inline bool MixAlpha(LPSURFACE pSrc, int srcX, int srcY, int srcWidth, int srcHeight
	, LPSURFACE pDst, int destX, int destY) {
	if(!pSrc->HasAlpha()){
		return false;
	}

	if (!pDst->HasAlpha()) {
		pDst->CreateAlpha();
	}

	auto srcCoef = GetSfCoef(pSrc);
	if (srcCoef.pData == nullptr || srcCoef.pAlphaData == nullptr) {
		return false;
	}

	auto dstCoef = GetSfCoef(pDst);
	if (dstCoef.pData == nullptr || dstCoef.pAlphaData == nullptr) {	
		return false;
	}

	int widthS = pSrc->GetWidth();
	int heightS = pSrc->GetHeight();

	BYTE* pAlphaDataS = srcCoef.pAlphaData;
	int alphaPitchS = srcCoef.alphaPitch;
	int alphaSzS = srcCoef.alphaSz;
	int alphaByteS = srcCoef.alphaByte;

	int widthD = pDst->GetWidth();
	int heightD = pDst->GetHeight();

	BYTE* pAlphaDataD = dstCoef.pAlphaData;
	int alphaPitchD = dstCoef.alphaPitch;
	int alphaSzD = dstCoef.alphaSz;
	int alphaByteD = dstCoef.alphaByte;

#define _PRE_PROTECT

#ifdef _PRE_PROTECT
	auto Range = [](int inputV, int minV, int maxV) {
		return (std::min)(maxV, (std::max)(minV, inputV));
	};
		
	auto actualWidth = Range(srcX + srcWidth, 0, widthS);
	auto actualHeight = Range(srcY + srcHeight, 0, heightS);

	auto actualSrcX= Range(srcX, 0, widthS);
	auto actualSrcY = Range(srcY, 0, heightS);

	for (int y = actualSrcY; y < actualHeight; y++) {
		for (int x = actualSrcX; x < actualWidth; x++) {
#else
	for (int y = srcY; y < srcY + srcHeight; y++) {
		for (int x = srcX; x < srcX + srcWidth; x++) {
#endif
			auto Protection = [](LPSURFACE pSf, int x, int y) {
				if (x < 0 || x >= pSf->GetWidth()) {
					return false;
				}

				if (y < 0 || y >= pSf->GetHeight()) {
					return false;
				}

				return true;
			};

			auto dstX = (x + destX);
			auto dstY = (y + destY);

			//protection
#ifdef _PRE_PROTECT
			if (!Protection(pDst, dstX, dstY)) {
#else
			if (!Protection(pSrc, x, y) || !Protection(pDst, dstX, dstY)) {
#endif
				continue;
			}

			auto alphaOffsetS = (y)*alphaPitchS + x * alphaByteS;
			auto alphaOffsetD = (dstY)*alphaPitchD + dstX * alphaByteD;

			BYTE* alphaPixelS = pAlphaDataS + alphaOffsetS;
			BYTE* alphaPixelD = pAlphaDataD + alphaOffsetD;

			auto src = 1.0 - alphaPixelS[0] / 255.0;
			auto dst = 1.0 - alphaPixelD[0] / 255.0;

			auto result = (1.0 - src * dst) * 255;

			*alphaPixelD = (BYTE)result;
		}
	}

	ReleaseSfCoef(pDst, dstCoef);
	ReleaseSfCoef(pSrc, srcCoef);

	return true;
}

inline bool MixAlpha(LPSURFACE pSrc, LPSURFACE pDst, int destX, int destY) {
	return MixAlpha(pSrc, 0, 0, pSrc->GetWidth(), pSrc->GetHeight()
		, pDst, destX, destY);
}

inline uint64_t GetEstimateSize(LPSURFACE pSf) {
	return static_cast<uint64_t>(pSf->GetWidth()) * pSf->GetHeight()
		// depth -> byte
		* (static_cast<uint64_t>(pSf->GetDepth()) >> 3);
}

inline uint64_t GetEstimateSizeMB(LPSURFACE pSf) {
	return GetEstimateSize(pSf) >> 20;
}

inline void AccessBackground(LPRDATA rdPtr) {
	const auto rhPtr = rdPtr->rHo.hoAdRunHeader;

	// non-HWA mode can access directly
	const auto bHWA = ((rhPtr->rh4.rh4Mv->mvAppMode & SM_D3D) != 0);
	if (!bHWA) { return; }

	mvNeebBackgroundAccess(rhPtr->rh4.rh4Mv, rhPtr->rhFrame, TRUE);
}

inline void ProcessBackground(LPRDATA rdPtr, const std::function<void(const LPSURFACE)>& callback) {
	const auto rhPtr = rdPtr->rHo.hoAdRunHeader;
	const auto ps = WinGetSurface((int)rhPtr->rhIdEditWin);

	const auto bHWA = ((rhPtr->rh4.rh4Mv->mvAppMode & SM_D3D) != 0);

	if (!bHWA) {
		callback(ps);

		return;
	}

	// HWA : get current render target (to use as source)
	const auto pRTT = ps->GetRenderTargetSurface();
	callback(ps);
	ps->ReleaseRenderTargetSurface(pRTT);
}