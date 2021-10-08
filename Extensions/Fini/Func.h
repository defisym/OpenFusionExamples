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

