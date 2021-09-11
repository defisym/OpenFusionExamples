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

inline void NewStr(LPTSTR& Tar, LPCTSTR Str) {
	if (Tar != nullptr) {
		delete[] Tar;
	}
	rsize_t total_length = wcslen(Str) + 1;

	Tar = new WCHAR[total_length];
	wcscpy_s(Tar, total_length, Str);
}

#define InvalidSecItem(X) if ((wcscmp(Section, Empty_Str) == 0) || (wcscmp(Item, Empty_Str) == 0)) { return X; }

#endif // !_FUNC_

