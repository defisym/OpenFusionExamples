#pragma once

inline bool EncryptCore(LPCTSTR pSrcFilePath, LPCTSTR pDesFilePath, LPCTSTR pKey, encryption::Mode mode) {
    Encryption Encrypt;
    Encrypt.GenerateKey(pKey);

    if (![&]{
		switch(mode){
		case encryption::Mode::encrypt:
			return Encrypt.EncryptFileDirectly(pSrcFilePath);
		case encryption::Mode::decrypt:
			return Encrypt.DecryptFileDirectly(pSrcFilePath);
		default:
			return false;
		}
	}()) { return false; }

    return Encrypt.SaveFile(pDesFilePath);
}
