#pragma once

inline void GlobalData::UpdateMicroTxnCallback() const {
	GetSteamUtilities([&] (const SteamUtilities* pSteamUtil) {
		pSteamUtil->GetMicroTxn()->SetCallback(
		[&] (SteamMicroTxn::Step step, EHTTPStatusCode code) {
			CallEvent(OnMixroTxnError);
		},
		[&] (SteamMicroTxn::Step step) {
			CallEvent(OnMixroTxnFinish);
		});
});
}
