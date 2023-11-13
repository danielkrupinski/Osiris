#pragma once

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"

struct HudFeatures {
    BombTimer bombTimer;
    DefusingAlert defusingAlert;
    KillfeedPreserver killfeedPreserver;
};
