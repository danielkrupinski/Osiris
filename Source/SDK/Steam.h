#pragma once

#include <cstdint>

#include "VirtualMethod.h"

namespace csgo::pod
{
    struct SteamFriends;
    struct SteamUtils;
}

class SteamFriends : public VirtualCallableFromPOD<SteamFriends, csgo::pod::SteamFriends> {
public:
    VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (steamID))
};

class SteamUtils : public VirtualCallableFromPOD<SteamUtils, csgo::pod::SteamUtils> {
public:
    VIRTUAL_METHOD(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (image, buff, buffSize))
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    csgo::pod::SteamFriends* steamFriends;
    csgo::pod::SteamUtils* steamUtils;
};
