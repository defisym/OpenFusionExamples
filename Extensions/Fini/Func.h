#ifndef _FUNC_
#define _FUNC_

inline void Init(LPRDATA rdPtr) {
	if (rdPtr->ini != nullptr) {
		delete rdPtr->ini;
	}

	rdPtr->ini = new INI;
}

#define Init() Init(rdPtr)

inline bool valid(LPRDATA rdPtr) {
	return rdPtr->ini != nullptr;
}

#define valid() valid(rdPtr)

inline void release(LPRDATA rdPtr) {
	delete rdPtr->ini;
	rdPtr->ini = nullptr;
}

#define release() release(rdPtr)

inline std::string to_byte_string(const std::wstring& input)
{
	WTBC converter;
	return converter.to_bytes(input);
}

#define _B(X) to_byte_string(X)

inline void NewStr(LPRDATA rdPtr, std::wstring& Str) {
	if (rdPtr->Str != nullptr) {
		delete rdPtr->Str;
	}
	rsize_t total_length = wcslen(Str.c_str()) + 1;

	rdPtr->Str = new WCHAR[total_length];
	wcscpy_s(rdPtr->Str, total_length, Str.c_str());
}

#define NewStr(X) NewStr(rdPtr,X)

#endif // !_FUNC_

