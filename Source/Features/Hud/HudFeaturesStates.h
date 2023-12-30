#pragma once

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"

struct HudFeaturesStates {
    BombTimerState bombTimerState;
    DefusingAlertState defusingAlertState;
    KillfeedPreserverState killfeedPreserverState;
};
