#pragma once

#include <vector>
#include <functional>

#include <iostream>

#include <Windows.h>

namespace FusionClipboard {
	constexpr auto MMFFRMED = 49987;
	constexpr auto MMFFRMLDDR = 50112;	    
	constexpr auto MMF2IMAGELIST = 50021;

	constexpr auto CF_CNCV25_EVENTS = 50322;	    
	constexpr auto CF_CNCV25_CONDITIONS = 50323;
	constexpr auto CF_CNCV25_ACTIONS = 50324;

	constexpr auto CONDSZ = 16;	    
	constexpr auto ACTSZ = 14;	    

	class FusionClipboard {
	private:
		struct ClipboardData {
			std::wstring formatStr;

			UINT uFormat = 0;

			BYTE* pBuf = nullptr;
			size_t sz = 0;

			inline void AllocBuffer(size_t size) {
				sz = size;
				pBuf = new BYTE[sz];
				memset(pBuf, 0, sz);
			}

			ClipboardData(UINT uFormat) {
				if (!IsClipboardFormatAvailable(uFormat)) {
					throw std::exception("Format Not Available");
				}

				//if (!OpenClipboard(nullptr)) {
				//	throw std::exception("Cannot Open Clipboard");
				//}

				this->uFormat = uFormat;

				const HANDLE handle = GetClipboardData(uFormat);
				const auto pData = static_cast<BYTE*>(GlobalLock(handle));

				AllocBuffer(GlobalSize(handle));
				memcpy(pBuf, pData, sz);

				formatStr = std::wstring(80, 0);
				GetClipboardFormatName(uFormat, &formatStr[0], formatStr.capacity());

				GlobalUnlock(handle);
				//CloseClipboard();
			}

			ClipboardData(FILE* fp) {
				fread(&sz, sizeof(size_t), 1, fp);

				size_t len = 0;
				fread(&len, sizeof(size_t), 1, fp);

				formatStr = std::wstring(len, 0);
				fread(&formatStr[0], len * sizeof(wchar_t), 1, fp);
				fread(&uFormat, sizeof(UINT), 1, fp);

				if (!formatStr.empty()) {
					uFormat = RegisterClipboardFormat(formatStr.c_str());
				}

				AllocBuffer(sz);
				fread(pBuf, sz, 1, fp);
			}

			~ClipboardData() {
				uFormat = 0;

				delete[] pBuf;
				pBuf = nullptr;

				sz = 0;
			}

			//inline bool SaveToClipboard(COleDataSource* pData) const {
			//	HGLOBAL cb = GlobalAlloc(GMEM_FIXED, sz);
			//	const auto pCb = static_cast<BYTE*>(GlobalLock(cb));

			//	memcpy(pCb, pBuf, sz);

			//	GlobalUnlock(cb);

			//	pData->CacheGlobalData(static_cast<CLIPFORMAT>(uFormat), cb);

			//	return true;
			//}

			inline bool SaveToClipboard() const {
				if (!OpenClipboard(nullptr)) {
					return false;
				}

				EmptyClipboard();

				const HGLOBAL cb = GlobalAlloc(GMEM_MOVEABLE, sz);
				const auto pData = static_cast<BYTE*>(GlobalLock(cb));

				memcpy(pData, pBuf, sz);

				SetClipboardData(uFormat, pData);

				GlobalUnlock(cb);
				CloseClipboard();

				return true;
			}
		};

		std::vector<ClipboardData*> pClipboardDatas;
		std::string err;

		static inline bool DataAvailable(UINT uFormat) {
			return IsClipboardFormatAvailable(uFormat);
		}

		inline ClipboardData* SaveData(const UINT uFormat) {
			try {
				return new ClipboardData(uFormat);
			}catch(std::exception& e) {
				err = e.what();
				return nullptr;
			}
		}

		static inline bool LoadData(const ClipboardData* pClipboardData) {
			return pClipboardData->SaveToClipboard();			
		}

		inline void IterateClipboardFormat(const std::function<void(UINT uFormat)>& callback) {
			// If there are no clipboard formats, return. 
			if (CountClipboardFormats() == 0) {
				err = "No Format Avaliable";
				return;
			}

			// Open the clipboard. 
			if (!OpenClipboard(nullptr)) {
				err = "Cannot Open Clipboard";
				return;
			}

			// Add a separator and then a menu item for each format. 
			auto uFormat = EnumClipboardFormats(0);

			while (uFormat) {
				callback(uFormat);

				uFormat = EnumClipboardFormats(uFormat);
			}

			CloseClipboard();
		}

		inline void EraseDataVector() {
			for (auto& it : pClipboardDatas) {
				delete it;
				it = nullptr;
			}

			pClipboardDatas.clear();
		}

