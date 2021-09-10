#ifndef _FUNC_
#define _FUNC_

inline bool valid(LPRDATA rdPtr) {
	return Fini != nullptr;
}

#define valid() valid(rdPtr)
#define invalid() if (!valid()) { return 0; }

inline void release(LPRDATA rdPtr) {
	if (valid()) {
		delete Fini;
	}

	Fini = nullptr;
}

#define release() release(rdPtr)

inline void Init(LPRDATA rdPtr) {
	release();

	Fini = new INI;
	Fini->SetUnicode();
}

#define Init() Init(rdPtr)

inline void NewStr(LPRDATA rdPtr, LPCTSTR Str) {
	if (OStr != nullptr) {
		delete OStr;
	}
	rsize_t total_length = wcslen(Str) + 1;

	OStr = new WCHAR[total_length];
	wcscpy_s(OStr, total_length, Str);
}

#define NewStr(X) NewStr(rdPtr,X)

#endif // !_FUNC_

