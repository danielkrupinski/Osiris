#pragma once

#include "BombTimer/BombTimerState.h"
#include "DefusingAlert/DefusingAlertState.h"
#include "KillfeedPreserver/KillfeedPreserverState.h"
#include "PostRoundTimer/PostRoundTimerState.h"

struct HudFeaturesStates {
    BombTimerState bombTimerState;
    DefusingAlertState defusingAlertState;
    KillfeedPreserverState killfeedPreserverState;
    PostRoundTimerState postRoundTimerState;
};
