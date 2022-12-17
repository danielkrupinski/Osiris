#include <memory>

#include "../Memory.h"

#include <CSGO/Entity.h>
#include <CSGO/GameMovement.h>
#include <CSGO/GlobalVars.h>
#include <CSGO/LocalPlayer.h>
#include <CSGO/MoveHelper.h>
#include <CSGO/Prediction.h>

#include "EnginePrediction.h"
#include <Interfaces/ClientInterfaces.h>

static int localPlayerFlags;

void EnginePrediction::run(const ClientInterfaces& clientInterfaces, const Memory& memory, csgo::UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;
    
    localPlayerFlags = localPlayer.get().flags();

    *memory.predictionRandomSeed = 0;

    const auto oldCurrenttime = memory.globalVars->currenttime;
    const auto oldFrametime = memory.globalVars->frametime;

    memory.globalVars->currenttime = memory.globalVars->serverTime();
    memory.globalVars->frametime = memory.globalVars->intervalPerTick;

    memory.moveHelper().setHost(localPlayer.get().getPOD());
    clientInterfaces.getPrediction().setupMove(localPlayer.get().getPOD(), cmd, memory.moveHelper().getPOD(), memory.moveData);
    clientInterfaces.getGameMovement().processMovement(localPlayer.get().getPOD(), memory.moveData);
    clientInterfaces.getPrediction().finishMove(localPlayer.get().getPOD(), cmd, memory.moveData);
    memory.moveHelper().setHost(nullptr);

    *memory.predictionRandomSeed = -1;

    memory.globalVars->currenttime = oldCurrenttime;
    memory.globalVars->frametime = oldFrametime;
}

int EnginePrediction::getFlags() noexcept
{
    return localPlayerFlags;
}
