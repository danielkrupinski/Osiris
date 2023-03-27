#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct MoveData;
struct UserCmd;

struct EntityPOD;
struct MoveHelperPOD;
struct PredictionPOD;

struct Prediction : GameClass<Prediction, PredictionPOD> {
    VIRTUAL_METHOD_V(void, setupMove, 20, (EntityPOD* localPlayer, UserCmd* cmd, MoveHelperPOD* moveHelper, MoveData* moveData), (localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD_V(void, finishMove, 21, (EntityPOD* localPlayer, UserCmd* cmd, MoveData* moveData), (localPlayer, cmd, moveData))
};

}
