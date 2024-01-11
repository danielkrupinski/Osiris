#pragma once

#include "BombTimerState.h"
#include "DefusingAlertState.h"
#include "KillfeedPreserverState.h"

struct HudFeaturesStates {
    BombTimerState bombTimerState;
    DefusingAlertState defusingAlertState;
    KillfeedPreserverState killfeedPreserverState;
};
