#pragma once

#include <string>

#include "SteamInclude.h"

class SteamRichPresence :public SteamCallbackClass {
private:
	inline void InitCallback() override {
		AddCallback(GetCallBack<FriendRichPresenceUpdate_t>([&] (const FriendRichPresenceUpdate_t* pCallback) {
			return true;
			}));
	}
	public:
		SteamRichPresence() { SteamRichPresence::InitCallback(); }
		~SteamRichPresence() override = default;

	template <CStyleStrConcept Name>
	inline bool SetRichPresence(const Name pchKey, const Name pchValue) {
		if constexpr (CStyleWideStrConcept<Name>) {
			return SetRichPresence(ConvertWStrToStr(pchKey).c_str(), ConvertWStrToStr(pchValue).c_str());
		}
		else {
			return SteamFriends()->SetRichPresence(pchKey, pchValue);
		}
	}

	static inline void ClearRichPresence() {
		SteamFriends()->ClearRichPresence();
	}
	
	template <CStyleStrConcept Name>
	inline auto GetFriendRichPresence(CSteamID steamIDFriend,const Name pchKey) {
		if constexpr (CStyleWideStrConcept<Name>) {
			return ConvertStrToWStr(GetFriendRichPresence(steamIDFriend, ConvertWStrToStr(pchKey).c_str()));
		}
		else {
			return std::string(SteamFriends()->GetFriendRichPresence(steamIDFriend, pchKey));
		}
	}

	static inline int GetFriendRichPresenceKeyCount(CSteamID steamIDFriend) {
		return SteamFriends()->GetFriendRichPresenceKeyCount(steamIDFriend);
	}

	template <CStyleStrConcept Name>
	static inline auto GetFriendRichPresenceKeyByIndex(CSteamID steamIDFriend, int iKey) {
		const auto pRet = SteamFriends()->GetFriendRichPresenceKeyByIndex(steamIDFriend, iKey);
		if constexpr (CStyleWideStrConcept<Name>) {
			return ConvertStrToWStr(pRet);
		}else {
			return std::string(pRet);
		}
	}

	inline void RequestFriendRichPresence(CSteamID steamIDFriend) const {
		CallCallback([&] () {
			SteamFriends()->RequestFriendRichPresence(steamIDFriend);
		});
	}
};