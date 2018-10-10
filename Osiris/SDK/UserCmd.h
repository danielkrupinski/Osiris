#pragma once

class UserCmd final {
public:
    int pad;
    int     command_number;     // 0x04 For matching server and client commands for debugging
    int     tick_count;         // 0x08 the tick the client created this command
    // QAngle  viewangles;         // 0x0C Player instantaneous view angles.
    // Vector  aimdirection;       // 0x18
    short pad1[3];
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
