#include "Entity.h"
#include "GlobalVars.h"
#include "LocalPlayer.h"
#include "UserCmd.h"

namespace csgo
{

float GlobalVars::serverTime(UserCmd* cmd) const noexcept
{
    static int tick;
    static UserCmd* lastCmd;

    if (cmd) {
        if (localPlayer && (!lastCmd || lastCmd->hasbeenpredicted))
            tick = localPlayer.get().tickBase();
        else
            tick++;
        lastCmd = cmd;
    }
    return tick * intervalPerTick;
}

}

