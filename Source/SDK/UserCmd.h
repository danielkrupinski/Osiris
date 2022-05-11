#pragma once

#include "Vector.h"
#include "CheckSumCRC.h"

struct UserCmd {
    enum {
        IN_ATTACK = 1 << 0,
        IN_JUMP = 1 << 1,
        IN_DUCK = 1 << 2,
        IN_FORWARD = 1 << 3,
        IN_BACK = 1 << 4,
        IN_USE = 1 << 5,
        IN_MOVELEFT = 1 << 9,
        IN_MOVERIGHT = 1 << 10,
        IN_ATTACK2 = 1 << 11,
        IN_SCORE = 1 << 16,
        IN_BULLRUSH = 1 << 22
    };
    void* vmt;
    int commandNumber;
    int tickCount;
    Vector viewangles;
    Vector aimdirection;
    float forwardmove;
    float sidemove;
    float upmove;
    int buttons;
    char impulse;
    int weaponselect;
    int weaponsubtype;
    int randomSeed;
    short mousedx;
    short mousedy;
    bool hasbeenpredicted;

    const CRC32 getChecksum() noexcept
    {
        CRC32 crc;
        CRC32_Init(&crc);

        CRC32_ProcessBuffer(&crc, &commandNumber, sizeof(commandNumber));
        CRC32_ProcessBuffer(&crc, &tickCount, sizeof(tickCount));
        CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
        CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
        CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
        CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
        CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
        CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
        CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
        CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
        CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
        CRC32_ProcessBuffer(&crc, &randomSeed, sizeof(randomSeed));
        CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
        CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

        CRC32_Final(&crc);

        return crc;
    }
};

class VerifiedUserCmd
{
public:
    UserCmd cmd;
    CRC32 crc;
};
