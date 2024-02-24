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

private:
	inline void InitCallback() override {
		SteamAPICall_t hCall = 0;
		bool bSuccess = SteamHTTP()->SendHTTPRequest(hRequest, &hCall);
		AddCallback(GetCallBack<HTTPRequestCompleted_t>(hCall,
			[&] (const HTTPRequestCompleted_t* pCallback, bool bIOFailure) {
				const auto bCallbackSuccess = pCallback->m_eStatusCode == k_EHTTPStatusCode200OK;

				if (!bCallbackSuccess) {
					errorCallback(pCallback->m_eStatusCode);

					return false;
				}

				SteamHTTP()->GetHTTPResponseBodySize(hRequest, &unBufferSize);

				pBodyDataBuffer = new uint8[unBufferSize + 1];
				pBodyDataBuffer[unBufferSize] = '\0';

				SteamHTTP()->GetHTTPResponseBodyData(hRequest, pBodyDataBuffer, unBufferSize);

				finishCallback(pBodyDataBuffer, unBufferSize);

				return true;
			}));
	}

public:
	// Get
	SteamHttp(const char* pchFullURL,
		const ErrorCallback& errorCallback,
		const FinishCallback& finishCallback) {
		this->finishCallback = finishCallback;
		this->errorCallback = errorCallback;
		hRequest = SteamHTTP()->CreateHTTPRequest(k_EHTTPMethodGET, pchFullURL);
		SteamHttp::InitCallback();
	}
	// Post
	SteamHttp(const char* pchAbsoluteURL,
		const char* pchContentType, const char* pubBody, const uint32 unBodyLen,
		const ErrorCallback& errorCallback,
		const FinishCallback& finishCallback) {
		this->finishCallback = finishCallback;
		this->errorCallback = errorCallback;
		hRequest = SteamHTTP()->CreateHTTPRequest(k_EHTTPMethodPOST, pchAbsoluteURL);
		SteamHTTP()->SetHTTPRequestRawPostBody(hRequest, pchContentType, (uint8*)pubBody, unBodyLen);
		SteamHttp::InitCallback();
	}
	~SteamHttp() override {
		SteamHTTP()->ReleaseHTTPRequest(hRequest);
		delete pBodyDataBuffer;
	}
};