#pragma once

#include <string>

#include "SteamInclude.h"

#include "JsonInterface.h"
#include "SteamHttp.h"

//------------
// Class
//------------

#define USETESTSANDBOX

class SteamMicroTxn :public SteamCallbackClass {
public:
	enum class Step {
		GetUsetInfo_Pending,
		GetUsetInfo_Finish,
		SendRequest_Pending,
		SendRequest_Finish,
		Callback_Pending,
		Callback_Finish,
		Finalize_Pending,
		Finalize_Finish,
	};

	std::string name;
	EHTTPStatusCode code = k_EHTTPStatusCodeInvalid;
	Step step = Step::Finalize_Finish;

	using ErrorCallback = std::function<void(Step, EHTTPStatusCode)>;
	ErrorCallback errorCallback = nullptr;

	using FinishCallback = std::function<void(Step)>;
	FinishCallback finishCallback = nullptr;

private:
	const std::string baseUrl = "https://partner.steam-api.com/";
#ifdef USETESTSANDBOX
	const std::string intf = "ISteamMicroTxnSandbox";
#else
	const std::string intf = "ISteamMicroTxn";
#endif

	SteamHttp* pGetUserInfo = nullptr;
	SteamHttp* pInitTxn = nullptr;
	SteamHttp* pFinalizeTxn = nullptr;

	std::string country;
	std::string currency;

	uint32 m_unAppID = 0;
	uint64 m_ulOrderID = 0;
	uint8 m_bAuthorized = 0;

public:
	std::string transid;
	std::string errorcode;
	std::string errordesc;

	inline void ResetCallbackResult() {
		delete pGetUserInfo;
		pGetUserInfo = nullptr;
		delete pInitTxn;
		pInitTxn = nullptr;
		delete pFinalizeTxn;
		pFinalizeTxn = nullptr;
	}

private:
	inline void CallCallback(void* udata = nullptr) override {
		bCallbackSuccess = false;
		pCallback = GetCallBack<MicroTxnAuthorizationResponse_t>([&] (const MicroTxnAuthorizationResponse_t* pCallback) {
			bCallbackSuccess = true;

			step = Step::Callback_Pending;

			m_unAppID = pCallback->m_unAppID;
			m_ulOrderID = pCallback->m_ulOrderID;
			m_bAuthorized = pCallback->m_bAuthorized;

			if (finishCallback != nullptr) {
				finishCallback(step);
			}

			step = Step::Callback_Finish;
		});
	}

	inline void ParseJson(uint8* pBodyDataBuffer, uint32 unBufferSize,
		const std::function<void(const JsonData& params)>& OKCallback) {
		JsonInterface JI;
		JI.Load(reinterpret_cast<char*>(pBodyDataBuffer));

		const JsonData data = JsonInterface::GetData(JI.Get(), "response");
		const std::string result = JsonInterface::GetData(data, "result");

		if (result == "OK") {
			const JsonData params = JsonInterface::GetData(data, "params");

			OKCallback(params);

			if (finishCallback != nullptr) {
				finishCallback(step);
			}

			return;
		}

		if (result == "Failure") {
			const JsonData error = JsonInterface::GetData(data, "error");

			errorcode = JsonInterface::GetData(error, "errorcode");
			errordesc = JsonInterface::GetData(error, "errordesc");

			if (errorCallback != nullptr) {
				this->code = k_EHTTPStatusCodeInvalid;
				errorCallback(step, code);
			}

			return;
		}
	}

	static inline std::string EncodeUrl(const std::string& in) {		
		static std::vector<std::pair<const char, const char*>> escapeMap = {
			{'%',"%25"}, // must be replaced first
			{'+',"%2B"},
			{' ',"%20"},
			{'/',"%2F"},
			{'?',"%3F"},
			{'#',"%23"},
			{'&',"%26"},
			{'=',"%3D"}
		};

		auto ret = in;

		for (const auto& [chr, rep] : escapeMap) {
			auto pos = ret.find(chr);
			while (pos != std::string::npos) {
				ret = ret.replace(pos, 1, rep);
				pos = ret.find(chr);
			}
		}

		return ret;
	}

public:
	SteamMicroTxn()
		:SteamCallbackClass() {		
		SteamMicroTxn::CallCallback();
	}
	~SteamMicroTxn() override {
		ResetCallbackResult();
	}

private:
	std::string webAPIKey;
	uint64 orderID = 0;
	CSteamID purchaseID;

public:
	inline void SetMicroTxnInfo(const std::string& name,
		const std::string& key, uint64 orderid, CSteamID id = SteamUser()->GetSteamID()) {
		step = Step::Finalize_Finish;

		ResetCallbackResult();

		this->name = name;
		this->webAPIKey = key;
		this->orderID = orderid;
		this->purchaseID = id;
	}

