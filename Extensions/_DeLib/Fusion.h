#pragma once

#include	"ccxhdr.h"
#include	"Surface.h"

#include	"ImgFlt.h"
#include	"ImageFlt.h"
#include	"CfcFile.h"

#include	<vector>
#include	<thread>

//Check if a dir has animation
inline bool DirHasAnimation(LPRDATA rdPtr, LPRO object, size_t Dir) {
	Dir = max(0, min(DIRID_MAX - 1, Dir));

	if (object == NULL) {
		return false;
	}

	return (object->roa.raAnimOffset->anOffsetToDir[Dir] > 0) ? true : false;
}

inline bool DirHasAnimation(LPRDATA rdPtr, int Fixed, size_t Dir) {	
	return DirHasAnimation(rdPtr, LproFromFixed(rdPtr, Fixed), Dir);
}

#define DirHasAnimation(X, Dir) DirHasAnimation(rdPtr, X, Dir)

//Get object's display animation direction
inline size_t DisplayAnimationDirection(LPRDATA rdPtr, LPRO object) {
	if (object == NULL) {
		return 0;
	}

	//Mechanism
	//fusion decides to display a replaced animation when current direction doesn't have one by following rules below:
	//	- Display the closest direction(0 is treated as 32)
	//	- If both former and later have the same distance, display the one backward to the direction of direction change(if only 0 and 8 have animation, switch from 3->4, display 0, switch from 5->4, display 8)

	size_t former = 0;
	size_t later = 0;

	size_t curdir = object->roa.raAnimDir;
	size_t prevdir = object->roa.raAnimPreviousDir;

	bool clockwize;

	if (prevdir == 0) {
		if (curdir <= 16) {
			clockwize = false;
		}
		else {
			clockwize = true;
		}
	}
	else {
		clockwize = prevdir >= curdir;
	}

	//former
	for (size_t pos = curdir; pos >= 0; pos--) {
		if (DirHasAnimation(object, pos)) {
			former = pos;
			break;
		}
	}

	//later
	for (size_t pos = curdir; pos <= DIRID_MAX; pos++) {
		if (DirHasAnimation(object, pos % 32)) {
			later = pos;
			break;
		}
	}

	if ((curdir - former) == (later - curdir)) {
		return !clockwize ? former : later % 32;
	}
	else {
		return (curdir - former) <= (later - curdir) ? former : later % 32;
	}
}

inline size_t DisplayAnimationDirection(LPRDATA rdPtr, int Fixed) {
	return DisplayAnimationDirection(rdPtr, LproFromFixed(rdPtr, Fixed));
}

#define DisplayAnimationDirection(X) DisplayAnimationDirection(rdPtr, X)

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

//RGBA��ֵ����
inline double Range(double A) {
	return max(0.0, min(255.0, A));
}

inline RGBA Range(RGBA A) {
	A.r = Range(A.r);
	A.g = Range(A.g);
	A.b = Range(A.b);
	A.a = Range(A.a);

	return A;
}

//RGBA��������� +
inline RGBA operator +(RGBA A, RGBA B) {	
	A.r += B.r;
	A.g += B.g;
	A.b += B.b;
	A.a += B.a;

	return A;	
}

//RGBA��������� -
inline RGBA operator -(RGBA A, RGBA B) {
	A.r -= B.r;
	A.g -= B.g;
	A.b -= B.b;
	A.a -= B.a;

	return A;
}

//RGBA��������� +=
inline RGBA operator +=(RGBA A, RGBA B) {
	return A + B;
}

//RGBA��������� -=
inline RGBA operator -=(RGBA A, RGBA B) {
	return A - B;
}

//RGBA��������� *
inline RGBA operator *(RGBA A, double B) {
	A.r = A.r * B;
	A.g = A.g * B;
	A.b = A.b * B;
	A.a = A.a * B;

	return A;
}

//RGBA��������� /
inline RGBA operator /(RGBA A, double B) {
	A.r = A.r / B;
	A.g = A.g / B;
	A.b = A.b / B;
	A.a = A.a / B;

	return A;
}

//RGBA��������� *
inline RGBA operator *(double B, RGBA A) {
	return A * B;
}

//RGBA��������� /
inline RGBA operator /(double B, RGBA A) {
	return A / B;
}

//RGBA��������� >>
inline RGBA operator >>(RGBA A, int B) {
	A.r = (double)((int)A.r >> B);
	A.g = (double)((int)A.g >> B);
	A.b = (double)((int)A.b >> B);
	A.a = (double)((int)A.a >> B);

	return A;
}

//Stretch Surface
inline void Stretch(LPSURFACE Src, LPSURFACE Des, bool HighQuality) {
	DWORD flag = 0;

	if (HighQuality)
		flag = flag | STRF_RESAMPLE;
	if (Src->HasAlpha())
		flag = flag | STRF_COPYALPHA;

	Src->Stretch(*Des, 0, 0, Des->GetWidth(), Des->GetHeight(), BMODE_OPAQUE, BOP_COPY, 0, flag);

	return;
}

