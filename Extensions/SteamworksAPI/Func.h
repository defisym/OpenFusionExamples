#pragma once

inline void GlobalData::UpdateMicroTxnCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamMicroTxn()->SetCallback(
		[&] (SteamMicroTxn::Step step, EHTTPStatusCode code) {
			CallEvent(OnMixroTxnError);
		},
		[&] (SteamMicroTxn::Step step) {
			//CallEvent(OnMixroTxnFinish);
			AddEvent(OnMixroTxnFinish);
		});
	});
}

inline void GlobalData::UpdateScreenshotCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamScreenshot()->SetCallback(
		[&] () {
			CallEvent(OnScreenshot);
		});
	});
}

inline void GlobalData::UpdateGamepadTextInputCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamGamepadTextInput()->SetCallback(
		[&] (bool bSubmitted, const std::string& text) {
			CallEvent(OnInputDismiss);
		});
	});
}

inline void GlobalData::UpdateSteamDLCCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetSteamDLC()->SetDLCInstallCallback([&] (AppId_t nAppId) {
			rdPtr->callBackAppID = nAppId;
			CallEvent(OnDLCInstalled);
		});
	});
}

inline void GlobalData::UpdateSteamInventoryCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		// full update
		pSteamUtil->GetSteamInv()->SetCallback([&] () {
			CallEvent(OnInventoryFullUpdate);
		});
		// result ready
		pSteamUtil->GetSteamInv()->SetCallback([&] (bool bSuccess) {
			rdPtr->bCallbackSuccess = bSuccess;
			CallEvent(OnInventoryResultReady);
		});
	});
}
