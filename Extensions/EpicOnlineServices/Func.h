#pragma once

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
	sandboxId = ConvertWStrToStr(edPtr->pSandboxId);
	deploymentId = ConvertWStrToStr(edPtr->pDeploymentId);

	clientId = ConvertWStrToStr(edPtr->pClientId);
	clientSecret = ConvertWStrToStr(edPtr->pClientSecret);

	EOS_Platform_Options platOpt{};
	platOpt.ApiVersion = EOS_INITIALIZE_API_LATEST;

	platOpt.ProductId = productId.c_str();
	platOpt.SandboxId = sandboxId.c_str();
	platOpt.DeploymentId = deploymentId.c_str();

	platOpt.ClientCredentials.ClientId = clientId.c_str();
	platOpt.ClientCredentials.ClientSecret = clientSecret.c_str();

	// runtime
	EOSUtilities_RuntimeOptions runtimeOpt{};
	runtimeOpt.authCredentialsType = AuthTypeComboListEnumToLoginCredentialType(edPtr->authType);
	runtimeOpt.bRequireLauncher = edPtr->bRequireLauncher;
	runtimeOpt.bRequireBootstrap = edPtr->bRequireBootstrap;

	pEOSUtilities = new EOSUtilities(runtimeOpt, initOpt, platOpt);

	return pEOSUtilities->State() == EOSState::Init;
}