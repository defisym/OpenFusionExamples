#pragma once

#include "NineSlice.h"

#include "JsonInterface.h"
#include "JsonUtilities.h"

struct NineSliceInterface {
	LPRDATA rdPtr = nullptr;

	LPRDATA pLib = nullptr;
	std::wstring basePath;
	std::wstring key;

	NineSlice* pNineSlice = nullptr;
	std::wstring fileName;

	bool bReload = true;

	LPSURFACE pRTT = nullptr;
	SurfaceLibValue* pLibValue[9] = { nullptr };

	int minWidth = -1;
	int minHeight = -1;

	explicit NineSliceInterface(const LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}
	~NineSliceInterface() {
		ResetNineSlice();
		ResetRTT();
	}

	inline void SetNineSliceSource(const LPRDATA pLib, const std::wstring& basePath, const std::wstring& key) {
		bReload = true;

		this->pLib = pLib;
		this->basePath = basePath;
		this->key = key;

		if (this->basePath.back() != '\\') {
			this->basePath.push_back('\\');
		}
	}

	inline bool LoadNineSlice(const LPCWSTR pFileName, const LPCTSTR pKey = L"") {
		JsonInterface JI;

		if (!LoadJsonFile(JI, pFileName, pKey)) {
#if !defined(RUN_ONLY)
			MSGBOX(L"Json file load error");
#endif

			return false;
		}

		ResetNineSlice();

		try {
			bReload = true;

			fileName = pFileName;
			pNineSlice = new NineSlice(JI.Get());
		} catch (std::exception& e) {
			const auto pErr = e.what();

#if !defined(RUN_ONLY)
			MSGBOX(ConvertStrToWStr(pErr));
#endif

			return false;
		}

		return true;
	}

	inline void ResetNineSlice() {
		delete pNineSlice;
		pNineSlice = nullptr;

		ResetReference();
	}

	inline void ResetReference() {
		bReload = true;

		for (auto& it : pLibValue) {
			if (it != nullptr) {
				it->UnrefImage(rdPtr);
				it = nullptr;
			}
		}
	}

	inline void ResetRTT() {
		delete pRTT;
		pRTT = nullptr;
	}

	enum class Tile :size_t {
		LT,
		MT,
		RT,
		LM,
		MM,
		RM,
		LB,
		MB,
		RB,
	};

