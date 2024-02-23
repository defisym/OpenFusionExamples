#pragma once

#include <string>

#include "SteamInclude.h"

//constexpr size_t GamepadTextInputDismissed = 0;
//constexpr size_t FloatingGamepadTextInputDismissed = 1;

class SteamGamepadTextInput :public SteamCallbackClass<SteamGamepadTextInput> {
private:
	std::string inputText;
	bool bSubmitted = false;

	using DismissCallback = std::function<void(bool bSubmitted, const std::string& text)>;
	DismissCallback dismissCallback = nullptr;

private:
	inline void InitCallback() override {
		AddCallback(GetCallBack<GamepadTextInputDismissed_t>([&] (const GamepadTextInputDismissed_t* pCallback) {
			bSubmitted = pCallback->m_bSubmitted;

			if (bSubmitted) {
				const auto length = SteamUtils()->GetEnteredGamepadTextLength() + 1;
				inputText = std::string(length, 0);
				SteamUtils()->GetEnteredGamepadTextInput(inputText.data(), length);
			}

			if (this->dismissCallback != nullptr) {
				this->dismissCallback(pCallback->m_bSubmitted, GetText());
			}

			return true;
			}));
		AddCallback(GetCallBack<FloatingGamepadTextInputDismissed_t>([&] (const FloatingGamepadTextInputDismissed_t* pCallback) {
			return true;
			}));
	}

public:
	SteamGamepadTextInput() { SteamGamepadTextInput::InitCallback(); }
	~SteamGamepadTextInput() override = default;

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
