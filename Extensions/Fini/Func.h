#ifndef _FUNC_
#define _FUNC_

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

inline size_t GetRVOffset(LPRDATA rdPtr, LPRO object) {
	if (!LPROValid(object)) {
		return -1;
	}

	size_t offset = sizeof(headerObject);

	// Internal objects
	auto identifier = object->roHo.hoIdentifier;

	if (identifier == IDENTIFIER_LIVES || identifier == IDENTIFIER_SCORE) {		
		return -1;
	}

	if (identifier == IDENTIFIER_COUNTER || identifier == IDENTIFIER_STRING) {
		if (rdPtr->cf25p && rdPtr->allowRVforCS) {
			offset += sizeof(rCom) + sizeof(rMvt) + sizeof(rSpr);
			return offset;
		}
		else {
			return -1;
		}
	}

	if (identifier == IDENTIFIER_ACTIVE) {
		offset += sizeof(rCom) + sizeof(rMvt) + sizeof(rSpr);
		return offset;
	}

	// General extensions
	auto flags = object->roHo.hoOEFlags;

	if (!(flags & OEFLAG_VALUES)) {
		return -1;
	}

	offset += (flags & OEFLAG_MOVEMENTS) || (flags & OEFLAG_ANIMATIONS) || (flags & OEFLAG_SPRITES)
		? sizeof(rCom) : 0;
	offset += (flags & OEFLAG_MOVEMENTS) ? sizeof(rMvt) : 0;
	offset += (flags & OEFLAG_ANIMATIONS) ? sizeof(rAni) : 0;
	offset += (flags & OEFLAG_SPRITES) ? sizeof(rSpr) : 0;

	return offset;
}

#endif // !_FUNC_

