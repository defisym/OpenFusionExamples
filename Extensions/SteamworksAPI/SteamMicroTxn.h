#pragma once

#include <string>

//#define CPPHTTPLIB_OPENSSL_SUPPORT
//#include <httplib.h>
//#include <Windows.h>

#ifdef _DEBUG
#pragma comment(lib,"libssl32MDd.lib")
#pragma comment(lib,"libcrypto32MDd.lib")
#else
#pragma comment(lib,"libssl32MD.lib")
#pragma comment(lib,"libcrypto32MD.lib")
#endif

#include "SteamInclude.h"

//------------
// Class
//------------

class SteamMicroTxn :public SteamCallbackClass {
private:
	const std::string baseUrl = "https://partner.steam-api.com/";
#ifdef _DEBUG
	const std::string intf = "ISteamMicroTxnSandbox";
#else
	const std::string intf = "ISteamMicroTxn";
#endif

	httplib::SSLClient cli = httplib::SSLClient(baseUrl + intf, 443);

	inline void CallCallback(void* udata = nullptr) override {
		bCallbackSuccess = false;
		pCallback = GetCallBack<MicroTxnAuthorizationResponse_t>([&] (const MicroTxnAuthorizationResponse_t* pCallback) {
			//todo
		});
	}
public:
	SteamMicroTxn()
		:SteamCallbackClass() {
		cli.set_connection_timeout(0, 300000); // 300 milliseconds
		cli.set_read_timeout(5, 0); // 5 seconds
		cli.set_write_timeout(5, 0); // 5 seconds

		SteamMicroTxn::CallCallback();
	}
	~SteamMicroTxn() override {

	}

	inline void GetUserInfo() {
		
	}

	inline void SendRequest() {
		
	}
};