#pragma once

#include "Utils.h"

class Engine {
public:
	int getMaxClients()
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	bool isInGame()
	{
		return callVirtualFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}
};
