#include "PredictionSystem.h"

#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/GameMovement.h"
#include "../SDK/MoveData.h"
#include "../SDK/MoveHelper.h"
#include "../SDK/Prediction.h"
#include "../SDK/ChecksumMD5.h"

float previousCurrenttime;
float previousFrametime;
static MoveData* moveData = nullptr;

void PredictionSystem::StartPrediction(UserCmd* cmd)
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    if (!localPlayer || !cmd)
        return;

    *memory.predictionRandomSeed = MD5_PseudoRandom(cmd->commandNumber) & 0x7FFFFFFF;
    **memory.predictionPlayer = localPlayer;

    previousCurrenttime = memory.globalVars->currenttime;
    previousFrametime = memory.globalVars->frametime;

    *(UserCmd**)((uintptr_t)localPlayer + 0x16E8) = cmd;

    memory.globalVars->currenttime = memory.globalVars->serverTime(cmd);
    memory.globalVars->frametime = memory.globalVars->intervalPerTick;

    if (!moveData)
        moveData = (MoveData*)(calloc(1, sizeof(MoveData)));

    cmd->buttons |= *reinterpret_cast<uint32_t*>(uintptr_t(localPlayer) + 0x3310);
    if (cmd->impulse)
        *reinterpret_cast<uint8_t*>(uintptr_t(localPlayer) + 0x31EC) = cmd->impulse;

    const auto buttonsChanged = cmd->buttons ^ *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31E8);
    *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31DC) = *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31E8);
    *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31E8) = cmd->buttons;
    *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31E0) = cmd->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
    *reinterpret_cast<int*>(uintptr_t(localPlayer) + 0x31E4) = buttonsChanged & ~cmd->buttons; //m_afButtonReleased ~ The ones not down are "released"

    memory.moveHelper->SetHost(localPlayer);
    interfaces.gameMovement->StartTrackPredictionErrors(localPlayer);
    interfaces.prediction->SetupMove(localPlayer, cmd, memory.moveHelper, moveData);
    interfaces.gameMovement->ProcessMovement(localPlayer, moveData);
    interfaces.prediction->FinishMove(localPlayer, cmd, moveData);
}

void PredictionSystem::EndPrediction()
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };

    interfaces.gameMovement->FinishTrackPredictionErrors(localPlayer);
    memory.moveHelper->SetHost(nullptr);

    *memory.predictionRandomSeed = -1;
    **memory.predictionPlayer = nullptr;

    memory.globalVars->currenttime = previousCurrenttime;
    memory.globalVars->frametime = previousFrametime;
}