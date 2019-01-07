#pragma once

#include "QAngle.h"
#include "Utils.h"

class Engine {
public:
    auto getViewAngles(QAngle& angles) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, QAngle&)>(this, 18)(this, angles);
    }

    auto setViewAngles(const QAngle& angles) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const QAngle&)>(this, 19)(this, angles);
    }

	auto getMaxClients() noexcept
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	auto isInGame() noexcept
	{
		return callVirtualFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}
};
