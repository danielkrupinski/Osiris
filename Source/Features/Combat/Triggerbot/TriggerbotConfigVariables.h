#pragma once

#include <cstdint>

#include <Config/ConfigVariable.h>
#include "TriggerbotParams.h"

namespace triggerbot_vars
{
    CONFIG_VARIABLE(Enabled, bool, false);
    CONFIG_VARIABLE(VisibilityCheck, bool, true);
    CONFIG_VARIABLE(FlashImmunity, bool, true);
    // Fire cooldown milliseconds. CS2 weapons cycle at 75-900ms depending on
    // type. 100ms ≈ perfectly-timed AK tapping; 900ms ≈ AWP cycle. Default
    // 120ms covers most rifles without wasting bullets.
    CONFIG_VARIABLE_RANGE(FireCooldownMs, triggerbot_params::kFireCooldownMs);
    // NDC pixel radius on a 1920px reference screen. Tight radius = needs
    // more precise crosshair placement; loose = fires earlier.
    CONFIG_VARIABLE_RANGE(PixelRadius, triggerbot_params::kPixelRadius);
    // Don't fire when holding knife, grenade, bomb, or taser. Prevents
    // accidental equipment throws.
    CONFIG_VARIABLE(FirearmsOnly, bool, true);
}
