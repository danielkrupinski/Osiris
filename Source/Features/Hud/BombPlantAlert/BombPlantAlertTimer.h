#pragma once

namespace bomb_plant_alert_timer
{

[[nodiscard]] constexpr int remainingTenths(float timeToArmingEnd) noexcept
{
    const auto truncatedTenths = static_cast<int>(timeToArmingEnd * 10.0f);
    if (timeToArmingEnd > 0.0f && truncatedTenths == 0)
        return 1;
    return truncatedTenths;
}

}
