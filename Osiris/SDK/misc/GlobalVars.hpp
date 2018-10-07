#pragma once

class CGlobalVarsBase
{
public:
    float     realtime;                     // 0x0000
    int       framecount;                   // 0x0004
    float     absoluteframetime;            // 0x0008
    float     absoluteframestarttimestddev; // 0x000C
    float     curtime;                      // 0x0010
    float     frametime;                    // 0x0014
    int       maxClients;                   // 0x0018
    int       tickcount;                    // 0x001C
    float     interval_per_tick;            // 0x0020
    float     interpolation_amount;         // 0x0024
    int       simTicksThisFrame;            // 0x0028
    int       network_protocol;             // 0x002C
    void*     pSaveData;                    // 0x0030
    bool      m_bClient;                    // 0x0031
    bool      m_bRemoteClient;              // 0x0032

private:
    // 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
    int       nTimestampNetworkingBase;
    // 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to Set the networking basis, prevents
    //  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
    int       nTimestampRandomizeWindow;
};