	inline void SetCallback(const ErrorCallback& errorCallback, const FinishCallback& finishCallback) {
		this->errorCallback = errorCallback;
		this->finishCallback = finishCallback;
	}

	inline void GetUserInfo() {
		if(step != Step::Finalize_Finish) {
			return;
		}

		step = Step::GetUsetInfo_Pending;

		const auto param = std::format("?key={}&appid={}&steamid={}", webAPIKey, appID, purchaseID.ConvertToUint64());
		const auto url = std::format("{}{}/GetUserInfo/v2/{}", baseUrl, intf, param);

		pGetUserInfo = new SteamHttp(k_EHTTPMethodGET, url.c_str(),
			[&](EHTTPStatusCode code) {
				if (errorCallback != nullptr) {
					this->code = code;
					errorCallback(step, code);
				}
			},
			[&] (uint8* pBodyDataBuffer, uint32 unBufferSize) {
				ParseJson(pBodyDataBuffer, unBufferSize,
				[&] (const JsonData& params) {
					country = JsonInterface::GetData(params, "country");
					currency = JsonInterface::GetData(params, "currency");
				});

				step = Step::GetUsetInfo_Finish;
			});
	}

	inline void SendRequest(uint32 itemcount, uint32 itemid, int16 qty, int64 amount,
		const std::string& description,
		const std::string& otherParams = std::string()) {
		if (step != Step::GetUsetInfo_Finish) {
			return;
		}

		step = Step::SendRequest_Pending;

		const auto param = std::format("?key={}&orderid={}&steamid={}&appid={}&itemcount={}&language={}&currency={}&itemid[0]={}&qty[0]={}&amount[0]={}&description[0]={}", webAPIKey, orderID, purchaseID.ConvertToUint64(), appID, itemcount, SteamApps()->GetCurrentGameLanguage(), currency, itemid, qty, amount, EncodeUrl(description));
		auto url = std::format("{}{}/InitTxn/v3/{}", baseUrl, intf, param);
		if(!otherParams.empty()) {
			url += "&" + otherParams;
		}

		pInitTxn = new SteamHttp(k_EHTTPMethodPOST, url.c_str(),
			[&] (EHTTPStatusCode code) {
				if (errorCallback != nullptr) {
					this->code = code;
					errorCallback(step, code);
				}
			},
			[&] (uint8* pBodyDataBuffer, uint32 unBufferSize) {
				ParseJson(pBodyDataBuffer, unBufferSize,
				[&] (const JsonData& params) {
					transid = JsonInterface::GetData(params, "transid");
				});

				step = Step::SendRequest_Finish;
			});
	}

	inline void Finalize() {
		if (step != Step::Callback_Finish) {
			return;
		}

		step = Step::Finalize_Pending;

		const auto param = std::format("?key={}&appid={}&orderid={}", webAPIKey, appID, orderID);
		const auto url = std::format("{}{}/FinalizeTxn/v2/{}", baseUrl, intf, param);

		pInitTxn = new SteamHttp(k_EHTTPMethodPOST, url.c_str(),
			[&] (EHTTPStatusCode code) {
				if (errorCallback != nullptr) {
					this->code = code;
					errorCallback(step, code);
				}
			},
			[&] (uint8* pBodyDataBuffer, uint32 unBufferSize) {
				ParseJson(pBodyDataBuffer, unBufferSize,
				[&] (const JsonData& params) {
					transid = JsonInterface::GetData(params, "transid");
				});

				step = Step::Finalize_Finish;
			});
	}
};