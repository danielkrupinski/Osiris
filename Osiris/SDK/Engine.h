#pragma once

#include "NetworkChannel.h"
#include "Utils.h"
#include "Vector.h"

struct PlayerInfo {
    __int64 pad;
    union {
        __int64 steamID64;
        __int32 xuidLow;
        __int32 xuidHigh;
    };
    char name[128];
    int userId;
    char steamIdString[20];
    char pad1[16];
    unsigned long steamId;
    char friendsName[128];
    bool fakeplayer;
    bool ishltv;
    unsigned int customfiles[4];
    unsigned char filesdownloaded;
};

class Engine {
public:
    constexpr auto getPlayerInfo(int entityIndex, const PlayerInfo& playerInfo) noexcept
    {
        return callVirtualFunction<bool, int, const PlayerInfo&>(this, 8, entityIndex, playerInfo);
    }

    constexpr auto getLocalPlayer() noexcept
    {
        return callVirtualFunction<int>(this, 12);
    }

    constexpr auto getViewAngles(Vector& angles) noexcept
    {
        callVirtualFunction<void, Vector&>(this, 18, angles);
    }

    constexpr auto setViewAngles(const Vector& angles) noexcept
    {
        callVirtualFunction<void, const Vector&>(this, 19, angles);
    }

	constexpr auto getMaxClients() noexcept
	{
		return callVirtualFunction<int>(this, 20);
	}

	constexpr auto isInGame() noexcept
	{
		return callVirtualFunction<bool>(this, 26);
	}

    using Matrix = float[4][4];

    constexpr auto worldToScreenMatrix() noexcept
    {
        return callVirtualFunction<const Matrix&>(this, 37);
    }

    constexpr auto getNetworkChannel() noexcept
    {
        return callVirtualFunction<NetworkChannel*>(this, 78);
    }
};
