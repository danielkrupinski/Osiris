#include "GlobalVars.h"
#include "UserCmd.h"

#define FIX_TICKBASE_PREDICTION 0

float GlobalVars::serverTime(UserCmd* cmd) noexcept
{
#if FIX_TICKBASE_PREDICTION == 1
    static int tick;
    static UserCmd* lastCmd;

    if (cmd) {
        if (!lastCmd || lastCmd->hasbeenpredicted)
            tick = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_nTickBase");
        else
            tick++;
        lastCmd = cmd;
    }
    return tick * intervalPerTick;
#else
    return interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_nTickBase")* intervalPerTick;
#endif
}
