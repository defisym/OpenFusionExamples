#pragma once

#include "CLI11.hpp"

// Example:
// -AUTH_LOGIN=unused -AUTH_PASSWORD=<password> -AUTH_TYPE=exchangecode -epicapp=<appid> -epicenv=Prod -EpicPortal  -epicusername=<username> -epicuserid=<userid> -epiclocale=en-US -epicsandboxid=<sandboxid>
// -AUTH_LOGIN=unused -AUTH_PASSWORD=<password> -AUTH_TYPE=exchangecode -epicapp=<appid> -epicenv=Prod -EpicPortal -epicusername=<username>-epicuserid<userid> -epiclocale=en-US

// https://eoshelp.epicgames.com/s/case/5004z00001rezlvAAA/login-with-exchange-code?language=zh_CN
// the Launcher will use the equals sign, so any custom parsing that you do should factor this in, for example:
// FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD=")

namespace EOSCommandLineParam {
	constexpr const char* EOSCommandLine_AuthLogin = "-AUTH_LOGIN";
	constexpr const char* EOSCommandLine_AuthPassword = "-AUTH_PASSWORD";
	constexpr const char* EOSCommandLine_AuthType = "-AUTH_TYPE";
	constexpr const char* EOSCommandLine_EpicApp = "-epicApp";
	constexpr const char* EOSCommandLine_EpicEnv = "-epicEnv";
	constexpr const char* EOSCommandLine_EpicPortal = "-epicPortal";
	constexpr const char* EOSCommandLine_EpicUserName = "-epicUserName";
	constexpr const char* EOSCommandLine_EpicUserID = "-epicUserID";
	constexpr const char* EOSCommandLine_EpicLocal = "-epicLocal";
	constexpr const char* EOSCommandLine_EpicSandboxID = "-epicSandboxID";

	// https://github.com/CLIUtils/CLI11#features-not-supported-by-this-library
	// Non-standard variations on syntax, like -long options. This is non-standard and should be avoided, so that is enforced by this library.
	// So pre process it to make it compatible

	constexpr const char EOSCommandLine_Equal = '=';
	constexpr const char EOSCommandLine_Dash = '-';
	constexpr const char EOSCommandLine_Space = ' ';

	// add '-' before params, replace '=' after it to ' '
	constexpr auto GetCompatibleEOSCommandLine(const char* pCommandLine, bool bNameIncluded) {
		std::string commandLine;

		if (bNameIncluded) {
			const auto vals = CLI::detail::split_program_name(pCommandLine);
			commandLine = vals.second;
		}
		else {
			commandLine = pCommandLine;
		}

		std::string compatibleCommandLine;

		bool bInParam = false;
		bool bEquReplaced = false;

		for (const auto& chr : commandLine) {
			if (!bInParam && chr == EOSCommandLine_Dash) {
				compatibleCommandLine.push_back(chr);
				bInParam = true;
				bEquReplaced = false;
			}

			if (bInParam && chr == EOSCommandLine_Space) {
				bInParam = false;
				bEquReplaced = false;
			}

			// only replace the first '='
			if (bInParam && !bEquReplaced && chr == EOSCommandLine_Equal) {
				compatibleCommandLine.push_back(EOSCommandLine_Space);
				bEquReplaced = true;
				continue;
			}

			compatibleCommandLine.push_back(chr);
		}

		return compatibleCommandLine;
	}

	// add '-' before params
	inline auto GetCompatibleEOSCommandLineParam(const char* pParam) {
		return std::format("{}{}", EOSCommandLine_Dash, pParam);
	}
}

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
		//app.allow_windows_style_options();

		try {
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_AuthLogin), authLogin)
			   //->required()
			   ->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_AuthPassword), authPassword)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_AuthType), authType)
				->required()
				->ignore_case();

			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicApp), epicApp)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicEnv), epicEnv)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicPortal), epicPortal)
				//->required()
				->ignore_case();

			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicUserName),epicUserName)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicUserID), epicUserID)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicLocal), epicLocal)
				//->required()
				->ignore_case();
			app.add_option(EOSCommandLineParam::GetCompatibleEOSCommandLineParam(EOSCommandLineParam::EOSCommandLine_EpicSandboxID), epicSandboxID)
				//->required()
				->ignore_case();
			
		} catch (const CLI::ConstructionError& e) {
			std::cout << e.get_name() << std::endl;
			bValid = false;

			return;
		}
		
		try {
			const auto compatibleCommandLine = EOSCommandLineParam::GetCompatibleEOSCommandLine(GetCommandLineA(), true);
			// GetCompatibleEOSCommandLine will remove program name part
			app.parse(compatibleCommandLine, false);
		} catch (const CLI::ParseError& e) {
			std::cout << e.get_name() << std::endl;
			bValid = false;

			return;
		}
	}
};