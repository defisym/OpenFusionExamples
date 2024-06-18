#pragma once

//immediate conditon ID
constexpr auto OnMixroTxnError = 1;
constexpr auto OnMixroTxnFinish = 2;
constexpr auto OnScreenshot = 4;
constexpr auto OnInputDismiss = 5;
constexpr auto OnDLCInstalled = 7;

class SteamUtilities;

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

struct GlobalData {
	LPRDATA rdPtr = nullptr;

	bool bInit = false;
	SteamUtilities* pSteamUtil = nullptr;

	GlobalData() {
		bInit = steamInit.bInit;

		if (bInit) {
			pSteamUtil = new SteamUtilities();
		}
	}
	~GlobalData() {
		delete pSteamUtil;
		pSteamUtil = nullptr;
	}

	inline bool SteamUtilitiesValid() const {
		return pSteamUtil != nullptr;
	}

	// rdPtr->pData->GetSteamUtilities([] (SteamUtilities* pSteamUtil) {});
	// wrapper function, only call callback if steam init successfully
	inline void GetSteamUtilities(const std::function<void(SteamUtilities* pSteamUtil)>& callback,
		const std::function<bool(SteamUtilities* pSteamUtil)>& extraCond = nullptr) const {
		if (!SteamUtilitiesValid()) { return; }

		const bool bExtra = extraCond != nullptr
			? extraCond(pSteamUtil)
			: true;
		if (!bExtra) { return; }

		callback(pSteamUtil);
	}

	// wrapper function, only call callback if steam init successfully & extra condition is true
	template<typename T>
	inline T GetSteamUtilities(const T defaultValue,
		const std::function<T(SteamUtilities* pSteamUtil)>& callback,
		const std::function<bool(SteamUtilities* pSteamUtil)>& extraCond = nullptr) const {
		if (!SteamUtilitiesValid()){ return defaultValue; }

		const bool bExtra = extraCond != nullptr
			? extraCond(pSteamUtil)
			: true;
		if (!bExtra) { return defaultValue; }

		return callback(pSteamUtil);
	}

private:
	inline void UpdateRdPtr(LPRDATA rdPtr) {
		this->rdPtr = rdPtr;
	}

	inline void UpdateMicroTxnCallback() const;
	inline void UpdateScreenshotCallback() const;
	inline void UpdateGamepadTextInputCallback() const;
	inline void UpdateSteamDLCCallback() const;

public:
	// update rdPtr when changing frame
	inline void Update(LPRDATA rdPtr) {
		UpdateRdPtr(rdPtr);
		UpdateMicroTxnCallback();
		UpdateScreenshotCallback();
		UpdateGamepadTextInputCallback();
		UpdateSteamDLCCallback();
	}
};