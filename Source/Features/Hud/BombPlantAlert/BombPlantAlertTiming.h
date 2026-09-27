#pragma once

#include <Utils/Optional.h>

namespace bomb_plant_alert_timing
{

[[nodiscard]] inline Optional<bool> canFinishArmingBeforeRoundEnd(
    const Optional<float>& armingEndTime,
    const Optional<float>& roundEndTime,
    Optional<bool> isRoundOver) noexcept
{
    if (isRoundOver.valueOr(false))
        return false;
    return armingEndTime.lessEqual(roundEndTime);
}

}
