#pragma once

#ifdef _DEBUG
// only called when auto login success, for debugging
inline void EOSLoginDebug(LPRDATA rdPtr) {
	//const auto bSuccess = rdPtr->pData->pEOSPresence->SetPresenceSetRawRichText("Nijigasaki",
	//[=] (EOSPresence* pEP) {
	//	rdPtr->pData->pEOSPresence->QueryPresence([=] (EOSPresence* pEP) {
	//		if (pEP->HasPresence()) {
	//			pEP->CopyPresence([=] (EOS_Presence_Info* pInfo) {
	//				OutputDebugStringA(pInfo->RichText);
	//			});
	//		}
	//	});
	//});
}
#endif

inline auto GetAuthPremissions(LPEDATA edPtr) {
	auto permission = EOS_EAuthScopeFlags::EOS_AS_NoFlags;

	if(edPtr->bAuthPremissions_BasicProfile) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_BasicProfile;
	}
	if (edPtr->bAuthPremissions_FriendsList) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_FriendsList;
	}
	if (edPtr->bAuthPremissions_Presence) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_Presence;
	}
	if (edPtr->bAuthPremissions_FriendsManagement) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_FriendsManagement;
	}
	if (edPtr->bAuthPremissions_Email) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_Email;
	}
	if (edPtr->bAuthPremissions_Country) {
		permission |= EOS_EAuthScopeFlags::EOS_AS_Country;
	}

	return permission;
}

inline GlobalData::~GlobalData() {
	EOSAutoLogout();	// no need to use callback to call events here, as app is terminated
	EOSWaitForCallbackComplete();

	EOSReleasePlatform();
	delete pEOSUtilities;	
}

inline bool GlobalData::EOSInit(LPEDATA edPtr) {
	// sdk
	productName = ConvertWStrToStr(edPtr->pAppName);
	productVersion = ConvertWStrToStr(edPtr->pAppVersion);

	EOS_InitializeOptions initOpt{};
	initOpt.ApiVersion = EOS_INITIALIZE_API_LATEST;

	initOpt.ProductName = productName.c_str();
	initOpt.ProductVersion = productVersion.c_str();

	// platform
	productId = ConvertWStrToStr(edPtr->pProductId);

	clientId = ConvertWStrToStr(edPtr->pClientId);
	clientSecret = ConvertWStrToStr(edPtr->pClientSecret);

	sandboxType = edPtr->sandboxType;

	devSandboxId = ConvertWStrToStr(edPtr->pDevSandboxId);
	devDeploymentId = ConvertWStrToStr(edPtr->pDevDeploymentId);
	stageSandboxId = ConvertWStrToStr(edPtr->pStageSandboxId);
	stageDeploymentId = ConvertWStrToStr(edPtr->pStageDeploymentId);
	liveSandboxId = ConvertWStrToStr(edPtr->pLiveSandboxId);
	liveDeploymentId = ConvertWStrToStr(edPtr->pLiveDeploymentId);

	EOS_Platform_Options platOpt{};
	platOpt.ApiVersion = EOS_INITIALIZE_API_LATEST;

	platOpt.ProductId = productId.c_str();

	const auto& [sandboxId, deploymentId] = GetSandboxInfo();

	//MessageBoxA(nullptr,
	//	std::format("CMD\r\nsandbox: {}, deploy: {}\r\ndev sandbox: {}, dev deploy: {}\r\nstage sandbox: {}, stage deploy: {}\r\nlive sandbox: {}, live deploy: {}",sandboxId,deploymentId,devSandboxId,devDeploymentId,stageSandboxId,stageDeploymentId,liveSandboxId,liveDeploymentId).c_str(),
	//	"Title",
	//	MB_OK);

	platOpt.SandboxId = sandboxId.c_str();
	platOpt.DeploymentId = deploymentId.c_str();

	platOpt.ClientCredentials.ClientId = clientId.c_str();
	platOpt.ClientCredentials.ClientSecret = clientSecret.c_str();

	// runtime
	EOSUtilities_RuntimeOptions runtimeOpt{};

	// command line override
	if (StrIEqu(cmdLine.authType.c_str(), "exchangecode")) {
		runtimeOpt.authCredentialsType = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
	}
	else {
		runtimeOpt.authCredentialsType = AuthTypeComboListEnumToLoginCredentialType(edPtr->authType);
	}

	runtimeOpt.authPremissions = GetAuthPremissions(edPtr);
	runtimeOpt.bRequireLauncher = edPtr->bRequireLauncher;
	runtimeOpt.bRequireBootstrap = edPtr->bRequireBootstrap;

	pEOSUtilities = new EOSUtilities(&cmdLine,
		runtimeOpt,
		initOpt,
		platOpt);
	EOSAllocPlatform();

	pEOSUtilities->SetErrorCallback([this] (const std::string& str) {
#ifdef _DEBUG
		OutputDebugStringA(str.c_str());
		OutputDebugStringA("\r\n");
#endif

		CallEvent(ON_Error);		
	});

	return pEOSUtilities->State() == EOSState::InitSuccess;
}