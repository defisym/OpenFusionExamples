#pragma once

inline bool GlobalData::EOSInit(LPEDATA edPtr) {
	// sdk
	productName = ConvertWStrToStr(edPtr->pAppName);
	productVersion = ConvertWStrToStr(edPtr->pAppVersion);

	//TODO remove
	productName = "EOS";
	productVersion = "1.0";

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

	//TODO remove
	productId = "c35399f648ae47a0b5bf42c0320ddb38";
	sandboxId = "59058ad1e73841bf8958085905ed722a";
	deploymentId = "2a67b9cd4bfe46a189e2ab7f727ad7c6";

	clientId = "xyza7891XI9damdLcASUFRDAUrsYMCrg";
	clientSecret = "Lf5I2BKkMLBGgC+iwDIwjd0gIbLRAOrx/2FSmc30ejQ";

	EOS_Platform_Options platOpt{};
	platOpt.ApiVersion = EOS_INITIALIZE_API_LATEST;

	platOpt.ProductId = productId.c_str();
	platOpt.SandboxId = sandboxId.c_str();
	platOpt.DeploymentId = deploymentId.c_str();

	platOpt.ClientCredentials.ClientId = clientId.c_str();
	platOpt.ClientCredentials.ClientSecret = clientSecret.c_str();

	pEOSUtilities = new EOSUtilities(initOpt, platOpt);

	return pEOSUtilities->State();
}