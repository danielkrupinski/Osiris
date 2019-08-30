#include "PredictionSystem.h"

#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/GameMovement.h"
#include "../SDK/MoveData.h"
#include "../SDK/MoveHelper.h"
#include "../SDK/Prediction.h"
#include "../SDK/ChecksumMD5.h"

float m_flOldCurtime;
float m_flOldFrametime;
MoveData m_MoveData;

void PredictionSystem::StartPrediction(UserCmd* cmd)
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    *memory.predictionRandomSeed = MD5_PseudoRandom(cmd->commandNumber) & 0x7FFFFFFF;

    m_flOldCurtime = memory.globalVars->currenttime;
    m_flOldFrametime = memory.globalVars->frametime;

    memory.globalVars->currenttime = memory.globalVars->serverTime(cmd);
    memory.globalVars->frametime = memory.globalVars->intervalPerTick;

    interfaces.gameMovement->StartTrackPredictionErrors(localPlayer);

    memset(&m_MoveData, 0, sizeof(m_MoveData));
    memory.moveHelper->SetHost(localPlayer);
    interfaces.prediction->SetupMove(localPlayer, cmd, memory.moveHelper, &m_MoveData);
    interfaces.gameMovement->ProcessMovement(localPlayer, &m_MoveData);
    interfaces.prediction->FinishMove(localPlayer, cmd, &m_MoveData);
}

void PredictionSystem::EndPrediction()
{
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    interfaces.gameMovement->FinishTrackPredictionErrors(localPlayer);
    memory.moveHelper->SetHost(nullptr);

    *memory.predictionRandomSeed = -1;

    memory.globalVars->currenttime = m_flOldCurtime;
    memory.globalVars->frametime = m_flOldFrametime;
}