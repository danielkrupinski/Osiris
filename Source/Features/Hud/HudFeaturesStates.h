#pragma once

#include "BombTimer/BombTimerState.h"
#include "DefusingAlert/DefusingAlertState.h"
#include "KillfeedPreserver/KillfeedPreserverState.h"

struct HudFeaturesStates {
    BombTimerState bombTimerState;
    DefusingAlertState defusingAlertState;
    KillfeedPreserverState killfeedPreserverState;
};
