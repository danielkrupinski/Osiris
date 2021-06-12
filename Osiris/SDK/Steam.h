#pragma once

#include <cstdint>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class SteamFriends {
public:
    INCONSTRUCTIBLE(SteamFriends)

    VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (this, steamID))
};

class SteamUtils {
public:
    INCONSTRUCTIBLE(SteamUtils)

    VIRTUAL_METHOD(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (this, image, buff, buffSize))
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    SteamFriends* steamFriends;
    SteamUtils* steamUtils;
};
