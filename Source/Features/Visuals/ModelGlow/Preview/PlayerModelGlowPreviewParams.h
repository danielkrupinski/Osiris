#pragma once

#include <CS2/Constants/ColorConstants.h>

namespace player_model_glow_preview_params::player_color_animation_params
{
    constexpr auto kCycleDuration{15.0f};
    constexpr auto kTeamColorSteps{1};
    constexpr auto kPlayerColorSteps{static_cast<int>(cs2::kPlayerColors.size())};
    constexpr auto kSteps{kTeamColorSteps + kPlayerColorSteps};
    constexpr auto kStepDuration{kCycleDuration / kSteps};
}

namespace player_model_glow_preview_params::player_health_animation_params
{
    constexpr auto kMinPlayerHealth{1};
    constexpr auto kMaxPlayerHealth{100};
    constexpr auto kCycleDuration{10.0f};
    constexpr auto kSteps{2 * (kMaxPlayerHealth - kMinPlayerHealth + 1)};
    constexpr auto kStepDuration{kCycleDuration / kSteps};
}

namespace player_model_glow_preview_params
{
    constexpr auto kAnimationCycleDuration{player_color_animation_params::kCycleDuration * player_health_animation_params::kCycleDuration};
}
