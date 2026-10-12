#pragma once

#include "FootstepSound.h"

struct EnemyFootstepSound : FootstepSound {
    static constexpr auto kFadeAwayStart = 0.20f;
    static constexpr auto kFadeAwayDuration = 0.65f;
    static constexpr auto kFadeInDuration = 0.08f;
};
