#pragma once

inline bool Modified(SI_Error Input) {
	return Input == SI_UPDATED || Input == SI_INSERTED;
}

inline bool LoadFile(LPINI pIni,LPCTSTR pFilePath,LPCTSTR pKey) {
	auto ret = SI_OK;

	//Key has value, try to Decry
	if (!StrEmpty(pKey)) {
		Encryption Decrypt;
		Decrypt.GenerateKey(pKey);

		Decrypt.OpenFile(pFilePath);
		Decrypt.Decrypt();

		ret = pIni->LoadData(Decrypt.GetOutputStr(), Decrypt.GetOutputStrLength());
		Decrypt.ReleaseOutputStr();
	}
	else {
		ret = pIni->LoadFile(pFilePath);
	}

	return ret == SI_OK;
}

inline bool SaveFile(LPINI pIni, const wchar_t* pFilePath, const wchar_t* pKey) {
	auto ret = true;

	//Key has value, try to Encry
	if (!StrEmpty(pKey)) {
		std::string Output;
		pIni->Save(Output);

		Encryption Encrypt;
		Encrypt.GenerateKey(pKey);

		Encrypt.SetEncryptStr(Output);
		Encrypt.Encrypt();

		ret = Encrypt.SaveFile(pFilePath);
	}
	else {
		ret = pIni->SaveFile(pFilePath, false) == SI_OK;
	}

	return ret;
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
		offset += sizeof(rCom) + sizeof(rMvt) + sizeof(rSpr)+ sizeof(rAni);
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

inline void GlobalData::StashData(LPRDATA rdPtr) {
	const std::wstring objName = GetObjectName(rdPtr);

	if(!pInis.contains(objName)) {
		pInis[objName] = rdPtr->ini;
	}
}

inline void GlobalData::RetrieveData(LPRDATA rdPtr) {
	const std::wstring objName = GetObjectName(rdPtr);
	const auto it = pInis.find(objName);

	if (it != pInis.end()) {
		rdPtr->ini = it->second;
		pInis.erase(it);
	}
}