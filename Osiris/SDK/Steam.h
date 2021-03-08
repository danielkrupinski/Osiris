#pragma once

#include <cstdint>

#include "VirtualMethod.h"

class SteamFriends {
public:
	VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (this, steamID))
};
