#pragma once

#include "SteamInclude.h"

class SteamFriend :public SteamCallbackClass {
private:
	std::vector<CSteamID> friendList;
	// friend avatar large: 128 * 128, depth 32
	constexpr static size_t AVATAR_BUFFER_SZ = 128 * 128 * 4;
	uint8 friendAvatar[AVATAR_BUFFER_SZ];

	inline void InitCallback() override {
	//AddCallback(GetCallBack<CallbackType>([&] (const CallbackType* pCallback) {
	//	return pCallback->m_eResult == k_EResultOK
	//		&& pCallback->m_nGameID == appID;
	//	}));
	}
public:
	SteamFriend() {
		SteamFriend::InitCallback();
	}
	~SteamFriend() override = default;

	inline const auto& GetFriendList() { return this->friendList; }

	inline void UpdateFriendList(int iFriendFlags = k_EFriendFlagImmediate) {
		const auto friendCount = SteamFriends()->GetFriendCount(iFriendFlags);

		friendList.clear();
		friendList.reserve(friendCount);

		for (int idx = 0; idx < friendCount; idx++) {
			const auto curID = SteamFriends()->GetFriendByIndex(idx, iFriendFlags);
			friendList.emplace_back(curID);
		}
	}

	inline bool GetFriendAvatar(const int hImg) {
		memset(friendAvatar, 0, AVATAR_BUFFER_SZ * sizeof(decltype(friendAvatar[0])));
		return SteamUtils()->GetImageRGBA(hImg, friendAvatar, AVATAR_BUFFER_SZ);		
	}

	inline const uint8* GetFriendAvatarBuffer() const {
		return friendAvatar;
	}
};
