#pragma once

#include "QAngle.h"
#include "Utils.h"

class Engine {
public:
    void setViewAngles(const QAngle& angles)
    {
        callVirtualFunction<void(__thiscall*)(void*, const QAngle&)>(this, 19)(this, angles);
    }

	int getMaxClients()
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	bool isInGame()
	{
		return callVirtualFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}
};
