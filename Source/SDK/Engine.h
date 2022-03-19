#pragma once

#include <cstdint>
#include <functional>

#include "Inconstructible.h"
#include "Pad.h"
#include "Vector.h"
#include "VirtualMethod.h"

struct SteamAPIContext;

struct Matrix4x4 {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;

        };
        float m[4][4];
    };
};

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
    unsigned char filesDownloaded;
};

struct DemoPlaybackParameters {
    PAD(16)
    bool anonymousPlayerIdentity;
    PAD(23)
};

class NetworkChannel;

class Engine {
public:
    INCONSTRUCTIBLE(Engine)

    VIRTUAL_METHOD(void, getScreenSize, 5, (int& w, int& h), (this, std::ref(w), std::ref(h)))
    VIRTUAL_METHOD(bool, getPlayerInfo, 8, (int entityIndex, PlayerInfo& playerInfo), (this, entityIndex, std::ref(playerInfo)))
    VIRTUAL_METHOD(int, getPlayerForUserID, 9, (int userId), (this, userId))
    VIRTUAL_METHOD(void, getViewAngles, 18, (Vector& angles), (this, std::ref(angles)))
    VIRTUAL_METHOD(void, setViewAngles, 19, (const Vector& angles), (this, std::cref(angles)))
    VIRTUAL_METHOD(int, getMaxClients, 20, (), (this))
    VIRTUAL_METHOD(bool, isInGame, 26, (), (this))
    VIRTUAL_METHOD(bool, isConnected, 27, (), (this))
    VIRTUAL_METHOD(bool, cullBox, 34, (const Vector& mins, const Vector& maxs), (this, std::cref(mins), std::cref(maxs)))
    VIRTUAL_METHOD(const Matrix4x4&, worldToScreenMatrix, 37, (), (this))
    VIRTUAL_METHOD(void*, getBSPTreeQuery, 43, (), (this))
    VIRTUAL_METHOD(const char*, getLevelName, 53, (), (this))
    VIRTUAL_METHOD(NetworkChannel*, getNetworkChannel, 78, (), (this))
    VIRTUAL_METHOD(void, clientCmdUnrestricted, WIN32_LINUX(114, 113), (const char* cmd, bool fromConsoleOrKeybind = false), (this, cmd, fromConsoleOrKeybind))
    VIRTUAL_METHOD_V(const SteamAPIContext*, getSteamAPIContext, 185, (), (this))

    auto getViewAngles() noexcept
    {
        Vector ang;
        getViewAngles(ang);
        return ang;
    }
};
