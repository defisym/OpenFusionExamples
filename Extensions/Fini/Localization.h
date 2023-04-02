#pragma once

struct Localization {
	LPRDATA pObject=nullptr;

	LPINI pLocalization = nullptr;
	std::wstring* pLanguageCode = nullptr;

	Localization() {
		this->pLanguageCode = new std::wstring;
	}
	~Localization() {
		if (this->pObject == nullptr) {
			ReleaseFile();
		}

		delete this->pLanguageCode;
	}

	inline void ResetObject() {
		this->pObject = nullptr;
	}

	inline void LinkObject(const LPRDATA pObject) {
		ResetFile();
		ResetObject();

		if (!pObject || pObject->rHo.hoIdentifier != IDENTIFIER) {
			return;
		}

		this->pObject = pObject;		
		UpdateLink();
	}

	// should be call periodically to update object state for danling
	inline void UpdateLink() {
		if (this->pObject != nullptr && !IsDestroyed((LPRO)this->pObject)) {
			this->pLocalization = pObject->ini;

			return;
		}

		this->pLocalization = nullptr;
		ResetObject();
	}

	inline void NewFile() {
		this->pLocalization = new INI;
		this->pLocalization->SetUnicode();
	}

	inline void ReleaseFile() {
		delete this->pLocalization;
		this->pLocalization = nullptr;
	}

	inline void ResetFile() {
		ReleaseFile();
		NewFile();
	}

	inline void LoadFile(const LPCTSTR pFilePath, const LPCTSTR pKey) {
		ResetFile();
		ResetObject();

		if(!::LoadFile(this->pLocalization, pFilePath, pKey)) {
			ResetFile();
		}
	}

	inline void SetLanguageCode(const LPCTSTR pLanguageCode) const {
		*this->pLanguageCode = pLanguageCode;
	}

	inline LPCTSTR GetLocalization(LPCTSTR pItem) const {
		if (this->pLocalization != nullptr && !StrEmpty(this->pLanguageCode->c_str())) {
			const auto pLocalzation = this->pLocalization->GetValue(this->pLanguageCode->c_str(),
				pItem, Default_Str);

			if (!StrEmpty(pLocalzation)) {
				pItem = pLocalzation;
			}
		}

		return pItem;
	}
};