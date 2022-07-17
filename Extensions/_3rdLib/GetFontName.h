#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>

#include <vector>

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

inline FontNames GetFontNameFromFile(LPCWSTR lpszFilePath) {
	FILE* fp = nullptr;
	FontNames fontNames;

	_wfopen_s(&fp, lpszFilePath, L"rb");

	if (fp!=nullptr) {
		TT_OFFSET_TABLE ttOffsetTable;
		fread(&ttOffsetTable, sizeof(TT_OFFSET_TABLE),1,fp);

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
			fread(&tblDir, sizeof(TT_TABLE_DIRECTORY), 1, fp);

			if (_stricmp(std::string(tblDir.szTag, 4).c_str(), "name") == 0) {
				bFound = TRUE;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
		}

		if (bFound) {
			fseek(fp, tblDir.uOffset, SEEK_SET);

			TT_NAME_TABLE_HEADER ttNTHeader;			
			fread(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER), 1, fp);

			ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
			ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);

			TT_NAME_RECORD ttRecord;
			bFound = FALSE;

			for (int i = 0; i < ttNTHeader.uNRCount; i++) {
				fread(&ttRecord, sizeof(TT_NAME_RECORD), 1, fp);

				ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
				if (ttRecord.uNameID == 1) {
					ttRecord.uPlatformID = SWAPWORD(ttRecord.uPlatformID);
					ttRecord.uEncodingID = SWAPWORD(ttRecord.uEncodingID);
					ttRecord.uLanguageID = SWAPWORD(ttRecord.uLanguageID);
					ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
					ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);
					
					int nPos = ftell(fp);
					fseek(fp, tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);

					//bug fix: see the post by SimonSays to read more about it
					auto sz = ttRecord.uStringLength + 1;
					auto lpszNameBuf = new char[2 * sz];
					memset(lpszNameBuf, 0, 2 * sz);

					fread(lpszNameBuf, ttRecord.uStringLength, 1, fp);
					
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

					fseek(fp, nPos,SEEK_SET);
				}
			}
		}

		fclose(fp);
	}

	return fontNames;
}
