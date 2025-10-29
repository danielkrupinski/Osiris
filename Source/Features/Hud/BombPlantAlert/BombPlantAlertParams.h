#pragma once

#include <CS2/Classes/Color.h>

namespace bomb_plant_alert_params
{

constexpr cs2::Color kPositiveBackgroundColor{0, 77, 0, 127};
constexpr cs2::Color kNegativeBackgroundColor{77, 0, 0, 127};
constexpr cs2::Color kFallbackBackgroundColor{0, 0, 0, 127};

constexpr cs2::Color kPositiveTimerColor{0, 180, 0};
constexpr cs2::Color kNegativeTimerColor{255, 0, 0};
constexpr cs2::Color kFallbackTimerColor{255, 255, 255};

}
