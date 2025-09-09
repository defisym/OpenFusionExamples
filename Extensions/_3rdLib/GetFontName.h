#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#include <stdio.h>
#include <string>

#include <vector>

#define _FUSION_EMBED

#ifdef _FUSION_EMBED
#include "CfcFile.h"
#endif

//https://www.i4k.xyz/article/weixin_45513192/115480892

typedef struct _tagTT_OFFSET_TABLE {
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
}TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY {
	char	szTag[4];			//table name
	ULONG	uCheckSum;			//Check sum
	ULONG	uOffset;			//Offset from beginning of file
	ULONG	uLength;			//length of the table in bytes
}TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER {
	USHORT	uFSelector;			//format selector. Always 0
	USHORT	uNRCount;			//Name Records count
	USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
}TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD {
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
}TT_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

using FontNames = std::vector<std::wstring>;

#ifdef _FUSION_EMBED
// if memBufSz == 0 then read from disk, or lpszFilePath is a pointer to memory buffer
inline FontNames GetFontNameFromFile(LPCWSTR lpszFilePath, size_t memBufSz = 0) {
#else
inline FontNames GetFontNameFromFile(LPCWSTR lpszFilePath) {
#endif
	FontNames fontNames;

#ifdef _FUSION_EMBED
	CInputFile* fp = nullptr;

	if (memBufSz!=0) {
		fp = new CInputMemFile;
		((CInputMemFile*)fp)->Create((BYTE*)lpszFilePath, memBufSz);
	}
	else {
		fp = new CInputBufFile;
		((CInputBufFile*)fp)->Create(lpszFilePath);
	}
#else
	FILE* fp = nullptr;
	_wfopen_s(&fp, lpszFilePath, L"rb");
#endif
	
	if (fp != nullptr) {
		TT_OFFSET_TABLE ttOffsetTable;
#ifdef _FUSION_EMBED
		fp->Read((BYTE*)(&ttOffsetTable), sizeof(TT_OFFSET_TABLE));
#else
		fread(&ttOffsetTable, sizeof(TT_OFFSET_TABLE), 1, fp);
#endif		

		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0) {
			return fontNames;
		}

		TT_TABLE_DIRECTORY tblDir;
		BOOL bFound = FALSE;

		std::string szTag;

		for (int i = 0; i < ttOffsetTable.uNumOfTables; i++) {
#ifdef _FUSION_EMBED
			fp->Read((BYTE*)(&tblDir), sizeof(TT_TABLE_DIRECTORY));
#else
			fread(&tblDir, sizeof(TT_TABLE_DIRECTORY), 1, fp);
#endif			

			if (_stricmp(std::string(tblDir.szTag, 4).c_str(), "name") == 0) {
				bFound = TRUE;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
		}

		if (bFound) {
#ifdef _FUSION_EMBED
			fp->Seek(tblDir.uOffset, SEEK_SET);
#else
			fseek(fp, tblDir.uOffset, SEEK_SET);
#endif			

			TT_NAME_TABLE_HEADER ttNTHeader;
#ifdef _FUSION_EMBED
			fp->Read((BYTE*)(&ttNTHeader), sizeof(TT_NAME_TABLE_HEADER));
#else
			fread(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER), 1, fp);
#endif
		

			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);

			TT_NAME_RECORD ttRecord;
			bFound = FALSE;

			for (int i = 0; i < ttNTHeader.uNRCount; i++) {
#ifdef _FUSION_EMBED
				fp->Read((BYTE*)(&ttRecord), sizeof(TT_NAME_RECORD));
#else
				fread(&ttRecord, sizeof(TT_NAME_RECORD), 1, fp);
#endif				

				ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
				if (ttRecord.uNameID == 1) {
					ttRecord.uPlatformID = SWAPWORD(ttRecord.uPlatformID);
					ttRecord.uEncodingID = SWAPWORD(ttRecord.uEncodingID);
					ttRecord.uLanguageID = SWAPWORD(ttRecord.uLanguageID);
					ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
					ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);
					
#ifdef _FUSION_EMBED
					int nPos = fp->GetPosition();
					fp->Seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);
#else
					int nPos = ftell(fp);
					fseek(fp, tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);
#endif					

					//bug fix: see the post by SimonSays to read more about it
					auto sz = ttRecord.uStringLength + 1;
					auto lpszNameBuf = new char[2 * sz];
					memset(lpszNameBuf, 0, 2 * sz);

#ifdef _FUSION_EMBED
					fp->Read((BYTE*)lpszNameBuf, ttRecord.uStringLength);
#else
					fread(lpszNameBuf, ttRecord.uStringLength, 1, fp);
#endif					
					
					//https://gist.github.com/honood/3851840
					// 用 uint16_t 是因为 Linux 的 wchar_t 为4字节
					// 而 TTF 文件中的信息是以2字节 wchar_t 记录的
					if (ttRecord.uPlatformID == 3) {						
						// 将 pbuf 转换为固定2字节 wchar_t 的字符串指针
						uint16_t* pwch = (uint16_t*)lpszNameBuf;
						// 计算2字节 wchar_t 的字符个数（原因同上）
						uint16_t cwch = ttRecord.uStringLength / sizeof(uint16_t);
						uint16_t wSz = cwch + 1;
						// 为与平台相关的目标 wchar_t 指针分配空间
						wchar_t* pwstr=new wchar_t[wSz];
						memset(pwstr, 0, wSz * sizeof(wchar_t));

						if (pwstr != NULL) {
							for (uint32_t iwch = 0; iwch < cwch; iwch++) {
								// 将固定2字节的 wchar_t 转换字节序后
								// 赋值给与平台相关的 wchar_t 字符数组
								pwstr[iwch] = SWAPWORD(*pwch);
								pwch++;
							}
							//wcstombs(lpszNameBuf, pwstr, (ttRecord.uStringLength + sizeof(char)) * 2);
							
							fontNames.emplace_back(pwstr);

							delete[] pwstr;
						}
					}

					delete[] lpszNameBuf;

#ifdef _FUSION_EMBED
					fp->Seek(nPos, SEEK_SET);
#else
					fseek(fp, nPos, SEEK_SET);
#endif
					
				}
			}
		}

#ifdef _FUSION_EMBED
		delete fp;
#else
		fclose(fp);
#endif		
	}

	return fontNames;
}
