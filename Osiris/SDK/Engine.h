#pragma once

#include <cstdint>
#include <Windows.h>
#include <d3d9types.h>

#include "Vector.h"
#include "VirtualMethod.h"

struct PlayerInfo {
    std::uint64_t version;
    union {
        std::uint64_t xuid;
        struct {
            std::uint32_t xuidLow;
            std::uint32_t xuidHigh;
        };
    };
    char name[128];
    int userId;
    char guid[33];
    std::uint32_t friendsId;
    char friendsName[128];
    bool fakeplayer;
    bool hltv;
    int customfiles[4];
    unsigned char filesdownloaded;
    int entityIndex;
};

class NetworkChannel;

class Engine {
public:
    VIRTUAL_METHOD(bool, getPlayerInfo, 8, (int entityIndex, PlayerInfo& playerInfo), (this, entityIndex, std::ref(playerInfo)))
    VIRTUAL_METHOD(int, getPlayerForUserID, 9, (int userId), (this, userId))
    VIRTUAL_METHOD(void, getViewAngles, 18, (Vector& angles), (this, std::ref(angles)))
    VIRTUAL_METHOD(void, setViewAngles, 19, (const Vector& angles), (this, std::cref(angles)))
    VIRTUAL_METHOD(int, getMaxClients, 20, (), (this))
    VIRTUAL_METHOD(bool, isInGame, 26, (), (this))
    VIRTUAL_METHOD(bool, isConnected, 27, (), (this))
    VIRTUAL_METHOD(void*, getBSPTreeQuery, 43, (), (this))
    VIRTUAL_METHOD(const char*, getLevelName, 53, (), (this))
    VIRTUAL_METHOD(NetworkChannel*, getNetworkChannel, 78, (), (this))
    VIRTUAL_METHOD(void, clientCmdUnrestricted, 114, (const char* cmd), (this, cmd, false))
    VIRTUAL_METHOD(const D3DMATRIX&, worldToScreenMatrix, 37, (), (this))
};
