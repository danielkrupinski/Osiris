#include "Entity.h"
#include "GlobalVars.h"
#include "LocalPlayer.h"
#include "UserCmd.h"

float GlobalVars::serverTime(UserCmd* cmd) const noexcept
{
    static int tick;
    static UserCmd* lastCmd;

    if (cmd) {
        if (localPlayer && (!lastCmd || lastCmd->hasbeenpredicted))
            tick = localPlayer->tickBase();
        else
            tick++;
        lastCmd = cmd;
    }
    return tick * intervalPerTick;
}

bool GlobalVars::newTick() const noexcept
{
    static float lastTick = 0.f;
    if (lastTick != tickCount) {
        lastTick = tickCount;
        return true;
    }
    return false;
}