#pragma once

struct PlayerInfo {
    __int64         unknown;            //0x0000 
    union
    {
        __int64       steamID64;          //0x0008 - SteamID64
        struct
        {
            __int32     xuid_low;
            __int32     xuid_high;
        };
    };
    char            szName[128];        //0x0010 - Player Name
    int             userId;             //0x0090 - Unique Server Identifier
    char            szSteamID[20];      //0x0094 - STEAM_X:Y:Z
    char            pad_0x00A8[0x10];   //0x00A8
    unsigned long   iSteamID;           //0x00B8 - SteamID 
    char            szFriendsName[128];
    bool            fakeplayer;
    bool            ishltv;
    unsigned int    customfiles[4];
    unsigned char   filesdownloaded;
};

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
