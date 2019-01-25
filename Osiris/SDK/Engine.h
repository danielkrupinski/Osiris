#pragma once

#include "NetworkChannel.h"
#include "Utils.h"
#include "Vector.h"

class Engine {
public:
    auto getLocalPlayer() noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*)>(this, 12)(this);
    }

    auto getViewAngles(Vector& angles) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, Vector&)>(this, 18)(this, angles);
    }

    auto setViewAngles(const Vector& angles) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const Vector&)>(this, 19)(this, angles);
    }

	auto getMaxClients() noexcept
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	auto isInGame() noexcept
	{
		return callVirtualFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}

    auto getNetworkChannel() noexcept
    {
        return callVirtualFunction<NetworkChannel*(__thiscall*)(void*)>(this, 78)(this);
    }
};
