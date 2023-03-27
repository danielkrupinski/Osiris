#pragma once

#include <cstdint>
#include <functional>

#include <Platform/Macros/PlatformSpecific.h>

#include <Utils/Pad.h>
#include "Vector.h"
#include "VirtualMethod.h"

namespace csgo
{

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
    PAD(16);
    bool anonymousPlayerIdentity;
    PAD(23);
};

struct NetworkChannelPOD;
struct EnginePOD;

struct Engine : GameClass<Engine, EnginePOD> {
    VIRTUAL_METHOD(void, getScreenSize, 5, (int& w, int& h), (&w, &h))
    VIRTUAL_METHOD(bool, getPlayerInfo, 8, (int entityIndex, PlayerInfo& playerInfo), (entityIndex, &playerInfo))
    VIRTUAL_METHOD(int, getPlayerForUserID, 9, (int userId), (userId))
    VIRTUAL_METHOD(void, getViewAngles, 18, (Vector& angles), (&angles))
    VIRTUAL_METHOD(void, setViewAngles, 19, (const Vector& angles), (&angles))
    VIRTUAL_METHOD(int, getMaxClients, 20, (), ())
    VIRTUAL_METHOD(bool, isInGame, 26, (), ())
    VIRTUAL_METHOD(bool, isConnected, 27, (), ())
    VIRTUAL_METHOD(bool, cullBox, 34, (const Vector& mins, const Vector& maxs), (&mins, &maxs))
    VIRTUAL_METHOD(const Matrix4x4&, worldToScreenMatrix, 37, (), ())
    VIRTUAL_METHOD(void*, getBSPTreeQuery, 43, (), ())
    VIRTUAL_METHOD(const char*, getLevelName, 53, (), ())
    VIRTUAL_METHOD(NetworkChannelPOD*, getNetworkChannel, 78, (), ())
    VIRTUAL_METHOD(void, clientCmdUnrestricted, WIN32_LINUX(114, 113), (const char* cmd, bool fromConsoleOrKeybind = false), (cmd, fromConsoleOrKeybind))
    VIRTUAL_METHOD_V(const SteamAPIContext*, getSteamAPIContext, 185, (), ())

    auto getViewAngles() const noexcept
    {
        Vector ang;
        getViewAngles(ang);
        return ang;
    }
};

}
