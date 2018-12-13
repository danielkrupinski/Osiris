#pragma once

class EngineClient {
public:
	int getMaxClients()
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 20)(this);
	}

	bool isInGame()
	{
		return callVirtualFunction<int(__thiscall*)(void*)>(this, 26)(this);
	}

	bool isConnected()
	{
		return callVirtualFunction<bool(__thiscall*)(void*)>(this, 27)(this);
	}
};
