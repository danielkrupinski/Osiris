#pragma once

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"

struct HudFeatures {
    explicit HudFeatures(const ClientPatterns& clientPatterns) noexcept
        : killfeedPreserver{clientPatterns}
    {
    }

    BombTimer bombTimer;
    DefusingAlert defusingAlert;
    KillfeedPreserver killfeedPreserver;
};
