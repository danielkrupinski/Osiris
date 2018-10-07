#pragma once

#include "checksum_crc.hpp"
#include "checksum_md5.hpp"
#include "../Math/QAngle.hpp"

#define IN_ATTACK  (1 << 0)
#define IN_JUMP   (1 << 1)
#define IN_DUCK   (1 << 2)
#define IN_FORWARD  (1 << 3)
#define IN_BACK   (1 << 4)
#define IN_USE   (1 << 5)
#define IN_CANCEL  (1 << 6)
#define IN_LEFT   (1 << 7)
#define IN_RIGHT  (1 << 8)
#define IN_MOVELEFT  (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2  (1 << 11)
#define IN_RUN   (1 << 12)
#define IN_RELOAD  (1 << 13)
#define IN_ALT1   (1 << 14)
#define IN_ALT2   (1 << 15)
#define IN_SCORE  (1 << 16)   // Used by client_panorama.dll for when scoreboard is held down
#define IN_SPEED  (1 << 17) // Player is holding the speed key
#define IN_WALK   (1 << 18) // Player holding walk key
#define IN_ZOOM   (1 << 19) // Zoom key for HUD zoom
#define IN_WEAPON1  (1 << 20) // weapon defines these bits
#define IN_WEAPON2  (1 << 21) // weapon defines these bits
#define IN_BULLRUSH  (1 << 22)
#define IN_GRENADE1  (1 << 23) // grenade 1
#define IN_GRENADE2  (1 << 24) // grenade 2
#define IN_LOOKSPIN  (1 << 25)

class CUserCmd
{
public:
    CUserCmd()
    {
        memset(this, 0, sizeof(*this));
    };
    virtual ~CUserCmd() {};

    CRC32_t GetChecksum(void) const
    {
        CRC32_t crc;
        CRC32_Init(&crc);

        CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
        CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
        CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
        CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
        CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
        CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
        CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
        CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
        CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
        CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
        CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
        CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
        CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
        CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

        CRC32_Final(&crc);
        return crc;
    }

    int     command_number;     // 0x04 For matching server and client commands for debugging
    int     tick_count;         // 0x08 the tick the client created this command
    QAngle  viewangles;         // 0x0C Player instantaneous view angles.
    Vector  aimdirection;       // 0x18
    float   forwardmove;        // 0x24
    float   sidemove;           // 0x28
    float   upmove;             // 0x2C
    int     buttons;            // 0x30 Attack button states
    char    impulse;            // 0x34
    int     weaponselect;       // 0x38 Current weapon id
    int     weaponsubtype;      // 0x3C
    int     random_seed;        // 0x40 For shared random functions
    short   mousedx;            // 0x44 mouse accum in x from create move
    short   mousedy;            // 0x46 mouse accum in y from create move
    bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
    char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

class CVerifiedUserCmd
{
public:
    CUserCmd m_cmd;
    CRC32_t  m_crc;
};
