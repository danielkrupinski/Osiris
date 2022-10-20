#pragma once

#include <cstdint>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class SteamFriends {
public:
    INCONSTRUCTIBLE(SteamFriends)

    VIRTUAL_METHOD(int, getSmallFriendAvatar, 34, (std::uint64_t steamID), (this, steamID))
};

namespace csgo::pod { struct SteamUtils; }

class SteamUtils : public VirtualCallableFromPOD<SteamUtils, csgo::pod::SteamUtils> {
public:
    VIRTUAL_METHOD2(bool, getImageRGBA, 6, (int image, std::uint8_t* buff, int buffSize), (image, buff, buffSize))
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    SteamFriends* steamFriends;
    csgo::pod::SteamUtils* steamUtils;
};
