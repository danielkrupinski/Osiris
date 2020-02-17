#include "../Interfaces.h"
#include "../Memory.h"

#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/GameMovement.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/MoveHelper.h"
#include "../SDK/Prediction.h"

#include "EnginePrediction.h"

static int localPlayerFlags;

void EnginePrediction::run(UserCmd* cmd) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
    if (!localPlayer)
        return;
    
    localPlayerFlags = localPlayer->flags();

    *memory.predictionRandomSeed = 0;

    const auto oldCurrenttime = memory.globalVars->currenttime;
    const auto oldFrametime = memory.globalVars->frametime;

    memory.globalVars->currenttime = memory.globalVars->serverTime();
    memory.globalVars->frametime = memory.globalVars->intervalPerTick;

    memory.moveHelper->setHost(localPlayer);
    interfaces.prediction->setupMove(localPlayer, cmd, memory.moveHelper, memory.moveData);
    interfaces.gameMovement->processMovement(localPlayer, memory.moveData);
    interfaces.prediction->finishMove(localPlayer, cmd, memory.moveData);
    memory.moveHelper->setHost(nullptr);

    *memory.predictionRandomSeed = -1;

    memory.globalVars->currenttime = oldCurrenttime;
    memory.globalVars->frametime = oldFrametime;
}

int EnginePrediction::getFlags() noexcept
{
    return localPlayerFlags;
}
