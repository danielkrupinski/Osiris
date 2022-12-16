#pragma once

#include <cstdint>

#include "VirtualMethod.h"

namespace csgo
{

namespace pod
{
    struct SteamFriends;
    struct SteamUtils;
}

class SteamFriends : public VirtualCallableFromPOD<SteamFriends, pod::SteamFriends> {
public:
    VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (steamID))
};

class SteamUtils : public VirtualCallableFromPOD<SteamUtils, pod::SteamUtils> {
public:
    VIRTUAL_METHOD(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (image, buff, buffSize))
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    pod::SteamFriends* steamFriends;
    pod::SteamUtils* steamUtils;
};

}