	// return false if not rendered
	inline bool Render(int width, int height) {
		if (pNineSlice == nullptr || pLib == nullptr) {
			return false;
		}

		bool redraw = false;

		if (bReload) {
			ResetReference();

			bReload = false;
			redraw = true;

			size_t index = 0;
			for (auto& it : pNineSlice->GetNineSliceFrameData()->files) {
				const auto curFile = basePath + it;
				const auto loadResult = LoadLib(rdPtr, pLib, curFile.c_str(), key.c_str());
				if (loadResult == pLib->pLib->end()) {
					return false;
				}

				loadResult->second.RefImage(rdPtr);
				pLibValue[index] = &loadResult->second;

				++index;
			}

			minWidth = pLibValue[static_cast<size_t>(Tile::LT)]->pSf->GetWidth()
				+ pLibValue[static_cast<size_t>(Tile::RT)]->pSf->GetWidth();
			minHeight = pLibValue[static_cast<size_t>(Tile::LT)]->pSf->GetHeight()
				+ pLibValue[static_cast<size_t>(Tile::LB)]->pSf->GetHeight();
		}

		width = (std::max)(minWidth, width);
		height = (std::max)(minHeight, height);

		if (pRTT == nullptr
			|| pRTT->GetWidth() != width || pRTT->GetHeight() != height) {
			ResetRTT();

			redraw = true;

			pRTT = CreateHWASurface(rdPtr, 32, width, height, ST_HWA_RTTEXTURE);
			pRTT->CreateAlpha();
		}

		if (!redraw) {
			return false;
		}
		
		auto bRet = true;
		pRTT->BeginRendering(TRUE, 0);

		if (!pNineSlice->GetNineSliceSettings()->stretch) {
			auto draw = [&](size_t index, size_t stride,
				const int& x, const int& y, int& accumulate, int total,
				int(cSurface::* pGetSize)()const,
				const std::function<bool(size_t idx, int destX, int destY, int clip)>& renderer) {
				// calc loop times
				auto getSize = [&] (size_t idx) {
					return (pLibValue[idx]->pSf->*pGetSize)();
				};

				const auto loopSize = total - getSize(index + 0 * stride) - getSize(index + 2 * stride);
				const auto scale = 1.0 * loopSize / getSize(index + 1 * stride);
				const auto loopTimes = static_cast<int>(floor(scale));
				const bool bExtra = loopTimes != static_cast<int>(ceil(scale));

				bool bRet = true;

				bRet &= renderer(index + 0 * stride, x, y, getSize(index + 0 * stride));
				accumulate += getSize(index + 0 * stride);

				for (int i = 0; i < loopTimes; i++) {
					bRet &= renderer(index + 1 * stride, x, y, getSize(index + 0 * stride));
					accumulate += getSize(index + 1 * stride);
				}

				if (bExtra) {
					const auto clip = loopSize - (accumulate - getSize(index + 0 * stride));
					bRet &= renderer(index + 1 * stride, x, y, clip);
					accumulate += clip;
				}

				bRet &= renderer(index + 2 * stride, x, y, getSize(index + 0 * stride));

				return bRet;
			};

			auto drawHorizentally = [&] (size_t index, int x, int y, int clipY) {
				return draw(index, 1,
					x, y, x, width,
					&cSurface::GetWidth,
					[&] (size_t idx, int destX, int destY, int clip) {
					return pLibValue[idx]->pSf->Blit(*pRTT, destX, destY,
					0, 0, clip, clipY, BMODE_OPAQUE) == TRUE;
					});
			};

			auto drawVertically = [&] (size_t index, int x, int y) {
				return draw(index, 3,
					x, y, y, height,
					&cSurface::GetHeight,
					[&] (size_t idx, int destX, int destY, int clip) {
					return drawHorizentally(idx, destX, destY, clip);
					});
			};

			bRet = drawVertically(0, 0, 0);
		}
		else {
			const auto stretchWidth = width - minWidth;
			const auto stretchHeight = height - minHeight;

			auto drawHorizentally = [&] (size_t index, int y, int h) {
				bool bRet = true;

				bRet &= pLibValue[index + 0]->pSf->Stretch(*pRTT,
					0, y,
					pLibValue[index + 0]->pSf->GetWidth(), h,
					BMODE_OPAQUE) == TRUE;

				if (stretchWidth != 0) {
					bRet &= pLibValue[index + 1]->pSf->Stretch(*pRTT,
						pLibValue[index + 0]->pSf->GetWidth(), y,
						stretchWidth, h,
						BMODE_OPAQUE) == TRUE;
				}

				bRet &= pLibValue[index + 2]->pSf->Stretch(*pRTT,
					width - pLibValue[index + 2]->pSf->GetWidth(), y,
					pLibValue[index + 2]->pSf->GetWidth(), h,
					BMODE_OPAQUE) == TRUE;

				return bRet;
			};

			bRet &= drawHorizentally(static_cast<size_t>(Tile::LT),
				0,
				pLibValue[static_cast<size_t>(Tile::LT)]->pSf->GetHeight());

			if (stretchHeight != 0) {
				bRet &= drawHorizentally(static_cast<size_t>(Tile::LM),
					pLibValue[static_cast<size_t>(Tile::LT)]->pSf->GetHeight(),
					stretchHeight);
			}

			bRet &= drawHorizentally(static_cast<size_t>(Tile::LB),
				height - pLibValue[static_cast<size_t>(Tile::LB)]->pSf->GetHeight(),
				pLibValue[static_cast<size_t>(Tile::LB)]->pSf->GetHeight());
		}

		pRTT->EndRendering();

#ifdef _DEBUG
		//__SavetoFile(rdPtr, pRTT, L"F:\\9Slice.png");
		//OutputDebugString(L"Redraw\r\n");
#endif

		return bRet;
	}

	inline void Render() {
		if (!CanDisplay(rdPtr)) {
			return;
		}

		// cast to float to avoid precision loss
		const auto width = static_cast<int>(abs(static_cast<float>(rdPtr->src->GetWidth()) * rdPtr->zoomScale.XScale));
		const auto height = static_cast<int>(abs(static_cast<float>(rdPtr->src->GetHeight()) * rdPtr->zoomScale.YScale));

		if (Render(width, height)) {
			ReleaseNonFromLib(rdPtr);
			NewNonFromLib(rdPtr, CreateCloneSurface(rdPtr, pRTT));

			// don't reset stretch if it's already the min size
			auto resetScale = [] (const int wantedSz, const int renderedSz, const int minSz, float& scale) {
				if (renderedSz != minSz) {		// sz>=minSz, granteed by Render
					scale = scale / abs(scale);
				}else {
					scale = static_cast<float>(wantedSz) / static_cast<float>(minSz);
				}
			};

			resetScale(width, pRTT->GetWidth(), minWidth, rdPtr->zoomScale.XScale);
			resetScale(height,pRTT->GetHeight(), minHeight, rdPtr->zoomScale.YScale);

			// update hotspot
			ReDisplay(rdPtr);

			rdPtr->hotSpot.x = rdPtr->rHo.hoImgXSpot;
			rdPtr->hotSpot.y = rdPtr->rHo.hoImgYSpot;
		}
	}
};