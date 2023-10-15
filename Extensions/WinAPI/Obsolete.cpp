//Funcs that don't use but may use as a reference later


////截取场景区域到剪贴板
//short WINAPI DLLExport BitBltFrameArea(LPRDATA rdPtr, long param1, long param2) {
//	// 截取
//	// 参考范例：
//	// https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
//
//	// Source Area
//	RECT CurrentFrameRect;
//	::GetWindowRect(rdPtr->FrameWindowHandle, &CurrentFrameRect);
//	int FrameWidth = CurrentFrameRect.right - CurrentFrameRect.left;
//	int FrameHeight = CurrentFrameRect.bottom - CurrentFrameRect.top;
//
//	// Defination
//	HDC hdcWindow;
//	HDC hdcMemDC = NULL;
//	HBITMAP hbmScreen = NULL;
//
//	// Retrieve the handle to a display device context.
//	hdcWindow = GetDC(rdPtr->FrameWindowHandle);
//
//	// Create a compatible DC, which is used in a BitBlt from the window DC.
//	hdcMemDC = CreateCompatibleDC(hdcWindow);
//
//	// Create a compatible bitmap from the Window DC.
//	hbmScreen = CreateCompatibleBitmap(hdcWindow, FrameWidth, FrameHeight);
//
//	// Select the compatible bitmap into the compatible memory DC.
//	SelectObject(hdcMemDC, hbmScreen);
//
//	// Bit block transfer into our compatible memory DC.
//	BitBlt(hdcMemDC,
//		0, 0,
//		FrameWidth, FrameHeight,
//		hdcWindow,
//		0, 0,
//		SRCCOPY);
//
//	// 暂存到剪贴板
//	OpenClipboard(rdPtr->MainWindowHandle);
//	EmptyClipboard();
//	SetClipboardData(CF_BITMAP, hbmScreen);
//	CloseClipboard();
//
//	// Clean up.
//	DeleteObject(hbmScreen);
//	DeleteObject(hdcMemDC);
//	ReleaseDC(rdPtr->FrameWindowHandle, hdcWindow);
//
//	//保存与缩放
//	int Width = CNC_GetIntParameter(rdPtr);
//	int Height = CNC_GetIntParameter(rdPtr);
//	bool SaveToFile = CNC_GetIntParameter(rdPtr);
//	LPCTSTR FilePath = (LPCTSTR)CNC_GetStringParameter(rdPtr);
//
//	cSurface img;
//	LPSURFACE proto = nullptr;
//	CImageFilterMgr* pImgMgr = rdPtr->rhPtr->rh4.rh4Mv->mvImgFilterMgr;
//	CImageFilter    pFilter(pImgMgr);
//
//	//放弃了直接从HBITMAP获取信息
//	//BITMAPINFO* bmp = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));
//	//memset(bmp, 0, sizeof(*bmp));
//	//bmp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	//GetDIBits(hdcMemDC, hbmScreen, 0, 1, NULL, (BITMAPINFO*)bmp, DIB_RGB_COLORS);
//
//	//从剪贴板获取位图信息
//	OpenClipboard(rdPtr->MainWindowHandle);
//	BITMAPINFO* bmp = (BITMAPINFO*)GlobalLock(GetClipboardData(CF_DIB));
//
//	//Surface获取位图信息
//	GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
//	img.Create(FrameWidth, FrameHeight, proto);
//	img.LoadImage(bmp, GetDIBBitmap(bmp));
//	//img->SetTransparentColor(transCol);
//
//	CloseClipboard();
//
//	//缩放
//	cSurface ResizedImg;
//	ResizedImg.Clone(img);
//
//	img.Delete();
//	img.Create(Width, Height, proto);
//
//	ResizedImg.Stretch(img, 0, 0, Width, Height, BMODE_OPAQUE, BOP_COPY, 0, STRF_RESAMPLE);
//
//	//输出缩放后的图像到剪贴板
//	OpenClipboard(rdPtr->MainWindowHandle);
//	EmptyClipboard();
//
//	HGLOBAL cb = GlobalAlloc(GMEM_MOVEABLE, img.GetDIBSize());
//	BITMAPINFO* OutPut = (BITMAPINFO*)GlobalLock(cb);
//
//	img.SaveImage(OutPut, (BYTE*)(OutPut + 1) - 4);
//	SetClipboardData(CF_DIB, OutPut);
//
//	GlobalUnlock(cb);
//	CloseClipboard();
//
//	//保存到文件	
//	if (!SaveToFile) {
//		return 0;
//	}
//
//	//获取JPEG格式的FilterID
//	auto GetFilterID = [pImgMgr]() -> DWORD {
//		for (int i = 0; i < pImgMgr->GetFilterCount(); i++)
//		{
//			if (wcscmp(pImgMgr->GetFilterNameW(i), _T("JPEG")) == 0) {
//				return pImgMgr->GetFilterID(i);
//			}
//		}
//		return pImgMgr->GetFilterID(0);
//	};
//
//	ExportImage(pImgMgr, FilePath, &img, GetFilterID());
//
//	return 0;
//}

