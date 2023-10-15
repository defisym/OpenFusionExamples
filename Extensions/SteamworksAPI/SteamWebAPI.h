#pragma once

#include "GeneralDefinition.h"

#include "SteamInclude.h"

class SteamWebAPI {
public:
	static inline const char* GetWebAPILanguage(const char* pLang) {
		if (strcmp(pLang, "arabic") == 0) { return "ar"; }
		if (strcmp(pLang, "bulgarian") == 0) { return "bg"; }
		if (strcmp(pLang, "schinese") == 0) { return "zh-CN"; }
		if (strcmp(pLang, "tchinese") == 0) { return "zh-TW"; }
		if (strcmp(pLang, "czech") == 0) { return "cs"; }
		if (strcmp(pLang, "danish") == 0) { return "da"; }
		if (strcmp(pLang, "dutch") == 0) { return "nl"; }
		if (strcmp(pLang, "english") == 0) { return "en"; }
		if (strcmp(pLang, "finnish") == 0) { return "fi"; }
		if (strcmp(pLang, "french") == 0) { return "fr"; }
		if (strcmp(pLang, "german") == 0) { return "de"; }
		if (strcmp(pLang, "greek") == 0) { return "el"; }
		if (strcmp(pLang, "hungarian") == 0) { return "hu"; }
		if (strcmp(pLang, "italian") == 0) { return "it"; }
		if (strcmp(pLang, "japanese") == 0) { return "ja"; }
		if (strcmp(pLang, "koreana") == 0) { return "ko"; }
		if (strcmp(pLang, "norwegian") == 0) { return "no"; }
		if (strcmp(pLang, "polish") == 0) { return "pl"; }
		if (strcmp(pLang, "portuguese") == 0) { return "pt"; }
		if (strcmp(pLang, "brazilian") == 0) { return "pt-BR"; }
		if (strcmp(pLang, "romanian") == 0) { return "ro"; }
		if (strcmp(pLang, "russian") == 0) { return "ru"; }
		if (strcmp(pLang, "spanish") == 0) { return "es"; }
		if (strcmp(pLang, "latam") == 0) { return "es-419"; }
		if (strcmp(pLang, "swedish") == 0) { return "sv"; }
		if (strcmp(pLang, "thai") == 0) { return "th"; }
		if (strcmp(pLang, "turkish") == 0) { return "tr"; }
		if (strcmp(pLang, "ukrainian") == 0) { return "uk"; }
		if (strcmp(pLang, "vietnamese") == 0) { return "vn"; }

		return "en";
	}
};