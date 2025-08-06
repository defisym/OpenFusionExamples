#pragma once

#include "CLI11/include/CLI/CLI.hpp"
#include "GeneralDefinition.h"

// Example:
// -AUTH_LOGIN=unused -AUTH_PASSWORD=<password> -AUTH_TYPE=exchangecode -epicapp=<appid> -epicenv=Prod -EpicPortal  -epicusername=<username> -epicuserid=<userid> -epiclocale=en-US -epicsandboxid=<sandboxid> -epicdeploymentid=<deploymentid>

// https://eoshelp.epicgames.com/s/case/5004z00001rezlvAAA/login-with-exchange-code?language=zh_CN
// the Launcher will use the equals sign, so any custom parsing that you do should factor this in, for example:
// FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD=")
class EOSCommandLine {
private:
	std::string commandLine;

	static constexpr const char* EOSCommandLine_AuthLogin = "-AUTH_LOGIN";
	static constexpr const char* EOSCommandLine_AuthPassword = "-AUTH_PASSWORD";
	static constexpr const char* EOSCommandLine_AuthType = "-AUTH_TYPE";
	static constexpr const char* EOSCommandLine_EpicApp = "-epicApp";
	static constexpr const char* EOSCommandLine_EpicEnv = "-epicEnv";
	static constexpr const char* EOSCommandLine_EpicPortal = "-epicPortal";
	static constexpr const char* EOSCommandLine_EpicUserName = "-epicUserName";
	static constexpr const char* EOSCommandLine_EpicUserID = "-epicUserID";
	static constexpr const char* EOSCommandLine_EpicLocal = "-epicLocal";
	static constexpr const char* EOSCommandLine_EpicSandboxID = "-epicSandboxID";
	static constexpr const char* EOSCommandLine_EpicDeploymentID = "-epicDeploymentid";

	static constexpr char EOSCommandLine_Dash = '-';
	static constexpr char EOSCommandLine_Equal = '=';
	static constexpr char EOSCommandLine_Space = ' ';

	struct Param {
		std::string item;
		std::string content;
	};

	std::vector<Param> params;

	inline void DispatchParam() {
		const auto& cur = params.back();
		const auto& item = cur.item;

		do {
			if(StrIEqu(item.c_str(), EOSCommandLine_AuthLogin)) {
				authLogin = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_AuthPassword)) {
				authPassword = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_AuthType)) {
				authType = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicApp)) {
				epicApp = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicEnv)) {
				epicEnv = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicPortal)) {
				epicPortal = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicUserName)) {
				epicUserName = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicUserID)) {
				epicUserID = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicLocal)) {
				epicLocal = cur.content;

				break;
			}

			if (StrIEqu(item.c_str(), EOSCommandLine_EpicSandboxID)) {
				epicSandboxID = cur.content;

				break;
			}


			if (StrIEqu(item.c_str(), EOSCommandLine_EpicDeploymentID)) {
				epicDeploymentID = cur.content;

				break;
			}
		} while (false);
	}

public:
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
	std::string epicDeploymentID;

	EOSCommandLine() {
		// get command line
        // CLI11 requires the command line input is UTF-8
        // GetCommandLineA return value is not UTF-8 encoded
        // depending on the locale settings, so convert it here
        // to avoid crash
        const auto utf8CmdLine = to_byte_string(GetCommandLineW());
		const auto& [progname, cli] = CLI::detail::split_program_name(utf8CmdLine);
		commandLine = cli;

		const auto& splitResult = SplitString(commandLine, EOSCommandLine_Space);

		for (auto& it : splitResult) {
			const auto idx = it.find_first_of(EOSCommandLine_Equal);

			if (idx != std::string::npos) {
				params.emplace_back(it.substr(0, idx),
					it.substr(idx + 1));
				DispatchParam();
			}
		}
	}
};
