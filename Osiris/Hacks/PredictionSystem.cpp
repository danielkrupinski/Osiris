#include "PredictionSystem.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GameMovement.h"
#include "../SDK/MoveData.h"
#include "../SDK/MoveHelper.h"
#include "../SDK/Prediction.h"

static float previousCurrenttime{ 0.0f };
static float previousFrametime{ 0.0f };

void PredictionSystem::StartPrediction(UserCmd* cmd) noexcept
{
	const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
	if (!localPlayer || !cmd)
		return;

	*memory.predictionRandomSeed = memory.md5PseudoRandom(cmd->commandNumber) & 0x7FFFFFFF;

	previousCurrenttime = memory.globalVars->currenttime;
	previousFrametime = memory.globalVars->frametime;

	memory.globalVars->currenttime = memory.globalVars->serverTime();
	memory.globalVars->frametime = memory.globalVars->intervalPerTick;

	static MoveData moveData;
	memory.moveHelper->SetHost(localPlayer);
	interfaces.gameMovement->StartTrackPredictionErrors(localPlayer);
	interfaces.prediction->SetupMove(localPlayer, cmd, memory.moveHelper, &moveData);
	interfaces.gameMovement->ProcessMovement(localPlayer, &moveData);
	interfaces.prediction->FinishMove(localPlayer, cmd, &moveData);

	if (const auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon)
		activeWeapon->updateAccuracyPenalty();
}

void PredictionSystem::EndPrediction() noexcept
{
	const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };

	interfaces.gameMovement->FinishTrackPredictionErrors(localPlayer);
	memory.moveHelper->SetHost(nullptr);

	*memory.predictionRandomSeed = -1;

	memory.globalVars->currenttime = previousCurrenttime;
	memory.globalVars->frametime = previousFrametime;
}