	public:
		~FusionClipboard() {
			EraseDataVector();
		}

		inline void PrintFormats() {
			IterateClipboardFormat([] (UINT uFormat) {
				LPCWSTR lpFormatName;
				wchar_t szFormatName[80];

				// Note that, if the format name is larger than the
				// buffer, it is truncated. 
				if (GetClipboardFormatName(uFormat, szFormatName,
					sizeof(szFormatName))) {
					lpFormatName = szFormatName;
				}
				else {
					lpFormatName = L"(unknown)";
				}

				std::wcout << uFormat << L"    " << lpFormatName << std::endl;
			});			
		}

		inline UINT GetFormat(const std::wstring& str) {
			UINT required = -1;

			IterateClipboardFormat([&] (UINT uFormat) {
				wchar_t szFormatName[80];

				// Note that, if the format name is larger than the
				// buffer, it is truncated. 
				if (GetClipboardFormatName(uFormat, szFormatName,
					sizeof(szFormatName))) {
					if(str == szFormatName) {
						required = uFormat;
					}
				}
			});

			if (required == -1) {
				required = RegisterClipboardFormat(str.c_str());
			}

			return required;
		}

		inline bool LoadFromFile(const std::wstring& fileName, UINT requiredFormat) {
			FILE* fp = nullptr;
			_wfopen_s(&fp, fileName.c_str(), L"rb");

			if (fp == nullptr) {
				err = "Cannot Open File";
				return false;
			}

			bool bRet = true;

			do {
				EraseDataVector();

				fseek(fp, 0, SEEK_END);
				const auto fSz = ftell(fp);
				rewind(fp);

				// sz
				size_t sz = 0;
				fread(&sz, sizeof(size_t), 1, fp);

				if (sz == 0) {
					err = "Invalid File";
					bRet = false;
					break;
				}

				// offsets
				std::vector<size_t> offsets;
				for (size_t i = 0; i < sz; i++) {
					size_t offset = 0;
					fread(&offset, sizeof(size_t), 1, fp);
					offsets.push_back(offset);
				}

				if (offsets.empty()) {
					err = "Invalid File";
					bRet = false;
					break;
				}

				EmptyClipboard();

				for (const auto& it : offsets) {
					try {
						const auto data = ClipboardData(fp);
						if (data.uFormat == requiredFormat) {
							data.SaveToClipboard();
						}
					} catch (std::exception& e) {
						err = e.what();

						return false;
					}
				}
			} while (false);

			fclose(fp);

			return bRet;
		}
				
		inline bool SaveToFile(const std::wstring& fileName, UINT requiredFormat = 0) {
			FILE* fp = nullptr;
			_wfopen_s(&fp, fileName.c_str(), L"wb");

			if (fp == nullptr) {
				err = "Cannot Open File";
				return false;
			}

			bool bRet = true;

			do {
				if (requiredFormat != 0 && !DataAvailable(requiredFormat)) {
					err = "Required Format Not Available";
					bRet = false;
					break;
				}

				EraseDataVector();

				IterateClipboardFormat([&] (UINT uFormat) {
					const auto pData = SaveData(uFormat);

					if (pData == nullptr) {
						return;
					}

					pClipboardDatas.push_back(pData);
				});

				if (pClipboardDatas.empty()) {
					err = "No Valid Data";
					bRet = false;
					break;
				}

				// sz
				const auto sz = pClipboardDatas.size();
				fwrite(&sz, sizeof(size_t), 1, fp);

				// offsets
				size_t offset = sizeof(size_t);       // size
				offset += sz * sizeof(size_t);        // item offsets

				std::vector<size_t> offsets;
				offsets.push_back(offset);

				for (const auto& it : pClipboardDatas) {					
					offset += sizeof(size_t) + sizeof(UINT) + it->sz;
					offsets.push_back(offset);
				}

				offsets.pop_back();
				for (const auto& it : offsets) {
					fwrite(&it, sizeof(size_t), 1, fp);
				}

				// items
				for (const auto& it : pClipboardDatas) {
					fwrite(&it->sz, sizeof(size_t), 1, fp);

					const auto len = wcslen(it->formatStr.c_str());
					fwrite(&len, sizeof(size_t), 1, fp);
					fwrite(it->formatStr.c_str(), len * sizeof(wchar_t), 1, fp);
					fwrite(&it->uFormat, sizeof(UINT), 1, fp);

					fwrite(it->pBuf, it->sz, 1, fp);
				}
			} while (false);

			fclose(fp);

			return bRet;
		}

		inline const std::string& GetErrorInfo() {
			return err;
		}
	};
}
