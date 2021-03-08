#pragma once

#include <cstdint>

#include "VirtualMethod.h"

class SteamFriends {
public:
	VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (this, steamID))
};

class SteamUtils {
public:
	VIRTUAL_METHOD(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (this, image, buff, buffSize))
};

struct SteamAPIContext {
	void* steamClient;
	void* steamUser;
	SteamFriends* steamFriends;
	SteamUtils* steamUtils;
};