// Alpha Blur
//short WINAPI DLLExport MultiThreadStackBlur(LPRDATA rdPtr, long param1, long param2) {
//	if (rdPtr->Display) {
//		//获取参数
//		constexpr auto SB_MIN_RADIUS = 0;
//		constexpr auto SB_MAX_RADIUS = 254;
//
//		int radius = CNC_GetIntParameter(rdPtr);
//		radius = min(SB_MAX_RADIUS, max(SB_MIN_RADIUS, radius));
//
//		long p2 = CNC_GetFloatParameter(rdPtr);
//		float scale = *(float*)&p2;
//		GetValidScale(&scale);
//
//		int divide = CNC_GetIntParameter(rdPtr);
//		GetMaximumDivide(&divide);
//
//		//Dimensions
//		int owidth = rdPtr->img.GetWidth(), oheight = rdPtr->img.GetHeight();
//		int width = (int)(owidth / scale);
//		int height = (int)(oheight / scale);
//
//		// 降采样
//		LPSURFACE proto = nullptr;
//		GetSurfacePrototype(&proto, 24, ST_MEMORYWITHDC, SD_DIB);
//
//		cSurface ResizedImg;
//
//		ResizedImg.Clone(rdPtr->img);
//
//		rdPtr->img.Delete();
//		rdPtr->img.Create(width, height, proto);
//
//		ResizedImg.Stretch(rdPtr->img, 0, 0, width, height, BMODE_OPAQUE, BOP_COPY, 0, STRF_RESAMPLE | STRF_COPYALPHA);
//
//		//Alpha channel
//		int Abyte = 1;
//		int Apitch = 0;
//		int Asize = 0;
//		LPBYTE Abuff = nullptr, Asrc = nullptr;
//
//		bool Alpha = rdPtr->img.HasAlpha();
//		if (Alpha) {
//			Abuff = rdPtr->img.LockAlpha();
//			Apitch = rdPtr->img.GetAlphaPitch();
//			if (Apitch < 0)
//			{
//				Apitch *= -1;
//				Abuff -= Apitch * (height - 1);
//			}
//			Asize = Apitch * height;
//			Asrc = (LPBYTE)malloc(Asize);
//			memcpy(Asrc, Abuff, Asize);
//		}
//
//		//Lock buffer, get pitch etc.
//		BYTE* buff = rdPtr->img.LockBuffer();
//		if (!buff) return 0;
//
//		int pitch = rdPtr->img.GetPitch();
//		if (pitch < 0)
//		{
//			pitch *= -1;
//			buff -= pitch * (height - 1);
//		}
//		int size = pitch * height;
//		int byte = rdPtr->img.GetDepth() >> 3;
//
//		static unsigned short const stackblur_mul[255] =
//		{
//				512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
//				454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
//				482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
//				437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
//				497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
//				320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
//				446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
//				329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
//				505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
//				399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
//				324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
//				268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
//				451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
//				385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
//				332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
//				289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
//		};
//
//		static unsigned char const stackblur_shr[255] =
//		{
//				9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
//				17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
//				19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
//				20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
//				21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
//				21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
//				22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
//				22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
//				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//				23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//				23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
//				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
//		};
//
//		int t_width = width / divide;
//		int t_height = height / divide;
//
//		auto StackBlur1DFilter = [=](BYTE* src, BYTE* des, int size, bool dir, bool alpha) {
//			int div = radius * 2 + 1;
//			int sizem = size - 1;
//			int stride = dir ? pitch : byte;
//
//			int src_offset = 0;
//			int des_offset = 0;
//
//			int mul_sum = stackblur_mul[radius];
//			char shr_sum = stackblur_shr[radius];
//
//			RGBA Sum = { 0,0,0,0 };
//			RGBA Sum_In = { 0,0,0,0 };
//			RGBA Sum_Out = { 0,0,0,0 };
//			RGBA* stack = (RGBA*)malloc(sizeof(RGBA) * (div));
//
//			for (int i = 0; i <= radius; i++) {
//				int stack_offset = i;
//				//RGBA src_pixel = { (double)src[2],(double)src[1],(double)src[0],0 };
//				RGBA src_pixel;
//				if (!alpha) {
//					src_pixel = { (double)src[2],(double)src[1],(double)src[0],0 };
//				}
//				else {
//					src_pixel = { (double)src[0],0 ,0 ,0 };
//				}
//				stack[stack_offset] = src_pixel;
//				Sum = Sum + src_pixel * (i + 1);
//				Sum_Out = Sum_Out + src_pixel;
//			}
//
//			src_offset = 0;
//
//			for (int i = 1; i <= radius; i++) {
//				if (i <= sizem) {
//					src_offset += stride;
//				}
//				int stack_offset = i + radius;
//				//RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
//				RGBA src_pixel;
//				if (!alpha) {
//					src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
//				}
//				else {
//					src_pixel = { (double)src[src_offset + 0],0 ,0 ,0 };
//				}
//				stack[stack_offset] = src_pixel;
//				Sum = Sum + src_pixel * (radius + 1 - i);
//				Sum_In = Sum_In + src_pixel;
//			}
//
//			int sp = radius;
//			int xp = radius;
//
//			if (xp > sizem) {
//				xp = sizem;
//			}
//
//			src_offset = xp * stride;
//			des_offset = 0;
//
//			for (int i = 0; i < size; i++) {
//				RGBA des_pixel = (Sum * mul_sum) >> shr_sum;
//				if (!alpha) {
//					des[des_offset + 2] = (BYTE)des_pixel.r;
//					des[des_offset + 1] = (BYTE)des_pixel.g;
//					des[des_offset + 0] = (BYTE)des_pixel.b;
//				}
//				else {
//					des[des_offset + 0] = (BYTE)des_pixel.r;
//				}
//				des_offset += stride;
//
//				Sum = Sum - Sum_Out;
//
//				int stack_offset = sp + div - radius;
//				if (stack_offset >= div) {
//					stack_offset -= div;
//				}
//				Sum_Out = Sum_Out - stack[stack_offset];
//
//				if (xp < sizem) {
//					src_offset += stride;
//					xp++;
//				}
//
//				//RGBA src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
//				RGBA src_pixel;
//				if (!alpha) {
//					src_pixel = { (double)src[src_offset + 2],(double)src[src_offset + 1],(double)src[src_offset + 0],0 };
//				}
//				else {
//					src_pixel = { (double)src[src_offset + 0],0 ,0 ,0 };
//				}
//				stack[stack_offset] = src_pixel;
//
//				Sum_In = Sum_In + src_pixel;
//				Sum = Sum + Sum_In;
//
//				sp++;
//
//				if (sp >= div) {
//					sp = 0;
//				}
//
//				stack_offset = sp;
//
//				Sum_Out = Sum_Out + stack[stack_offset];
//				Sum_In = Sum_In - stack[stack_offset];
//			}
//
//			free(stack);
//		};
//
//		auto Filter1D = [StackBlur1DFilter, byte, pitch](BYTE* src, int it_size, int filter_size, bool dir, bool alpha) {
//			int stride = dir ? pitch : byte;
//			int o_stride = (!dir) ? pitch : byte;
//
//			for (int i = 0; i < it_size; i++) {
//				StackBlur1DFilter(src + i * o_stride, src + i * o_stride, filter_size, dir, alpha);
//			}
//		};
//
//		//auto multithread = [Filter1D, divide, t_width, t_height, width, height, byte, pitch](BYTE* buff, bool dir, bool alpha) {
//		auto multithread = [=](BYTE* buff, bool dir, bool alpha) {
//			std::vector<std::thread> t_vec;
//			//int stride = dir ? pitch : byte;
//			//int o_stride = (!dir) ? pitch : byte;
//			int stride = (!alpha) ? (dir ? pitch : byte) : (dir ? Apitch : Abyte);
//			int o_stride = (!alpha) ? ((!dir) ? pitch : byte) : ((!dir) ? Apitch : Abyte);
//
//			for (int i = 0; i < divide; i++) {
//				//边缘处理
//				int t_rsize = dir ? height : width;
//				int t_risize = ((!dir) ? t_height : t_width);
//
//				if (i == divide - 1) {
//					t_risize = ((!dir) ? height : width) - i * t_risize;
//				}
//
//				int offset = i * ((!dir) ? t_height : t_width);
//				//Filter1D(buff + offset * o_stride, t_risize, t_rsize, dir);
//				t_vec.emplace_back(std::thread(Filter1D, buff + offset * o_stride, t_risize, t_rsize, dir, alpha));
//			}
//
//			for (auto& it : t_vec) {
//				it.join();
//			}
//		};
//
//		//multithread(buff, Dir_X);
//		//multithread(buff, Dir_Y);
//
//		multithread(buff, Dir_X, Do_Normal);
//		multithread(buff, Dir_Y, Do_Normal);
//
//		multithread(buff, Dir_X, Do_Alpha);
//		multithread(buff, Dir_Y, Do_Alpha);
//
//		rdPtr->img.UnlockBuffer(buff);
//		if (Alpha) {
//			rdPtr->img.UnlockAlpha();
//		}
//
//		//还原大小
//		ResizedImg.Clone(rdPtr->img);
//
//		rdPtr->img.Delete();
//		rdPtr->img.Create(owidth, oheight, proto);
//
//		ResizedImg.Stretch(rdPtr->img, 0, 0, owidth, oheight, BMODE_OPAQUE, BOP_COPY, 0, STRF_RESAMPLE | STRF_COPYALPHA);
//
//		if (Alpha) {
//			free(Asrc);
//		}
//
//		// Redraw object			
//		ReDisplay(rdPtr);
//	}
//
//	return 0;
//}