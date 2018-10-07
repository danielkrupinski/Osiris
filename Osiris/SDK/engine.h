#pragma once

#include <Windows.h>

#include "playerinfo.h"
#include "steam.h"
#include "vector.h"
#include "vmatrix.h"

enum FlowType_t
{
    FLOW_OUTGOING = 0,
    FLOW_INCOMING = 1,
    FLOW_MAX = 2
};
enum Typetype_t
{
    TYPE_GENERIC = 0,
    TYPE_LOCALPLAYER = 1
};

class NET_SetConVar
{
public:
    NET_SetConVar(const char* name, const char* value);
    ~NET_SetConVar();
private:
    DWORD pad[13];
};

class INetChannelInfo
{
public:
    float GetLatency(int flow)
    {
        typedef float(__thiscall* Fn)(void*, int);
        return CallVFunction<Fn>(this, 9)(this, flow);
    }
    float GetAvgLatency(int type)
    {
        typedef float(__thiscall* Fn)(void*, int);
        return CallVFunction<Fn>(this, 10)(this, type);
    }
    void SendNetMsg(NET_SetConVar& convar)
    {
        typedef void(__thiscall* oSendNetMsg)(PVOID, NET_SetConVar&, int, int);
        CallVFunction<oSendNetMsg>(this, 42)(this, convar, 0, 0);
    }
    virtual float GetAvgLatency(int flow) const = 0; // average packet latency in seconds

    ////bameware xd
    uint8_t pad_0x0000[0x17];
    bool m_bShouldDelete;
    int m_nOutSequenceNr;
    int m_nInSequenceNr;
    int m_nOutSequenceNrAck;
    int m_nOutReliableState;
    int m_nInReliableState;
    int m_nChokedPackets;
    ////bameware xd

};
class CEngine
{
public:
    void SetName(std::string name);
    INetChannelInfo* GetNetChannelInfo()
    {
        typedef INetChannelInfo* (__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 78)(this);
    }
    void GetScreenSize(int& Width, int& Height)
    {
        typedef void(__thiscall* Fn)(void*, int&, int&);
        return CallVFunction<Fn>(this, 5)(this, Width, Height);
    }

    bool GetPlayerInfo(int Index, player_info_t* PlayerInfo)
    {
        typedef bool(__thiscall* Fn)(void*, int, player_info_t*);
        return CallVFunction<Fn>(this, 8)(this, Index, PlayerInfo);
    }

    int GetLocalPlayer()
    {
        typedef int(__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 12)(this);
    }
    int GetPlayerForUserID(int userid)
    {
        typedef int(__thiscall* Fn)(void*, int);
        return CallVFunction<Fn>(this, 9)(this, userid);
    }
    void ClientCmd(const char* Command)
    {
        typedef void(__thiscall* Fn)(void*, const char*);
        return CallVFunction<Fn>(this, 108)(this, Command);
    }

    void SetViewAngles(QAngle& Angles)
    {
        typedef void(__thiscall* Fn)(void*, QAngle&);
        return CallVFunction<Fn>(this, 19)(this, Angles);
    }

    void GetViewAngles(QAngle& angle)
    {
        typedef void(__thiscall* Fn)(void*, QAngle&);
        return CallVFunction<Fn>(this, 18)(this, angle);
    }

    matrix3x4_t& WorldToScreenMatrix()
    {
        typedef matrix3x4_t& (__thiscall* Fn)(void*);
        return CallVFunction<Fn>(this, 37)(this);


    }
    bool IsConnected()
    {
        typedef bool(__thiscall* Fn)(PVOID);
        return CallVFunction<Fn>(this, 27)(this);
    }

    bool IsInGame()
    {
        typedef bool(__thiscall* Fn)(PVOID);
        return CallVFunction<Fn>(this, 26)(this);
    }

    int GetMaxClients()
    {
        typedef bool(__thiscall* Fn)(PVOID);
        return CallVFunction<Fn>(this, 20)(this);
    }

    float Time()
    {
        typedef float(__thiscall* Fn)(PVOID);
        return CallVFunction<Fn>(this, 14)(this);
    }


    void ClientCmd_Unrestricted(char  const* cmd)
    {
        typedef void(__thiscall* oClientCmdUnres)(PVOID, const char*, char);
        return CallVFunction<oClientCmdUnres>(this, 114)(this, cmd, 0);
    }
    //52
    char const    GetLevelName()
    {
        typedef char const(__thiscall* oClientCmdUnres)(PVOID);
        return CallVFunction<oClientCmdUnres>(this, 52)(this);
    }
    unsigned int GetEngineBuildNumber()
    {
        typedef unsigned int(__thiscall* OriginalFn)(PVOID);
        return CallVFunction< OriginalFn >(this, 104)(this);
    }

};