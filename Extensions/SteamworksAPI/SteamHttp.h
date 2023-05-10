#pragma once

#include "SteamInclude.h"

class SteamHttp :public SteamCallbackClass {
private:
	using FinishCallback = std::function<void(uint8*, uint32)>;
	FinishCallback finishCallback = nullptr;

	using ErrorCallback = std::function<void(EHTTPStatusCode)>;
	ErrorCallback errorCallback = nullptr;

	HTTPRequestHandle hRequest = 0;
	uint8* pBodyDataBuffer = nullptr;
	uint32 unBufferSize = 0;

	inline void CallCallback(void* udata = nullptr) override {
		bCallbackSuccess = false;

		SteamAPICall_t hCall = 0;
		bool bSuccess = SteamHTTP()->SendHTTPRequest(hRequest, &hCall);
		pCallback = GetCallBack<HTTPRequestCompleted_t>(hCall,
			[&] (const HTTPRequestCompleted_t* pCallback, bool bIOFailure) {
				bCallbackSuccess = pCallback->m_eStatusCode == k_EHTTPStatusCode200OK;

				if (!bCallbackSuccess) {
					errorCallback(pCallback->m_eStatusCode);

					return;
				}

				SteamHTTP()->GetHTTPResponseBodySize(hRequest, &unBufferSize);

				pBodyDataBuffer = new uint8[unBufferSize + 1];
				pBodyDataBuffer[unBufferSize] = '\0';

				SteamHTTP()->GetHTTPResponseBodyData(hRequest, pBodyDataBuffer, unBufferSize);

				finishCallback(pBodyDataBuffer, unBufferSize);
		});
	}

public:
	SteamHttp(EHTTPMethod eHTTPRequestMethod, const char* pchAbsoluteURL,
		const ErrorCallback& errorCallback,
		const FinishCallback& finishCallback)
		:SteamCallbackClass() {
		this->finishCallback = finishCallback;
		this->errorCallback = errorCallback;
		hRequest = SteamHTTP()->CreateHTTPRequest(eHTTPRequestMethod, pchAbsoluteURL);
		SteamHttp::CallCallback();
	}
	~SteamHttp() override {
		SteamHTTP()->ReleaseHTTPRequest(hRequest);
		delete pBodyDataBuffer;
	}
};