#include <memory>

#include "../Interfaces.h"
#include "../Memory.h"

#include "../SDK/Entity.h"
#include "../SDK/GameMovement.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/MoveHelper.h"
#include "../SDK/Prediction.h"

#include "EnginePrediction.h"

static int localPlayerFlags;

void EnginePrediction::run(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;
    
    localPlayerFlags = localPlayer->flags();

    *memory->predictionRandomSeed = 0;

    const auto oldCurrenttime = memory->globalVars->currenttime;
    const auto oldFrametime = memory->globalVars->frametime;

    memory->globalVars->currenttime = memory->globalVars->serverTime();
    memory->globalVars->frametime = memory->globalVars->intervalPerTick;

    memory->moveHelper->setHost(localPlayer.get());
    interfaces->prediction->setupMove(localPlayer.get(), cmd, memory->moveHelper, memory->moveData);
    interfaces->gameMovement->processMovement(localPlayer.get(), memory->moveData);
    interfaces->prediction->finishMove(localPlayer.get(), cmd, memory->moveData);
    memory->moveHelper->setHost(nullptr);

    *memory->predictionRandomSeed = -1;

    memory->globalVars->currenttime = oldCurrenttime;
    memory->globalVars->frametime = oldFrametime;
}

int EnginePrediction::getFlags() noexcept
{
    return localPlayerFlags;
}
