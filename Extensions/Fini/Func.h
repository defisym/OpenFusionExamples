#ifndef _FUNC_
#define _FUNC_

#include	<locale>
#include	<codecvt>

// convert string to wstring
inline std::wstring to_wide_string(const std::string& input) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}

// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}

inline void NewStr(LPTSTR & Tar, LPCTSTR Str) {
	release_arr(Tar);
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

inline void NewStr(LPTSTR& Tar, const std::wstring& Str) {
	NewStr(Tar, Str.c_str());
}

//convert string to double, 5X faster than std::stod
inline double _stod(const wchar_t* p) {
	double r = 0.0;
	bool neg = false;

	if (*p == L'-') {
		neg = true;
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		r = (r * 10.0) + (*p - L'0');
		++p;
	}

	if (*p == L'.') {
		double f = 0.0;
		int n = 0;
		++p;

		while (*p >= L'0' && *p <= L'9') {
			f = (f * 10.0) + (*p - L'0');
			++p;
			++n;
		}

		r += f / std::pow(10.0, n);
	}

	if (*p != L'\0') {
		return 0.0;
	}

	if (neg) {
		r = -r;
	}

	return r;
}

inline float _stof(const std::wstring& p) {
	return (float)_stod(p.c_str());
}

//chekc if a string is number
inline bool StrIsNum(const wchar_t* p) {
	if (*p == L'-') {
		++p;
	}
	if (*p == L'+') {
		++p;
	}

	while (*p >= L'0' && *p <= L'9') {
		++p;
	}

	if (*p == L'.') {
		++p;

		while (*p >= L'0' && *p <= L'9') {
			++p;
		}

	}

	if (*p != L'\0') {
		return false;
	}

	return true;
}

inline bool StrIsNum(const std::wstring& p) {
	return StrIsNum(p.c_str());
}

inline long ReturnFloat(LPRDATA rdPtr, float Val) {
	if (Val == (int)Val) {
		return (int)Val;
	}
	else {
		//Setting the HOF_FLOAT flag lets MMF know that you are returning a float.
		rdPtr->rHo.hoFlags |= HOF_FLOAT;

		//Return the float without conversion
		return *((long*)&Val);
	}
}

#define ReturnFloat(Val) ReturnFloat(rdPtr, Val)

inline bool Modified(SI_Error Input) {
	return Input == SI_UPDATED || Input == SI_INSERTED;
}

inline void SaveFile(LPINI Ini,const wchar_t* FilePath, const wchar_t* Key) {
	//Key has value, try to Encry
	if (!StrEmpty(Key)) {
		std::string Output;
		Ini->Save(Output);

		Encryption Encrypt;
		Encrypt.GenerateKey(Key);

		Encrypt.SetEncryptStr(Output);
		Encrypt.Encrypt();

		Encrypt.SaveFile(FilePath);
	}
	else {
		Ini->SaveFile(FilePath, false);
	}
}

inline void AutoSave(LPRDATA rdPtr) {
	if (rdPtr->Modified && rdPtr->AutoSave && valid(Fini) && valid(rdPtr->AutoSaveFilePath) && valid(rdPtr->AutoSaveKey)) {
		SaveFile(Fini, rdPtr->AutoSaveFilePath, rdPtr->AutoSaveKey);
	}
}

#endif // !_FUNC_

