#pragma once

#include <string>

#include "SteamInclude.h"

class SteamGamepadTextInput :public SteamCallbackClass {
private:
	std::string inputText;
	bool bSubmitted = false;

	SteamCallback* pGamepadTextInputDismissed = nullptr;
	SteamCallback* pFloatingGamepadTextInputDismissed = nullptr;

	using DismissCallback = std::function<void(bool bSubmitted, const std::string& text)>;
	DismissCallback dismissCallback = nullptr;

	inline void CallCallback(void* udata = nullptr) override {
		pGamepadTextInputDismissed = GetCallBack<GamepadTextInputDismissed_t>([&] (const GamepadTextInputDismissed_t* pCallback) {
			bSubmitted = pCallback->m_bSubmitted;

			if (bSubmitted) {
				const auto length = SteamUtils()->GetEnteredGamepadTextLength() + 1;
				inputText = std::string(length, 0);
				SteamUtils()->GetEnteredGamepadTextInput(inputText.data(), length);
			}

			if (this->dismissCallback != nullptr) {
				this->dismissCallback(pCallback->m_bSubmitted, GetText());
			}
		});
		pFloatingGamepadTextInputDismissed = GetCallBack<FloatingGamepadTextInputDismissed_t>([&] (const FloatingGamepadTextInputDismissed_t* pCallback) {
			return;
		});
	}
public:
	SteamGamepadTextInput() {
		SteamGamepadTextInput::CallCallback();
	}
	~SteamGamepadTextInput() override {
		delete pGamepadTextInputDismissed;
		delete pFloatingGamepadTextInputDismissed;
	}

	inline const std::string& GetText() const { return inputText; }

	inline bool GetSubmitted() const { return bSubmitted; }

	inline void SetCallback(const DismissCallback& dismissCallback) {
		this->dismissCallback = dismissCallback;
	}

	inline bool ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char* pchDescription, uint32 unCharMax, const char* pchExistingText) {
		inputText.clear();
		bSubmitted = false;

		return SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
	}

	inline bool ShowFloatingGamepadTextInput(EFloatingGamepadTextInputMode eKeyboardMode, const RECT& rect) {
		inputText.clear();
		bSubmitted = false;

		return SteamUtils()->ShowFloatingGamepadTextInput(eKeyboardMode, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
};
