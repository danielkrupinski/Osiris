#pragma once

#include <cstdint>

#include "VirtualMethod.h"

namespace csgo
{

struct SteamFriendsPOD;
struct SteamUtilsPOD;

struct SteamFriends : GameClass<SteamFriends, SteamFriendsPOD> {
    VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (steamID))
};

struct SteamUtils : GameClass<SteamUtils, SteamUtilsPOD> {
    VIRTUAL_METHOD(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (image, buff, buffSize))
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    SteamFriendsPOD* steamFriends;
    SteamUtilsPOD* steamUtils;
};

}
