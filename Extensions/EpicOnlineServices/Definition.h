#pragma once

//immediate conditon ID
//constexpr auto ON_FINISH = 4;

// size of edit data
constexpr auto EOS_IDSZ = 36;

class EOSUtilities;

struct tagEDATA_V1;
typedef tagEDATA_V1 EDITDATA;
typedef EDITDATA* LPEDATA;

struct tagRDATA;
typedef tagRDATA RUNDATA;
typedef RUNDATA* LPRDATA;

struct GlobalData {
	LPRDATA rdPtr = nullptr;

	EOSUtilities* pEOSUtilities = nullptr;

	std::string productName;
	std::string productVersion;

	std::string productId;
	std::string sandboxId;
	std::string deploymentId;

	std::string clientId;
	std::string clientSecret;

	GlobalData() {
	}
	~GlobalData() {
		delete pEOSUtilities;
	}

	inline bool EOSInit(LPEDATA edPtr);
};