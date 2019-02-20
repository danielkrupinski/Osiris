#pragma once

#include "Vector.h"

class UserCmd final {
public:
    enum {
        IN_ATTACK = 1 << 0,
        IN_JUMP = 1 << 1,
        IN_ATTACK2 = 1 << 11,
        IN_BULLRUSH = 1 << 22
    };
    int pad;
    int     command_number;
    int     tick_count;
    Vector  viewangles;
    Vector  aimdirection;
    float   forwardmove;
    float   sidemove;
    float   upmove;
    int     buttons;
    char    impulse;
    int     weaponselect;
    int     weaponsubtype;
    int     random_seed;
    short   mousedx;
    short   mousedy;
    bool    hasbeenpredicted;
    char    pad_0x4C[0x18];
};