//Get Ext's FilterName
inline LPCWSTR GetFilterName(LPRDATA rdPtr, LPCWSTR Name) {
	//To Lower
	auto LowerWStr = [](LPWSTR Str)->LPWSTR {
		int i = 0;
		while (Str[i]) {
			Str[i] = towlower(Str[i]);
			i++;
		}
		return Str;
	};

	//�����ᱻ����ȫ�ֱ�����
	LPCWSTR ExtList[8][2] = {
		_T(".png"),_T("Portable Network Graphics"),
		_T(".tga"),_T("Targa Bitmap"),
		_T(".bmp"),_T("Windows Bitmap"),
		_T(".jpg"),_T("JPEG"),
		_T(".flc"),_T("Autodesk FLIC"),
		_T(".gif"),_T("Compuserve Bitmap"),
		_T(".avi"),_T("Video For Windows"),
		_T(".pcx"),_T("PaintBrush")
	};

	WCHAR* Ext = new WCHAR[FILENAME_MAX];
	wcscpy_s(Ext, FILENAME_MAX, Name);
	LowerWStr(Ext);

	for (int i = 0; i < 8; i++) {
		if (wcscmp(Ext, ExtList[i][0]) == 0) {
			delete[] Ext;
			return ExtList[i][1];
		}
	}

	delete[] Ext;

	//default format is JPEG
	//return _T("JPEG");
	return (rdPtr->DefaultFilterName == nullptr) ? ExtList[3][1] : rdPtr->DefaultFilterName;
};

//Get Filter ID By File Name
inline DWORD GetFilterIDByFileName(LPRDATA rdPtr, LPCTSTR FilePath) {
	//Surface
	CImageFilterMgr* pImgMgr = rdPtr->rhPtr->rh4.rh4Mv->mvImgFilterMgr;
	CImageFilter    pFilter(pImgMgr);

	//Get Ext's FilterID
	auto GetFilterID = [pImgMgr, rdPtr](LPCWSTR Name) -> DWORD {
		LPCWSTR FilterName = GetFilterName(rdPtr, Name);
		for (int i = 0; i < pImgMgr->GetFilterCount(); i++) {
			if (wcscmp(pImgMgr->GetFilterNameW(i), FilterName) == 0) {
				DWORD FilterID = pImgMgr->GetFilterID(i);
				return pImgMgr->GetFilterID(i);
			}
		}
		return pImgMgr->GetFilterID(0);
	};

	//Get Ext from FileName
	WCHAR* Ext = new WCHAR[FILENAME_MAX];
	_wsplitpath_s(FilePath, NULL, 0, NULL, 0, NULL, 0, Ext, FILENAME_MAX);

	DWORD FilterID = GetFilterID(Ext);

	delete[] Ext;

	return FilterID;
}

//Get Valid Sacle
inline void GetValidScale(float* scale) {
	*scale = max(1, *scale);
	return;
}

//Get Max supported thread of current CPU
inline void GetMaxmiumDivide(int* divide) {
	int Max = std::thread::hardware_concurrency();

	if (*divide == -1) {
		*divide = Max;
	}
	else {
		*divide = max(1, min(*divide, Max));
	}
	return;
}

//Stack Blur
inline void StackBlur(LPSURFACE img, int radius, float scale, int divide) {
	//��ȡ����
	constexpr auto SB_MIN_RADIUS = 0;
	constexpr auto SB_MAX_RADIUS = 254;

	radius = min(SB_MAX_RADIUS, max(SB_MIN_RADIUS, radius));

	GetValidScale(&scale);
	GetMaxmiumDivide(&divide);

	//Dimensions
	int owidth = img->GetWidth(), oheight = img->GetHeight();
	int width = (int)(owidth / scale);
	int height = (int)(oheight / scale);

	// ������
	cSurface ResizedImg;
	LPSURFACE proto = nullptr;
	GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);

	if (!(scale == 1.0)) {
		ResizedImg.Clone(*img);

		img->Delete();
		img->Create(width, height, proto);

		Stretch(&ResizedImg, img, Fast);
	}

	//Lock buffer, get pitch etc.
	BYTE* buff = img->LockBuffer();
	if (!buff) { 
		return; 
	}

	int pitch = img->GetPitch();
	if (pitch < 0) {
		pitch *= -1;
		buff -= pitch * (height - 1);
	}
	int size = pitch * height;
	int byte = img->GetDepth() >> 3;

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

	auto StackBlur1DFilter = [=](BYTE* src, BYTE* des, int size, bool dir) {
		int div = radius * 2 + 1;
		int sizem = size - 1;
		int stride = dir ? pitch : byte;

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
			RGBA src_pixel = { (double)src[2],(double)src[1],(double)src[0],0 };
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
			RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
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
			des[des_offset + 2] = (BYTE)des_pixel.r;
			des[des_offset + 1] = (BYTE)des_pixel.g;
			des[des_offset + 0] = (BYTE)des_pixel.b;
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

			RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
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

	auto Filter1D = [StackBlur1DFilter, byte, pitch](BYTE* src, int it_size, int filter_size, bool dir) {
		int stride = dir ? pitch : byte;
		int o_stride = (!dir) ? pitch : byte;

		for (int i = 0; i < it_size; i++) {
			StackBlur1DFilter(src + i * o_stride, src + i * o_stride, filter_size, dir);
		}
	};

	auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch](BYTE* buff, bool dir) {
		std::vector<std::thread> t_vec;
		int stride = dir ? pitch : byte;
		int o_stride = (!dir) ? pitch : byte;

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

	multithread(buff, Dir_X);
	multithread(buff, Dir_Y);

	img->UnlockBuffer(buff);

	//��ԭ��С
	if (!(scale == 1.0)) {
		ResizedImg.Clone(*img);

		img->Delete();
		img->Create(owidth, oheight, proto);

		Stretch(&ResizedImg, img, Fast);
	}

	return;
}