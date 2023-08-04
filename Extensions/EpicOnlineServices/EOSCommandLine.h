#pragma once

#include "CLI11.hpp"

// -AUTH_LOGIN=unused -AUTH_PASSWORD=<password> -AUTH_TYPE=exchangecode -epicapp=<appid> -epicenv=Prod -EpicPortal  -epicusername=<username> -epicuserid=<userid> -epiclocale=en-US -epicsandboxid=<sandboxid>
// -AUTH_LOGIN=unused -AUTH_PASSWORD=<password> -AUTH_TYPE=exchangecode -epicapp=<appid> -epicenv=Prod -EpicPortal -epicusername=<username>-epicuserid<userid> -epiclocale=en-US

// https://eoshelp.epicgames.com/s/case/5004z00001rezlvAAA/login-with-exchange-code?language=zh_CN
// the Launcher will use the equals sign, so any custom parsing that you do should factor this in, for example:
// FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD=")

class EOSCommandLine {
	CLI::App app;

public:
	bool bValid = true;

	std::string authLogin;
	std::string authPassword;
	std::string authType;

	std::string epicApp;
	std::string epicEnv;
	std::string epicPortal;

	std::string epicUserName;
	std::string epicUserID;
	std::string epicLocal;
	std::string epicSandboxID;

	EOSCommandLine() {
		app.allow_windows_style_options();

		try {
			app.add_option("-AUTH_LOGIN", authLogin)
				//->required()
				->ignore_case();
			app.add_option("-AUTH_PASSWORD", authPassword)
				//->required()
				->ignore_case();
			app.add_option("-AUTH_TYPE", authType)
				->required()
				->ignore_case();

			app.add_option("-epicApp", epicApp)
				//->required()
				->ignore_case();
			app.add_option("-epicEnv", epicEnv)
				//->required()
				->ignore_case();
			app.add_option("-epicPortal", epicPortal)
				//->required()
				->ignore_case();

			app.add_option("-epicUserName", epicUserName)
				//->required()
				->ignore_case();
			app.add_option("-epicUserID", epicUserID)
				//->required()
				->ignore_case();
			app.add_option("-epicLocal", epicLocal)
				//->required()
				->ignore_case();
			app.add_option("-epicSandboxID", epicSandboxID)
				//->required()
				->ignore_case();
			
		} catch (const CLI::ConstructionError& e) {
			std::cout << e.get_name() << std::endl;
			bValid = false;

			return;
		}
		
		try {
			app.parse(GetCommandLineA(),true);
		} catch (const CLI::ParseError& e) {
			std::cout << e.get_name() << std::endl;
			bValid = false;

			return;
		}
	}
};